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

#include "UnreliableHost.h"

UnreliableHost::UnreliableHost() {
    failure = NULL;
    coordFail = NULL;
}

UnreliableHost::~UnreliableHost() {
    neighbors.clear();
    neighborsTemp.clear();
    survivals.clear();
    events.clear();
    cancelAndDelete(failure);
    cancelAndDelete(hostInit);
    if (coordFail != NULL) {
        cancelAndDelete(coordFail);
    }
}

void UnreliableHost::initialize() {
    GatebasedModule::initialize();

    state = static_cast<State>(this->par("state").longValue());
    stage = EH;
    lifespan = par("lifespan");
    WATCH(lifespan);
    failure = new cMessage("failure");
    scheduleAt(simTime() + lifespan, failure);
    hostInit = new cMessage("host_init");
    simtime_t startup = par("startup");
    scheduleAt(simTime() + startup, hostInit);

    WATCH(state);
    WATCH(coord);
    WATCH(stage);
    WATCH_MAP(neighbors);
    WATCH_MAP(neighborsTemp);
    WATCH_MAP(survivals);
    WATCH_MAP(seniorities);
    WATCH_VECTOR(events);
}

void UnreliableHost::dispatchHandler(cMessage *msg) {
    state = static_cast<State>(this->par("state").longValue());
    if (state == On) {
        if (msg->isName("Event")) {
            // run on primary
            Event* event = check_and_cast<Event*>(msg);
            EV << "[" << simTime() << "s] " << "Event [" << event->getFullName()
                      << " (" << event->getSenderId() << ":" << event->getSeq()
                      << ") " << "] from " << event->getSourceName() << " to "
                      << event->getDestName() << " has been received" << endl;
            handleEvent(event);
        } else if (msg->isName("Update")) {
            EV << fullName << " received an update" << endl;
            // run on backups
            Update* event = check_and_cast<Update*>(msg);
            handleUpdate(event);
        } else if (msg->isName("heartbeat")) {
            EV << "[" << simTime() << "s] " << fullName
                      << " receives a heartbeat" << endl;

            // send heartbeat response
            Heartbeat* hb = check_and_cast<Heartbeat*>(msg);
            handleHeartbeat(hb);

            // check coordinator state
            transferStage();

        } else if (msg->isName("failure")) {
            EV << "[" << simTime() << "s] " << fullName << " is failed" << endl;
            fail();
        } else if (msg->isName("host_init")) {
            initNeighbors();

            // add self to purger
            Purger* purger = check_and_cast<Purger*>(
                    getParentModule()->getSubmodule("purger"));
            purger->addHost(fullName);

        } else if (msg->isName("COORD_CHANGE")) {
            CoordChange* cc = check_and_cast<CoordChange*>(msg);
            handleCoordChange(cc);
        } else if (msg->isName("COORD_CHANGE_RESP")) {
            CoordChangeResp* ccr = check_and_cast<CoordChangeResp*>(msg);
            handleCoordChangeResp(ccr);
        } else if (msg->isName("RECONFIG")) {
            Reconfig* reconfig = check_and_cast<Reconfig*>(msg);
            handleReconfig(reconfig);
        } else if (msg->isName("cood_failure")) {
            // TODO for coordinator change test
            fail();
        }
    } else {
        delete msg;
    }
}

void UnreliableHost::initNeighbors() {

    EV << fullName << " initialize group member: " << endl;

    // init neighbor list
    cChannelType *channelType = cChannelType::get(CH_TYPE_FAIR_LOSS);

    for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
            iter++) {
        cModule* module = iter();
        if (dynamic_cast<UnreliableHost*>(module) && this != module) {
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

                // add out-gate to the neighbor list referring to the neighbor
//                neighbors[hostName] = this->gateHalf(hostName.c_str(),
//                        cGate::OUTPUT)->getId();
                survivals[hostName] = this->gateHalf(hostName.c_str(),
                        cGate::OUTPUT)->getId();
                // initialize seniority
                seniorities[hostName] = 0;
            }
        }

        // set up gates to Rendezvous
        if (strcmp(module->getModuleType()->getName(), "Rendezvous") == 0) {
            rendezvousId = createGates(module);
        }

        // set up gates to clients
        if (strcmp(module->getModuleType()->getName(), "Client") == 0) {
            this->createGates(module);
        }

        EV << fullName << " created gates to " << module->getFullName() << endl;
    }
}

void UnreliableHost::handleEvent(Event* event) {
    Update *update = new Update("Update");
    update->setSenderId(event->getSenderId());
    update->setSeq(event->getSeq());
    update->setContent(event->getContent());
    update->setSourceName(event->getSourceName());
    events.push_back(update->getContent());

    for (map<string, int>::iterator it = neighbors.begin();
            it != neighbors.end(); ++it) {
        string moduleName = it->first;
        int gateId = it->second;
        update->setSourceName(fullName);
        update->setDestName(moduleName.c_str());
        transmit(update->dup(), moduleName, gateId);
    }

    string client = event->getSourceName();
    update->setDestName(client.c_str());
    if(hasGate(client.c_str())){
        sendMsg(update->dup(), client,
                    this->gateHalf(client.c_str(), cGate::OUTPUT)->getId());
    }

    // delete the original copy
    delete update;
    // delete the received msg
    delete event;
}

void UnreliableHost::handleUpdate(Update* event) {
    EV << "[" << simTime() << "s] " << "Update [" << event->getFullName()
              << " (" << event->getSenderId() << ":" << event->getSeq() << ") "
              << "] from " << event->getSourceName() << " to "
              << event->getDestName() << " has been received" << endl;
    events.push_back(event->getContent());
    // delete the received msg
    delete event;
}

void UnreliableHost::handleHeartbeat(Heartbeat *hb) {
    // parse message
//    if (ev.isGUI()) {
//        bubble("Received HB");
//    }

    string dest = hb->getSourceName();
    string content = hb->getSurvivals();
    vector<string> hostNames;
    map<string, int> hostSeniorities;
    cStringTokenizer tokenizer(content.c_str(), ";");
    while (tokenizer.hasMoreTokens()) {
        const char* token = tokenizer.nextToken();
        EV << "entries contains: " << token << endl;
        vector<string> pair = cStringTokenizer(token, ":").asVector();
        hostNames.push_back(pair[0]);
        hostSeniorities[pair[0]] = util::strToInt(pair[1]);
    }

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
        // TODO may leave neighbors change to GR
        neighbors.erase(toRemove[i]);
        survivals.erase(toRemove[i]);
        // remove seniority
        seniorities.erase(toRemove[i]);
    }

    // add new hosts
    for (size_t i = 0; i < toAdd.size(); i++) {
        string hostName = toAdd[i];
        if (this->hasGate(hostName.c_str())) {
            if(hasGate(hostName.c_str())){
                survivals[hostName] = this->gateHalf(hostName.c_str(),
                        cGate::OUTPUT)->getId();
            }
        }
    }

    // update seniorities
    for (map<string, int>::iterator it = hostSeniorities.begin();
            it != hostSeniorities.end(); it++) {
        // update seniority
        seniorities[it->first] = it->second;
    }

    hostNames.clear();
    hostSeniorities.clear();
    toRemove.clear();
    toAdd.clear();

    // send heartbeat response
    HeartbeatResponse* hbr = new HeartbeatResponse("hb_resp");
    hbr->setSourceName(fullName);
    hbr->setDestName(dest.c_str());
    hbr->setTimestamp(simTime());
    EV << "[" << simTime() << "s] " << fullName
              << " sends a heartbeat response" << endl;
    sendReliable(hbr, "rendezvous", rendezvousId);

    // delete the received msg
    delete hb;
}

void UnreliableHost::transferStage() {

    bool triggerCood = checkCoord();
    if (!triggerCood) {
        if (stage == CC) {
            if (coord.compare(fullName) == 0) {
                checkCoordChangeResp();
            }
        } else {
            // check whether group reconfiguration needs to be triggered
            checkConfig();
        }
    }
}

bool UnreliableHost::checkCoord() {

    // check whether coordinator is in the Group list
    if (coord.compare(fullName) != 0
            && neighbors.find(coord) == neighbors.end()) {

        // if not, go to coordinator change stage
        stage = CC;

        if (ev.isGUI()) {
            string msg = "coordinator " + coord + " has failed";
            bubble(msg.c_str());
        }
        EV << "[" << simTime() << "s] " << fullName << " found coordinator "
                  << coord << " has failed" << endl;

        // compute the coordinator candidate
        string candidate = coordinator();
        if (candidate.compare(coord) != 0 && candidate.compare(fullName) == 0) {
            if (ev.isGUI()) {
                string msg = string(fullName) + " became coordinator candidate";
                bubble(msg.c_str());
            }

            EV << "[" << simTime() << "s] " << fullName
                      << " requested to become the new coordinator" << endl;

            // if self is the coordinator candidate
            coord = fullName;

            // send coord_query to other members
            CoordChange* coordChange = new CoordChange("COORD_CHANGE");
            coordChange->setCoordName(fullName);
            cMessage* msg = broadcast(coordChange, neighbors);

            // fake the send-to-self process
            coordChange = dynamic_cast<CoordChange*>(msg);
            delete coordChange;
        }
        return true;
    } else {
        return false;
    }
}

bool UnreliableHost::checkConfig() {

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

        if (ev.isGUI()) {
            string msg = "GR is triggered";
            bubble(msg.c_str());
        }
        EV << "[" << simTime() << "s] " << fullName
                  << " entered the reconfiguration stage" << endl;

        stage = GR;
        neighborsTemp.insert(survivals.begin(), survivals.end());
        // coordinator initiate
        if (coord.compare(fullName) == 0) {

            if (ev.isGUI()) {
                string msg = "Start a GR";
                bubble(msg.c_str());
            }
            EV << "[" << simTime() << "s] " << fullName
                      << " start a reconfiguration" << endl;

            // send G' to all hosts on the Survivals list
            Reconfig* reconfig = new Reconfig("RECONFIG");
            stringstream sstream;
            sstream << fullName << ";";
            for (map<string, int>::iterator it = neighborsTemp.begin();
                    it != neighborsTemp.end(); ++it) {
                string hostName = it->first;
                sstream << hostName << ";";
            }
            string members = sstream.str();
            reconfig->setMembers(members.c_str());
            cMessage* msg = broadcast(reconfig, neighborsTemp);
            neighborsTemp.clear();

            // fake the send-to-self process
            reconfig = dynamic_cast<Reconfig*>(msg);
            vector<string> hostNames;
            cStringTokenizer tokenizer(reconfig->getMembers(), ";");
            while (tokenizer.hasMoreTokens()) {
                const char* token = tokenizer.nextToken();
                EV << "Reconfigured entries contains: " << token << endl;
                hostNames.push_back(token);
            }
            neighbors.clear();
            for (size_t i = 0; i < hostNames.size(); i++) {
                if (hostNames[i].compare(fullName) != 0) {
                    neighbors[hostNames[i]] = survivals[hostNames[i]];
                }
            }
            stage = EH;
            delete reconfig;
        }
        return true;
    } else {
        return false;
    }
}

void UnreliableHost::handleCoordChange(CoordChange* cc) {
    string candidateName = cc->getCoordName();

    EV << "[" << simTime() << "s] " << fullName
              << "received coordinator change from " << candidateName << endl;

    bool result = true;
    if (coord.compare(candidateName) != 0) {
        string candidate = this->coordinator();
        if (candidateName.compare(candidate) == 0) {
            coord = candidate;
            coordGate = survivals[candidate];
        }
        else{
            result = false;
        }
    }

    stage = EH;

    CoordChangeResp* ccr = new CoordChangeResp("COORD_CHANGE_RESP");
    ccr->setSenderName(fullName);
    ccr->setResult(result);
    transmit(ccr, candidateName, neighbors[candidateName]);

    delete cc;
}

void UnreliableHost::handleCoordChangeResp(CoordChangeResp*ccr) {

    string respHost = ccr->getSenderName();
    bool respResult = ccr->getResult();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change reply from " << respHost << endl;

    if (respResult) {
        coordChangeReply.insert(respHost);
        checkCoordChangeResp();
    } else {
        // retransmit coordinate change request
        string candidate = coordinator();
        if (candidate.compare(fullName) == 0) {
            EV << "[" << simTime() << "s] " << fullName
                      << " retransmit coordinator change request to "
                      << respHost << endl;

            // if self is the coordinator candidate
            coord = fullName;

            // send coord_query to the reply host
            CoordChange* coordChange = new CoordChange("COORD_CHANGE");
            coordChange->setCoordName(fullName);
            transmit(coordChange, respHost, neighbors[respHost]);
        }
    }

    delete ccr;
}

void UnreliableHost::handleReconfig(Reconfig* reconfig) {
    string senderName = reconfig->getSenderModule()->getFullName();

    EV << "[" << simTime() << "s] " << fullName
              << "received group reconfiguration from " << senderName << endl;

    // update coordinator for new members
    coord = senderName;
    coordGate = survivals[coord];

    vector<string> hostNames;
    cStringTokenizer tokenizer(reconfig->getMembers(), ";");
    while (tokenizer.hasMoreTokens()) {
        const char* token = tokenizer.nextToken();
        EV << "Reconfigured entries contains: " << token << endl;
        hostNames.push_back(token);
    }
    neighbors.clear();
    for (size_t i = 0; i < hostNames.size(); i++) {
        if (hostNames[i].compare(fullName) != 0) {
            neighbors[hostNames[i]] = survivals[hostNames[i]];
        }
    }
    stage = EH;

    if (ev.isGUI()) {
        string msg = "Become a member";
        bubble(msg.c_str());
    }

    delete reconfig;
}

void UnreliableHost::checkCoordChangeResp() {
    bool allReceived = true;
    // check whether all replies from  hosts on the Survivals list have been received
    for (map<string, int>::iterator it = neighbors.begin();
            it != neighbors.end(); ++it) {
        string hostName = it->first;
        if (coordChangeReply.find(hostName) == coordChangeReply.end()) {
            allReceived = false;
            break;
        }
    }

    if (allReceived) {

        // TODO check whether GR is needed

        // switch back to the event handling stage
        stage = EH;

        if (ev.isGUI()) {
            string msg = string(fullName)
                    + " has been elected as a new coordinator";
            bubble(msg.c_str());
        }
        EV << "[" << fullName << "s] has been elected as the new coordinator"
                  << endl;

        // TODO schedule failure for coordinator test
//        coordFail = new cMessage("cood_failure");
//        EV << "coord: " << coord << endl;
//        if (coord.compare(fullName) == 0) {
//            EV << fullName << " initialized a coordinator failure event"
//                      << endl;
//            scheduleAt(simTime() + 0.1, coordFail);
//        }
    }
}

string UnreliableHost::coordinator() {
    unsigned long coordId = sdbm::encode(fullName);
    string coordName = fullName;
    for (map<string, int>::iterator it = survivals.begin();
            it != survivals.end(); it++) {
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

cMessage* UnreliableHost::broadcast(cMessage* message,
        map<string, int>& hosts) {
    GatebasedModule::multicast(message, hosts);
    return message;
}

void UnreliableHost::fail() {
    state = Off;
    par("state") = state;
}
