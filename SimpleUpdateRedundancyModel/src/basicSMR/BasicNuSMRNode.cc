//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "BasicNuSMRNode.h"

Define_Module(BasicNuSMRNode);

BasicNuSMRNode::BasicNuSMRNode() {
    failure = NULL;
    purgerInit = NULL;
    esc = 0;
    escId = 0;
}

BasicNuSMRNode::~BasicNuSMRNode() {
    survivals.clear();
    neighbors.clear();
    neighborsTemp.clear();
    lastStates.clear();
    readies.clear();
    Qr.clear();
    Qd.clear();
    proposals.clear();
    decisions.clear();
    if(failure != NULL){
        cancelAndDelete(failure);
    }
    if(hostInit != NULL){
        cancelAndDelete(hostInit);
    }
    if(EHTimeout != NULL){
        cancelAndDelete(EHTimeout);
    }
    if(applyTimeout != NULL){
        cancelAndDelete(applyTimeout);
    }
    if(purgerInit != NULL){
        cancelAndDelete(purgerInit);
    }
}

void BasicNuSMRNode::initialize() {
    GatebasedModule::initialize();

    state = static_cast<State>(this->par("state").longValue());

    lifespan = par("lifespan");
    EHCycle = par("eh_cycle");
    applyCycle = par("apply_cycle");

    EV << "life span: " << lifespan << endl;

    failure = new cMessage(msg::FAILURE);
    scheduleAt(simTime() + lifespan, failure);
    purgerInit = new cMessage(msg::PURGER_INIT);
    simtime_t purgerInitStep = par("purgerInit");
    scheduleAt(simTime() + purgerInitStep, purgerInit);
    hostInit = new cMessage(msg::HOST_INIT);
    simtime_t startup = par("startup");
    scheduleAt(simTime() + startup, hostInit);
    EHTimeout = new cMessage(msg::EH_TIMEOUT);
    applyTimeout = new cMessage(msg::APPLY_CYCLE);

    WATCH(lifespan);
    WATCH(state);
    WATCH(coord);
    WATCH(stage);
    WATCH(epoch);
    WATCH(groupId);
    WATCH_MAP(neighbors);
    WATCH_MAP(neighborsTemp);
    WATCH_MAP(survivals);
    WATCH_MAP(seniorities);
    // TODO caution! Watching Qr and Qd may not be available
    WATCH_MAP(Qr);
    WATCH_MAP(Qd);
    // TODO for debug purpose
//    WATCH_MAP(proposals);
    WATCH_MAP(decisions);
    WATCH(esc);
    WATCH(escId);
}

void BasicNuSMRNode::dispatchHandler(cMessage *msg) {

    state = static_cast<State>(this->par("state").longValue());
    if (state == On) {
        if (msg->isName(msg::HEARTBEAT)) {
//            EV << "[" << simTime() << "s] " << fullName
//                      << " receives a heartbeat" << endl;

// send heartbeat response
            HBProbe* hb = check_and_cast<HBProbe*>(msg);
            handleHeartbeat(hb);

            // stage transfer
            stageTransfer();

        } else if (msg->isName(msg::FAILURE)) {
            EV << "[" << simTime() << "s] " << fullName << " is failed" << endl;
            fail();
        } else if (msg->isName(msg::PURGER_INIT)) {
            // add self to purger
            Purger* purger = check_and_cast<Purger*>(
                    getParentModule()->getSubmodule("purger"));
            purger->addHost(fullName);
        } else if (msg->isName(msg::HOST_INIT)) {
            seniorities[fullName] = 0;
            groupId = 0;
            // initial stage to avoid starting from EH
            stage = GR;
            epoch = 0;
            initNeighbors();

            scheduleAt(simTime() + EHCycle, EHTimeout);
            scheduleAt(simTime() + applyCycle, applyTimeout);

        } else if (msg->isName(msg::COORD_CHANGE)) {
            CCQuery* cc = check_and_cast<CCQuery*>(msg);
            handleCCQuery(cc);
        } else if (msg->isName(msg::CC_NACK)) {
            CCNAck* ccnack = check_and_cast<CCNAck*>(msg);
            handleCCNAck(ccnack);
        } else if (msg->isName(msg::COORD_CHANGE_REPLY)) {
            CCQueryReply* ccr = check_and_cast<CCQueryReply*>(msg);
            handleCCQueryReply(ccr);
        } else if (msg->isName(msg::LOAD_COORD)) {
            CCEnd* cce = check_and_cast<CCEnd*>(msg);
            handleCCEnd(cce);
        } else if (msg->isName(msg::GROUP_RECONFIG)) {
            GRQuery* gr = check_and_cast<GRQuery*>(msg);
            handleGRQuery(gr);
        } else if (msg->isName(msg::RECONFIG_READY)) {
            GRQueryReply* grr = check_and_cast<GRQueryReply*>(msg);
            handleGRQueryReply(grr);
        } else if (msg->isName(msg::LOAD_CONFIG)) {
            GREnd* gre = check_and_cast<GREnd*>(msg);
            handleGREnd(gre);
        } else if (msg->isName(msg::EVENT)) {
            // run on primary
            Event* event = check_and_cast<Event*>(msg);
//            EV << "[" << simTime() << "s] " << "Event [" << event->getFullName()
//                      << " (" << event->getSenderId() << ":" << event->getSeq()
//                      << ") " << "] from " << event->getSourceName() << " to "
//                      << event->getDestName() << " has been received" << endl;
            handleEvent(event);
        } else if (msg->isName(msg::EH_TIMEOUT)) {
            startPropose();
        } else if (msg->isName(msg::PROPOSAL)) {
            BSMRProposal* proposal = check_and_cast<BSMRProposal*>(msg);
            collectProposal(proposal);
        } else if (msg->isName(msg::DECISION)) {
            BSMRDecision* decision = check_and_cast<BSMRDecision*>(msg);
            handleDecision(decision);
        } else if (msg->isName(msg::APPLY_CYCLE)) {
            applyEvent();
        }
    } else {
        // self-message cannot be deleted here to avoid point double-delete error
        if(!msg->isSelfMessage()){
            delete msg;
        }
    }

}

void BasicNuSMRNode::initNeighbors() {

    EV << fullName << " initialize group member: " << endl;

    // init neighbor list
    cChannelType *channelType = cChannelType::get(CH_TYPE_FAIR_LOSS);

    for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
            iter++) {
        cModule* module = iter();
        if (dynamic_cast<BasicNuSMRNode*>(module) && this != module) {
            string hostName = module->getFullName();
            Purger* purger = check_and_cast<Purger*>(
                    getParentModule()->getSubmodule("purger"));
            if (purger->getState(hostName) == ModuleState::Live) {
                if (!this->hasGate(hostName.c_str())) {
                    this->addGate(hostName.c_str(), cGate::INOUT, false);
                }
                if (!module->hasGate(fullName)) {
                    module->addGate(fullName, cGate::INOUT, false);
                }
                if (!this->gateHalf(hostName.c_str(), cGate::OUTPUT)->isConnected()) {
                    cChannel* flChannel = channelType->create("channel");
                    this->gateHalf(hostName.c_str(), cGate::OUTPUT)->connectTo(
                            module->gateHalf(fullName, cGate::INPUT),
                            flChannel);
                    flChannel->getDisplayString().parse("ls=black,1,solid");
                }
                if (!module->gateHalf(fullName, cGate::OUTPUT)->isConnected()) {
                    cChannel* flChannel = channelType->create("channel");
                    module->gateHalf(fullName, cGate::OUTPUT)->connectTo(
                            this->gateHalf(hostName.c_str(), cGate::INPUT),
                            flChannel);
                    flChannel->getDisplayString().parse("ls=black,1,solid");
                }

                survivals[hostName] = this->gateHalf(hostName.c_str(),
                        cGate::OUTPUT)->getId();
                // initialize seniority
                seniorities[hostName] = 0;
            }
        }

        // set up gates to Rendezvous
        if (strcmp(module->getModuleType()->getName(), "BasicSMRRend") == 0) {
            rendezvousId = createGates(module);
        }

        // set up gates to clients
//        if (strcmp(module->getModuleType()->getName(), "Client") == 0) {
//            this->createGates(module);
//        }

        EV << fullName << " created gates to " << module->getFullName() << endl;
    }
}

void BasicNuSMRNode::handleEvent(Event* event) {
    Qr[event->getContent()] = simTime();
    // delete the received msg
    delete event;
}

void BasicNuSMRNode::startPropose() {
    if (stage == EH && idle) {
        idle = false;
        string received = QrToString();
        BSMRProposal* proposal = new BSMRProposal(msg::PROPOSAL);
        proposal->setSenderName(fullName);
        proposal->setEpoch(epoch);
        proposal->setGroupId(groupId);
        proposal->setReceived(received.c_str());
        if (coord.compare(fullName) != 0) {
            transmit(proposal, coord, coordGate);
        } else {
            collectProposal(proposal);
        }
    } else {
        // for debug purpose
        EV << "[" << simTime() << "s] " << fullName
                  << " cannot start a new consensus instance" << endl;
        if (stage != EH) {
            EV << "[" << simTime() << "s] " << fullName
                      << " is not in the event handling stage" << endl;
        } else {
            EV << "[" << simTime() << "s] " << fullName
                      << " has not finished the last consensus instance"
                      << endl;
        }
    }

    scheduleAt(simTime() + EHCycle, EHTimeout);
}

void BasicNuSMRNode::collectProposal(BSMRProposal* proposal) {
    string senderName = proposal->getSenderName();

    EV << "[" << simTime() << "s] " << fullName << " received PROPOSAL from "
              << senderName << endl;

    int e = proposal->getEpoch();
    int gid = proposal->getGroupId();
    string received = proposal->getReceived();
    // message validation, TODO the first two conditions may not needed
//    if (neighbors.find(senderName) != neighbors.end()
//            && survivals.find(senderName) != survivals.end() && e == epoch
//            && gid == groupId && stage == EH) {
    if (e == epoch && gid == groupId && stage == EH) {
        map<string, simtime_t> events;
        contentToEvents(received, events);
        proposals[senderName] = events;
    }

    checkProposals();

    delete proposal;
}

void BasicNuSMRNode::handleDecision(BSMRDecision* decision) {
    // validate message
    string sender = decision->getSenderName();

    if (ev.isGUI()) {
        string msg = "Received DECISION from Coord";
        bubble(msg.c_str());
    }
    EV << "[" << simTime() << "s] " << fullName << " received decision from "
              << sender << endl;

    int e = decision->getEpoch();
    int gid = decision->getGroupId();
    string dec = decision->getDecision();
    if (e == epoch && gid == groupId && stage == EH) {
        util::convertStrToMap(dec, decisions);

        // remove the corresponding events from Qr
        for (map<int, string>::iterator it = decisions.begin();
                it != decisions.end(); it++) {
            Qr.erase(it->second);
        }
        // move events from decisions to Qd
        Qd.insert(decisions.begin(), decisions.end());

        Update* update = new Update(msg::UPDATE);
        string content = util::convertMapToStr(decisions);
        update->setContent(content.c_str());
        // sourceName and destName must be set for module purge
        update->setSourceName(fullName);
        // broadcast update to clients
        for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
                iter++) {
            cModule* module = iter();
            if (strcmp(module->getModuleType()->getName(), "BasicSMRClient")
                    == 0) {
                string hostName = module->getFullName();
                int gateId = createGates(module);
                update->setDestName(hostName.c_str());
                sendMsg(update->dup(), hostName, gateId);
            }
        }
        delete update;

        decisions.clear();
        // resume consensus initiation
        idle = true;
    }

    delete decision;
}

void BasicNuSMRNode::contentToEvents(string content,
        map<string, simtime_t>& events) {
    vector<string> contentList;
    util::splitString(content, ";", contentList);
    for (size_t i = 0; i < contentList.size(); i++) {
        string value = contentList[i];
        vector<string> pair;
        util::splitString(value, ":", pair);
        simtime_t arrivalTime;
        arrivalTime.parse(pair[0].c_str());
        string event = pair[1];
        events[event] = arrivalTime;
    }
}

string BasicNuSMRNode::QrToString() {
    string events;
    stringstream ss;
    for (map<string, simtime_t>::iterator it = Qr.begin(); it != Qr.end();
            it++) {
        ss << it->second.str() << ":" << it->first << ";";
    }
    return ss.str();
}

void BasicNuSMRNode::applyEvent() {
    // check vector size to avoid error
    if(!Qd.empty()){
        escId = Qd.begin()->first;
        string content = Qd.begin()->second;
        Qd.erase(escId);
        if (esc == 0) {
            esc = sdbm::encode(content.c_str());
        } else {
            string text = util::longToString(esc + sdbm::encode(content.c_str()));
            esc = sdbm::encode(text.c_str());
        }
    }

    scheduleAt(simTime() + applyCycle, applyTimeout);
}

void BasicNuSMRNode::handleHeartbeat(HBProbe *hb) {

    // parse message
//    if (ev.isGUI()) {
//        bubble("Received HB");
//    }

    string dest = hb->getSourceName();
    bool isInit = hb->getIsInit();
    string content = hb->getSurvivals();
    vector<string> hostNames;
    util::splitString(content, ";", hostNames);

    // change survivals list
    vector<string> toRemove;
    vector<string> toAdd;
    for (map<string, int>::iterator it = survivals.begin();
            it != survivals.end(); it++) {
        string host = it->first;
        // apply the vector search algorithm
        if (find(hostNames.begin(), hostNames.end(), host) == hostNames.end()) {
            toRemove.push_back(it->first);
        }
    }
    for (size_t i = 0; i < hostNames.size(); i++) {
        if (survivals.find(hostNames[i]) == survivals.end()
                && strcmp(fullName, hostNames[i].c_str()) != 0) {
            toAdd.push_back(hostNames[i]);
        }
    }
    // remove failed hosts
    for (size_t i = 0; i < toRemove.size(); i++) {
        // TODO May leave neighbors change to GR.
        // If so, then map<string, int> map3 = intersectMaps(neighbors, survivals) should be applied
        neighbors.erase(toRemove[i]);
        survivals.erase(toRemove[i]);
        // remove seniority
        seniorities.erase(toRemove[i]);
    }

    // add new hosts
    for (size_t i = 0; i < toAdd.size(); i++) {
        string hostName = toAdd[i];
        if (hasGate(hostName.c_str())) {
            int outgateId =
                    this->gateHalf(hostName.c_str(), cGate::OUTPUT)->getId();
            survivals[hostName] = outgateId;
        }
    }

    if (isInit) {
        groupId = 1;
        neighbors.insert(survivals.begin(), survivals.end());
        for (map<string, int>::iterator it = neighbors.begin();
                it != neighbors.end(); it++) {
            seniorities[it->first] = 1;
        }
        seniorities[fullName] = 1;
    }

    hostNames.clear();
    toRemove.clear();
    toAdd.clear();

    // send heartbeat response
    HBResponse* hbr = new HBResponse(msg::HEARTBEAT_RESPONSE);
    hbr->setSourceName(fullName);
    hbr->setDestName(dest.c_str());
    hbr->setTimestamp(simTime());
//    EV << "[" << simTime() << "s] " << fullName << " sends a heartbeat response"
//              << endl;
    sendReliable(hbr, "rendezvous", rendezvousId);

    // delete the received msg
    delete hb;
}

void BasicNuSMRNode::stageTransfer() {
    // check whether coordinator change needs to be triggered
    bool triggerCood = checkCoord();
    if (!triggerCood) {
        if (stage == CC) {
            if (coordCand.compare(fullName) == 0) {
                checkLastStates();
            }
        } else {
            // check whether group reconfiguration needs to be triggered
            bool triggerReconfig = checkConfig();
            if (!triggerReconfig) {
                if (stage == GR) {
                    if (coord.compare(fullName) == 0) {
                        checkReadies();
                    }
                } else {
                    if (coord.compare(fullName) == 0) {
                        checkProposals();
                    }
                }
            }
        }
    }
}

bool BasicNuSMRNode::checkCoord() {

    // check whether coordinator is survived
    if (seniorities[fullName] > 0 && coordCand.compare(fullName) != 0
            && ((coord.compare(fullName) != 0)
                    && survivals.find(coord) == survivals.end())) {

        if (ev.isGUI()) {
            string msg = "coordinator " + coord + " has failed";
            bubble(msg.c_str());
        }
        EV << "[" << simTime() << "s] " << fullName << " found coordinator "
                  << coord << " has failed" << endl;

        // if not, go to coordinator change stage
        stage = CC;
        coord.clear();
        // abort the current consensus instance
        idle = true;

        // compute the coordinator candidate
        string candidate = coordinator();
        if (candidate.compare(fullName) == 0) {
            if (ev.isGUI()) {
                string msg = string(fullName) + " became coordinator candidate";
                bubble(msg.c_str());
            }

            EV << "[" << simTime() << "s] " << fullName
                      << " requested to become the new coordinator" << endl;

            // if self is the coordinator candidate
            coordCand = fullName;
            lastStates.clear();
            neighborsTemp.clear();
            proposals.clear();

            // send coord_query to other members
            CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
            coordChange->setCandidateName(fullName);
            cMessage* msg = broadcast(coordChange, neighbors);

            // fake the send-to-self process
            coordChange = dynamic_cast<CCQuery*>(msg);
            LastState ls;
            // load member list
            vector<string> members = util::loadMapKey(neighbors, 1, fullName);
            ls.groupMembers = members;
            ls.epoch = epoch;
            ls.groupId = groupId;
            ls.escId = escId;
            ls.esc = esc;
            ls.DL.insert(Qd.begin(), Qd.end());
            ls.DL.insert(decisions.begin(), decisions.end());
            lastStates[fullName] = ls;
            checkLastStates();

            delete coordChange;
        }
        return true;
    } else {
        return false;
    }
}

bool BasicNuSMRNode::checkConfig() {
    bool triggerGR = false;
    for (map<string, int>::iterator it = survivals.begin();
            it != survivals.end(); it++) {
        string hostName = it->first;
        if (neighbors.find(hostName) == neighbors.end()
                && neighborsTemp.find(hostName) == neighborsTemp.end()) {
            triggerGR = true;
            break;
        }
    }
    if (triggerGR) {
        // abort the current consensus instance
        idle = true;
        // TODO need to verify the location of G' assignment
        neighborsTemp.clear();
        neighborsTemp.insert(survivals.begin(), survivals.end());

        // coordinator initiate
        if (coord.compare(fullName) == 0) {
            if (ev.isGUI()) {
                string msg = "Start a GR";
                bubble(msg.c_str());
            }
            EV << "[" << simTime() << "s] " << fullName
                      << " start a reconfiguration" << endl;

            stage = GR;
            groupId++;
            readies.clear();
            proposals.clear();

            // send G' to all hosts on the Survivals list
            GRQuery* reconfig = new GRQuery(msg::GROUP_RECONFIG);
            reconfig->setSenderName(fullName);
            reconfig->setEpoch(epoch);
            reconfig->setGroupId(groupId);
            cMessage* msg = broadcast(reconfig, neighborsTemp);

            // fake the send-to-self process
            reconfig = dynamic_cast<GRQuery*>(msg);
            Ready ready;
            // load member list
            vector<string> members = util::loadMapKey(neighbors, 1, fullName);
            ready.groupMembers = members;
            ready.epoch = reconfig->getEpoch();
            ready.groupId = reconfig->getGroupId();
            ready.escId = escId;
            ready.esc = esc;
            ready.DL.insert(Qd.begin(), Qd.end());
            ready.DL.insert(decisions.begin(), decisions.end());
            readies[fullName] = ready;
            checkReadies();

            delete reconfig;
        }
        return true;
    } else {
        return false;
    }
}

void BasicNuSMRNode::handleCCQuery(CCQuery* cc) {

    string candidateName = cc->getCandidateName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change from " << candidateName << endl;

    if (survivals.find(candidateName) != survivals.end()) {

        string candidate = this->coordinator();
        if (candidateName.compare(candidate) == 0) {

            stage = CC;
            coord.clear();

            CCQueryReply* ccr = new CCQueryReply(msg::COORD_CHANGE_REPLY);
            ccr->setSenderName(fullName);
            ccr->setEpoch(epoch);
            ccr->setGroupId(groupId);
            // send events for state sync
            ccr->setState(buildStateForSync().c_str());
            ccr->setEscId(escId);
            ccr->setEsc(esc);
            // concatenate neighbor list to string content
            string members = util::convertMapKeyToStr(neighbors, 1, fullName);
            ccr->setGroupMembers(members.c_str());
            transmit(ccr, candidateName, neighbors[candidateName]);
        } else {
            CCNAck* ccr = new CCNAck(msg::CC_NACK);
            ccr->setSenderName(fullName);
            transmit(ccr, candidateName, neighbors[candidateName]);
        }
    }

    delete cc;
}

void BasicNuSMRNode::handleGRQuery(GRQuery* gr) {
    string senderName = gr->getSenderName();
    int e = gr->getEpoch();
    int gid = gr->getGroupId();

    EV << "[" << simTime() << "s] " << fullName
              << " received reconfiguration query from " << senderName << endl;

    // message validation
    if (e >= epoch && gid > groupId && stage != CC) {

        stage = GR;

        if (ev.isGUI()) {
            string msg = "GR is triggered";
            bubble(msg.c_str());
        }
        EV << "[" << simTime() << "s] " << fullName
                  << " entered the reconfiguration stage" << endl;

        groupId = gr->getGroupId();

        // update coordinator info for new members
        if (epoch == 0) {
            epoch = e;
            coord = senderName;
            coordGate = survivals[coord];
        }

        GRQueryReply* grr = new GRQueryReply(msg::RECONFIG_READY);
        grr->setSenderName(fullName);
        // concatenate neighbor list to string content
        string members = util::convertMapKeyToStr(neighbors, 1, fullName);
        grr->setGroupMembers(members.c_str());
        grr->setEpoch(epoch);
        grr->setGroupId(groupId);
        grr->setEscId(escId);
        grr->setEsc(esc);
        // send events for state sync
        grr->setState(buildStateForSync().c_str());
        transmit(grr, senderName, survivals[senderName]);
    }

    delete gr;
}

void BasicNuSMRNode::handleCCNAck(CCNAck* ccnack) {
    string senderName = ccnack->getSenderName();
    // retransmit coordinate change request
    string candidate = coordinator();
    if (candidate.compare(fullName) == 0) {
        EV << "[" << simTime() << "s] " << fullName
                  << " retransmit coordinator change request to " << senderName
                  << endl;

        // if self is the coordinator candidate
        coord = fullName;

        // send coord_query to the reply host
        CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
        coordChange->setCandidateName(fullName);
        transmit(coordChange, senderName, neighbors[senderName]);
    } else {
        coordCand.clear();
    }

    delete ccnack;
}

void BasicNuSMRNode::handleCCQueryReply(CCQueryReply* ccr) {
    string sender = ccr->getSenderName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change reply from " << sender << endl;

    // only handle messages from survived members, TODO but this condition may not needed
    if (neighbors.find(sender) != neighbors.end()
            && survivals.find(sender) != survivals.end()) {
        LastState ls;
        // split string-style neighbors to list of neighbors
        string members = ccr->getGroupMembers();
        vector<string> hostNames;
        util::splitString(members, ";", hostNames);
        // end split
        ls.groupMembers = hostNames;
        ls.epoch = ccr->getEpoch();
        ls.groupId = ccr->getGroupId();
        ls.escId = ccr->getEscId();
        ls.esc = ccr->getEsc();
        string state = ccr->getState();
        util::convertStrToMap(state, ls.DL);
        lastStates[sender] = ls;

        checkLastStates();
    }

    delete ccr;
}

void BasicNuSMRNode::handleGRQueryReply(GRQueryReply* grr) {
    string sender = grr->getSenderName();
    int e = grr->getEpoch();
    int gid = grr->getGroupId();

    EV << "[" << simTime() << "s] " << fullName
              << " received group reconfiguration reply from " << sender
              << endl;

    // only handle messages from survived hosts in neighborsTemp, TODO but this condition may not needed
    if (e == epoch && gid == groupId && stage != CC
            && neighborsTemp.find(sender) != neighborsTemp.end()
            && survivals.find(sender) != survivals.end()) {
        Ready ready;
        ready.epoch = grr->getEpoch();
        ready.groupId = grr->getGroupId();
        ready.escId = grr->getEscId();
        ready.esc = grr->getEsc();
        string state = grr->getState();
        util::convertStrToMap(state, ready.DL);
        readies[sender] = ready;
    }

    checkReadies();

    delete grr;
}

void BasicNuSMRNode::handleCCEnd(CCEnd* cce) {
    string senderName = cce->getSenderName();
    if (survivals.find(senderName) != survivals.end()
            || senderName.compare(fullName) == 0) {
        string c = coordinator();
        if (c.compare(senderName) == 0) {
            coord = senderName;
            if (senderName.compare(fullName) != 0) {
                coordGate = survivals[senderName];
            }
            epoch = cce->getEpoch() + 1;
            groupId = cce->getGroupId();

            // load synchronized DL
            Qd.clear();
            string state = cce->getState();
            util::convertStrToMap(state, Qd);
            escId = cce->getEscId();
            esc = cce->getEsc();

            // split string-style neighbors to list of neighbors
            string members = cce->getGroupMembers();
            vector<string> memberList;
            util::splitString(members, ";", memberList);
            // load new group member list
            neighbors.clear();
            for (size_t i = 0; i < memberList.size(); i++) {
                string hostName = memberList[i];
                if (hostName.compare(fullName) != 0
                        && survivals.find(hostName) != survivals.end()) {
                    neighbors[hostName] = survivals[hostName];
                }
            }

            coordCand.clear();
            stage = EH;

            if (ev.isGUI()) {
                string msg = string(fullName) + " load the new coordinator";
                bubble(msg.c_str());
            }
            EV << "[" << simTime() << "s] " << fullName
                      << " loaded the new coordinator" << endl;
        } else {
            EV << "[" << simTime() << "s] coordinator candidate " << fullName
                      << " is from the previous epoch" << endl;
        }
    } else {
        EV << "[" << simTime() << "s] coordinator candidate " << fullName
                  << " has failed" << endl;
    }

    delete cce;
}

void BasicNuSMRNode::handleGREnd(GREnd* gre) {
    string senderName = gre->getSenderName();
    int e = gre->getEpoch();
    int gid = gre->getGroupId();
//    if (senderName.compare(coord) == 0 && e == epoch && gid == groupId && stage != CC) {
    if (e == epoch && gid == groupId && stage != CC) {
        // load synchronized DL
        Qd.clear();
        string state = gre->getState();
        util::convertStrToMap(state, Qd);
        escId = gre->getEscId();
        esc = gre->getEsc();

        // split string-style neighbors to list of neighbors
        string members = gre->getGroupMembers();
        vector<string> memberList;
        util::splitString(members, ";", memberList);
        // load new group member list
        neighbors.clear();
        seniorities.clear();
        for (size_t i = 0; i < memberList.size(); i++) {
            string value = memberList[i];
            vector<string> pair;
            util::splitString(value, ":", pair);
            string hostName = pair[0];
            int seniority = util::strToInt(pair[1]);
            if (hostName.compare(fullName) != 0) {
                if (survivals.find(hostName) != survivals.end()) {
                    neighbors[hostName] = survivals[hostName];
                    // update seniority
                    seniorities[hostName] = seniority + 1;
                }
            } else {
                // update seniority
                seniorities[hostName] = seniority + 1;
            }
        }
        // resume event handling
        neighborsTemp.clear();
        stage = EH;

        if (ev.isGUI()) {
            string msg = string(fullName) + " load the new configuration";
            bubble(msg.c_str());
        }
        EV << "[" << simTime() << "s] " << fullName
                  << " loaded the new configuration" << endl;
    } else {
        EV << "[" << simTime() << "s] reconfiguration from " << fullName
                  << " is invalid" << endl;
    }

    delete gre;
}

void BasicNuSMRNode::checkLastStates() {

    bool allReceived = true;

    map<string, int> survivedNeighbors = util::intersectMaps(survivals,
            neighbors);
    survivedNeighbors[fullName] = -1;
    // check whether all replies from  hosts on the Survivals list have been received
    for (map<string, int>::iterator it = survivedNeighbors.begin();
            it != survivedNeighbors.end(); ++it) {
        string hostName = it->first;
        if (lastStates.find(hostName) == lastStates.end()) {
            allReceived = false;
            break;
        }
    }

    if (allReceived) {

        if (ev.isGUI()) {
            string msg = "Received all LAST_STATE";
            bubble(msg.c_str());
        }

        LastState ls = computeLastState();
        CCEnd* cce = new CCEnd(msg::LOAD_COORD);
        cce->setSenderName(fullName);
        cce->setEpoch(ls.epoch);
        cce->setGroupId(ls.groupId);
        // concatenate DL to string content
        string state = util::convertMapToStr(ls.DL);
        cce->setState(state.c_str());
        cce->setEscId(ls.escId);
        cce->setEsc(ls.esc);
        // concatenate neighbor list to string content
        string members = util::convertListToStr(ls.groupMembers, 0);
        cce->setGroupMembers(members.c_str());
        cMessage* msg = broadcast(cce, neighbors);

        // fake the send-to-self process,  switch back to the event handling stage
        cce = check_and_cast<CCEnd*>(msg);
        handleCCEnd(cce);
    }

}

void BasicNuSMRNode::checkReadies() {
    if (stage != CC) {
        bool allReceived = true;
        map<string, int> survivedNeighborsTemp = util::intersectMaps(survivals,
                neighborsTemp);
        survivedNeighborsTemp[fullName] = -1;
        // check whether all replies from  hosts on the Survivals list have been received
        for (map<string, int>::iterator it = survivedNeighborsTemp.begin();
                it != survivedNeighborsTemp.end(); ++it) {
            string hostName = it->first;
            if (readies.find(hostName) == readies.end()) {
                allReceived = false;
                break;
            }
        }

        if (allReceived) {

            if (ev.isGUI()) {
                string msg = "Received all LAST_STATE";
                bubble(msg.c_str());
            }

            Ready ready = computeReady();
            GREnd* gre = new GREnd(msg::LOAD_CONFIG);
            gre->setSenderName(fullName);
            gre->setEpoch(epoch);
            gre->setGroupId(groupId);
            // concatenate DL to string content
            string state = util::convertMapToStr(ready.DL);
            gre->setState(state.c_str());
            gre->setEscId(ready.escId);
            gre->setEsc(ready.esc);
            // concatenate neighbor list to string content
            string members = util::convertListToStr(ready.groupMembers, 0);
            gre->setGroupMembers(members.c_str());
            cMessage* msg = broadcast(gre, neighborsTemp);

            // fake the send-to-self process,  switch back to the event handling stage
            gre = check_and_cast<GREnd*>(msg);
            handleGREnd(gre);
        }
    }
}

void BasicNuSMRNode::checkProposals() {
    if (stage == EH) {
        bool allReceived = true;
        map<string, int> survivedNeighbors = util::intersectMaps(survivals,
                neighbors);
        survivedNeighbors[fullName] = -1;
        // check whether all replies from  hosts on the Survivals list have been received
        for (map<string, int>::iterator it = survivedNeighbors.begin();
                it != survivedNeighbors.end(); ++it) {
            string hostName = it->first;
            if (proposals.find(hostName) == proposals.end()) {
                allReceived = false;
                break;
            }
        }

        if (allReceived) {

            if (ev.isGUI()) {
                string msg = "Received all Proposals";
                bubble(msg.c_str());
            }
            EV << "[" << simTime() << "s] " << fullName
                      << " start processing proposals" << endl;

            // sort events in ascending order in arrival time
            map<string, simtime_t> E;
            for (map<string, map<string, simtime_t> >::iterator it =
                    proposals.begin(); it != proposals.end(); it++) {
                for (map<string, simtime_t>::iterator it2 = it->second.begin();
                        it2 != it->second.end(); it2++) {
                    string event = it2->first;
                    simtime_t arrival = it2->second;
                    if (E.find(event) == E.end()) {
                        E[event] = arrival;
                    } else if (E[event] > arrival) {
                        E[event] = arrival;
                    }
                }
            }
            // sort events by arrival time
            multimap<simtime_t, string> SE;
            for (map<string, simtime_t>::iterator it = E.begin(); it != E.end();
                    it++) {
                SE.insert(pair<simtime_t, string>(it->second, it->first));
            }
            // assign events IDs
            map<int, string> SQ;
            int lastEid = Qd.empty() ? 0 : Qd.rbegin()->first;
            for (multimap<simtime_t, string>::iterator it = SE.begin();
                    it != SE.end(); it++) {
                lastEid++;
                SQ[lastEid] = it->second;
            }

            // create DECISION message
            BSMRDecision* decisionMsg = new BSMRDecision(msg::DECISION);
            decisionMsg->setSenderName(fullName);
            decisionMsg->setEpoch(epoch);
            decisionMsg->setGroupId(groupId);
            decisionMsg->setDecision(util::convertMapToStr(SQ).c_str());

            // broadcast DECISION message
            cMessage* msg = broadcast(decisionMsg, neighbors);

            // empty proposals
            proposals.clear();

            // fake the send-to-self process
            decisionMsg = check_and_cast<BSMRDecision*>(msg);
            handleDecision(decisionMsg);
        }
    }
}

string BasicNuSMRNode::buildStateForSync() {
    map<int, string> DL = Qd;
    DL.insert(decisions.begin(), decisions.end());
    string state = util::convertMapToStr(DL);
    return state;
}

LastState BasicNuSMRNode::syncState(map<string, LastState>& lsMap) {
    LastState ls;
    int largestEid = 0;
    for (map<string, LastState>::iterator it = lsMap.begin(); it != lsMap.end();
            it++) {
        LastState tmp = it->second;
        int lastEid = tmp.escId;
        map<int, string> DL = tmp.DL;
        if (!DL.empty()) {
            lastEid = DL.rbegin()->first;
        }
        if (lastEid >= largestEid) {
            largestEid = lastEid;
            ls.DL = tmp.DL;
            ls.escId = tmp.escId;
            ls.esc = tmp.esc;
        }
    }
    return ls;
}

Ready BasicNuSMRNode::syncState(map<string, Ready>& readyMap) {
    Ready ready;
    int largestEid = 0;
    for (map<string, Ready>::iterator it = readyMap.begin(); it != readyMap.end();
            it++) {
        Ready tmp = it->second;
        int lastEid = tmp.escId;
        map<int, string> DL = tmp.DL;
        if (!DL.empty()) {
            lastEid = DL.rbegin()->first;
        }
        if (lastEid >= largestEid) {
            largestEid = lastEid;
            ready.DL = tmp.DL;
            ready.escId = tmp.escId;
            ready.esc = tmp.esc;
        }
    }
    return ready;
}

string BasicNuSMRNode::coordinator() {
    unsigned long coordId = sdbm::encode(fullName);
    string coordName = fullName;
    for (map<string, int>::iterator it = neighbors.begin();
            it != neighbors.end(); it++) {
        string fullName = it->first;
        unsigned long id = sdbm::encode(fullName.c_str());
        if (seniorities[fullName] > seniorities[coordName]
                || (seniorities[fullName] == seniorities[coordName]
                        && id > coordId)) {
            coordId = id;
            coordName = it->first;
        }
    }
    return coordName;
}

LastState BasicNuSMRNode::computeLastState() {
    LastState ls = syncState(lastStates);
    // initial value for comparison purpose
    ls.epoch = epoch;
    ls.groupId = groupId;
    for (map<string, LastState>::iterator it = lastStates.begin();
            it != lastStates.end(); it++) {
        LastState tmp = it->second;
        if (tmp.epoch > ls.epoch) {
            ls.epoch = tmp.epoch;
        }
        if (tmp.groupId >= ls.groupId) {
            ls.groupId = tmp.groupId;
            ls.groupMembers = tmp.groupMembers;
        }
//        // merge DL to the the superset of events
//        ls.DL.insert(tmp.DL.begin(), tmp.DL.end());
    }
    return ls;
}

Ready BasicNuSMRNode::computeReady() {
    Ready ready = syncState(readies);
    // retrieve new group members from G'
    for (map<string, int>::iterator it = neighborsTemp.begin();
            it != neighborsTemp.end(); it++) {
        string pair = it->first + ":" + util::intToStr(seniorities[it->first]);
        ready.groupMembers.push_back(pair);
    }
    string selfPair = string(fullName) + ":"
            + util::intToStr(seniorities[fullName]);
    ready.groupMembers.push_back(selfPair);
//    for (map<string, Ready>::iterator it = readies.begin(); it != readies.end();
//            it++) {
//        Ready tmp = it->second;
//        ready.DL.insert(tmp.DL.begin(), tmp.DL.end());
//    }
    return ready;
}

cMessage* BasicNuSMRNode::broadcast(cMessage* message, map<string, int>& hosts) {
    GatebasedModule::multicast(message, hosts);
    return message;
}

void BasicNuSMRNode::fail() {
    state = Off;
    par("state") = state;
}
