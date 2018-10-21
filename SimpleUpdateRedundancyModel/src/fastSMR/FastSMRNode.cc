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

#include "FastSMRNode.h"

Define_Module(FastSMRNode);

FastSMRNode::FastSMRNode() {
    failure = NULL;
    purgerInit = NULL;
    esc = 0;
    escId = 0;
    round = 0;
    consensus = 0;
    appliedRound = 0;
    isMember = false;
}

FastSMRNode::~FastSMRNode() {
//    survivals.clear();
//    neighbors.clear();
//    neighborsTemp.clear();
//    lastStates.clear();
//    readies.clear();
//    Qp.clear();
//    Qd.clear();
//    proposals.clear();
//    decisions.clear();
    agreements.clear();
    if (failure != NULL) {
        cancelAndDelete(failure);
    }
    if (hostInit != NULL) {
        cancelAndDelete(hostInit);
    }
    if (EHTimeout != NULL) {
        cancelAndDelete(EHTimeout);
    }
    if (applyTimeout != NULL) {
        cancelAndDelete(applyTimeout);
    }
    if (ECStart != NULL) {
        cancelAndDelete(ECStart);
    }
    if (purgerInit != NULL) {
        cancelAndDelete(purgerInit);
    }
}

void FastSMRNode::initialize() {
    GatebasedModule::initialize();

    state = static_cast<State>(this->par("state").longValue());

    lifespan = par("lifespan");
    roundCycle = par("roundCycle");
    applyCycle = par("applyCycle");
    cycleStart = par("cycleStart");
    simtime_t startup = par("startup");

    EV << "life span: " << lifespan << endl;

    failure = new cMessage(msg::FAILURE);
    scheduleAt(simTime() + lifespan, failure);
    purgerInit = new cMessage(msg::PURGER_INIT);
    simtime_t purgerInitStep = par("purgerInit");
    scheduleAt(simTime() + purgerInitStep, purgerInit);
    hostInit = new cMessage(msg::HOST_INIT);
    scheduleAt(simTime() + startup, hostInit);
    EHTimeout = new cMessage(msg::EH_TIMEOUT);
    applyTimeout = new cMessage(msg::APPLY_CYCLE);
    ECStart = new cMessage(msg::EC_START);
    scheduleAt(cycleStart, ECStart);

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
    WATCH_MAP(clients);
    WATCH(esc);
    WATCH(escId);
    WATCH_SET(toPropose);
    WATCH_SET(proposed);
    WATCH_MAP(senders);
    WATCH_MAP(sors);
    WATCH(round);
    WATCH(appliedRound);

    WATCH_MAP(Qr_w);
    WATCH_MAP(Qd_w);
    WATCH_MAP(Qp_w);
    WATCH_MAP(Qa);
    WATCH_MAP(Qd_bk_w);
//    // for debug purpose
//    WATCH_MAP(proposals); non-watchable
//    WATCH_MAP(decisions); non-watchable
//    WATCH_VECTOR(agreements); non-watchable
    WATCH(consensus);
}

void FastSMRNode::dispatchHandler(cMessage *msg) {

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

            // check whether there is any event in Qp
            handleEvent();

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

//            scheduleAt(simTime() + roundCycle, EHTimeout);
            scheduleAt(simTime() + applyCycle, applyTimeout);
        } else if (msg->isName(msg::COORD_CHANGE)) {
            CCQuery* cc = check_and_cast<CCQuery*>(msg);
            handleCCQuery(cc);
        } else if (msg->isName(msg::CC_NACK)) {
            CCNAck* ccnack = check_and_cast<CCNAck*>(msg);
            handleCCNAck(ccnack);
        } else if (msg->isName(msg::COORD_CHANGE_REPLY)) {
            FSMRCCQueryReply* ccr = check_and_cast<FSMRCCQueryReply*>(msg);
            handleCCQueryReply(ccr);
        } else if (msg->isName(msg::LOAD_COORD)) {
            FSMRCCEnd* cce = check_and_cast<FSMRCCEnd*>(msg);
            handleCCEnd(cce);
        } else if (msg->isName(msg::GROUP_RECONFIG)) {
            GRQuery* gr = check_and_cast<GRQuery*>(msg);
            handleGRQuery(gr);
        } else if (msg->isName(msg::RECONFIG_READY)) {
            GRQueryReply* grr = check_and_cast<GRQueryReply*>(msg);
            handleGRQueryReply(grr);
        } else if (msg->isName(msg::LOAD_CONFIG)) {
            FSMRGREnd* gre = check_and_cast<FSMRGREnd*>(msg);
            handleGREnd(gre);
        } else if (msg->isName(msg::EVENT)) {
            // run on primary
            Event* event = check_and_cast<Event*>(msg);
//            EV << "[" << simTime() << "s] " << "Event [" << event->getFullName()
//                      << " (" << event->getSenderId() << ":" << event->getSeq()
//                      << ") " << "] from " << event->getSourceName() << " to "
//                      << event->getDestName() << " has been received" << endl;
            receiveEvent(event);
        } else if (msg->isName(msg::CONSENSUS_QUERY)) {
            ConsensusQuery * cq = check_and_cast<ConsensusQuery*>(msg);
            this->startPropose(cq);
        } else if (msg->isName(msg::PROPOSAL)) {
            FSMRProposal* proposal = check_and_cast<FSMRProposal*>(msg);
            collectProposal(proposal);
        } else if (msg->isName(msg::DECISION)) {
            FSMRDecision* decision = check_and_cast<FSMRDecision*>(msg);
            handleDecision(decision);
        }
//        else if (msg->isName(msg::AGREEMENT)) {
//            FSMRAgree* agreement = check_and_cast<FSMRAgree*>(msg);
//            collectAgreement(agreement);
//        }
//        else if (msg->isName(msg::FINAL)) {
//            FSMRFinal* final = check_and_cast<FSMRFinal*>(msg);
//            handleFinal(final);
//        }
        else if (msg->isName(msg::APPLY_CYCLE)) {
            applyEvent();
        } else if (msg->isName(msg::EC_START)) {
            collectRoundEvents();
        } else if (msg->isName(msg::CYCLE_EVENT)) {
            CycleEvent* ce = check_and_cast<CycleEvent*>(msg);
            deliverCycleEvent(ce);
        } else if (msg->isName(msg::ROUND_QUERY)) {
            RoundQuery* query = check_and_cast<RoundQuery*>(msg);
            handleRoundQuery(query);
        } else if (msg->isName(msg::ROUND_QUERY_REPLY)) {
            RoundQueryReply* rqr = check_and_cast<RoundQueryReply*>(msg);
            handleRoundQueryReply(rqr);
        } else if (msg->isName(msg::INIT_JOIN)) {
            InitJoin* initJoin = check_and_cast<InitJoin*>(msg);
            handleJoin(initJoin);
        }
    } else {
        // self-message cannot be deleted here to avoid point double-delete error
        if (!msg->isSelfMessage()) {
            delete msg;
        }
    }

}

void FastSMRNode::handleHeartbeat(HBProbe *hb) {

    // parse message
//    if (ev.isGUI()) {
//        bubble("Received HB");
//          EV << fullName << " Received HB" << endl;
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
        initNode();
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

void FastSMRNode::initNode() {
    groupId = 1;
    neighbors.insert(survivals.begin(), survivals.end());
    for (map<string, int>::iterator it = neighbors.begin();
            it != neighbors.end(); it++) {
        seniorities[it->first] = 1;
    }
    seniorities[fullName] = 1;
    isMember = true;
}

void FastSMRNode::handleJoin(InitJoin* initJoin) {
    int seq_s = initJoin->getSeq();
    unsigned long senderId = initJoin->getSenderId();
    string clientName = initJoin->getSourceName();
    // decrease one round
    // the (specified) initial round of the first received message
//    senders[senderId] = joining sequence # - Round of (cycleStart);
    // since the start round is round1, for simplicity, Round of (cycleStart) = 1
    senders[senderId] = seq_s - 1;
    // for sor observation
    sors[senderId] = 0;

    Join* join = new Join(msg::JOIN);
    join->setSenderName(fullName);
    join->setCycleLength(roundCycle);
    join->setJoinTime(cycleStart - roundCycle);

    sendReliable(join, clientName, clients[clientName]);

    delete initJoin;
}

void FastSMRNode::initNeighbors() {

    EV << fullName << " initialize group member: " << endl;

    // init neighbor list
    cChannelType *channelType = cChannelType::get(CH_TYPE_FAIR_LOSS);

    for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
            iter++) {
        cModule* module = iter();
        if (dynamic_cast<FastSMRNode*>(module) && this != module) {
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
        if (strcmp(module->getModuleType()->getName(), "FastSMRRend") == 0) {
            rendezvousId = createGates(module);
        }

        // set up gates to clients
        if (strcmp(module->getModuleType()->getName(), "FastSMRClient") == 0) {
            clients[module->getFullName()] = createGates(module);
        }

        EV << fullName << " created gates to " << module->getFullName() << endl;
    }
}

void FastSMRNode::receiveEvent(Event* event) {
    unsigned long senderId = event->getSenderId();
    int seq = event->getSeq();
    if (seq >= sors[senderId]) {
        string content = event->getContent();
        Qr[senderId][seq] = content;
    }

    // for observation
    decodeQr();

    delete event;
}

void FastSMRNode::collectRoundEvents() {
    if (isMember) {
        round++;
        EV << fullName << " round: " << round << endl;

        // on Coordinator, if the events of the round have been received from other hosts before
        if (Qp.find(round) == Qp.end()) {
            map<unsigned long, string> QR;

            // determine QR
            for (map<unsigned long, int>::iterator it = senders.begin();
                    it != senders.end(); it++) {
                unsigned long senderId = it->first;
                // determine the sequence of round: seq_s + (r - r_s) where r = round
                int& sor = sors[senderId];
                sor++;
//                EV << "sor from increment is: " << sor << endl;
                sor = it->second + round;
                sors[senderId] = sor;
//                EV << "sor from computation is: " << sor << endl;

//                EV << senderId << "'s sequence of round (SOR): " << sor << endl;

                // check whether the event of the round has been received
                if (Qr.find(senderId) != Qr.end()
                        && Qr[senderId].find(sor) != Qr[senderId].end()) {
                    QR[senderId] = Qr[senderId][sor];
                    // remove events from Qr
                    Qr[senderId].erase(sor);

                } else {
                    QR[senderId].clear();
                }
            }

            bool complete = true;
            for (map<unsigned long, string>::iterator it = QR.begin();
                    it != QR.end(); it++) {
                if (it->second.empty()) {
                    complete = false;
                    break;
                }
            }

            // send QR to the coordinator, if there is no missing event in it
            if (complete && coord.compare(fullName) != 0) {
                CycleEvent* ce = new CycleEvent(msg::CYCLE_EVENT);
                ce->setSenderName(fullName);
                ce->setRound(round);
                string cycleEvents = util::convertMapToStr(QR);
                ce->setCycleEvents(cycleEvents.c_str());
                transmit(ce, coord, coordGate);
            }

            // move events from QR to Qp
            Qp[round].clear();
            Qp[round].insert(QR.begin(), QR.end());

            // prepare for the next cycle
            scheduleAt(simTime() + roundCycle, ECStart);

//            if(!Qp.empty()){
//                for(map<int, map<unsigned long, string> >::iterator it = Qp.begin();
//                        it != Qp.end(); it++) {
//                    EV << "Qp 3 is not empty" << endl;
//                    if(!it->second.empty()){
//                        EV << "Qp[" << it->first <<"] is not empty" << endl;
//                        for(map<unsigned long, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
//                            EV << "Qp[" << it->first <<"]" << "[" << it2->first <<"] is: " << it2->second << endl;
//                        }
//                    }
//                }
//            }

            // handle event
            handleEvent();

            // for observation
            decodeQr();
            decodeQp();
        }
    }
}

void FastSMRNode::deliverCycleEvent(CycleEvent* ce) {
    int round = ce->getRound();
    map<unsigned long, string> cycleEvents;
    string cycleEventStr = ce->getCycleEvents();
    util::convertStrToMap(cycleEventStr, cycleEvents);

    if (Qd.find(round) == Qd.end()) {
        Qp.erase(round);
        Qp[round].insert(cycleEvents.begin(), cycleEvents.end());

        // for observation
        decodeQp();
    }

    delete ce;
}

void FastSMRNode::handleEvent() {

    // retrieve and handle the events of missing rounds first
    int firtUndeliveredRound = 0;
    if (!Qp.empty()) {
        firtUndeliveredRound = Qp.rbegin()->first;
    } else if (!Qd.empty()) {
        firtUndeliveredRound = Qd.rbegin()->first;
    }

//    EV << "current first undelivered round: " << firtUndeliveredRound << endl;

    for (int i = firtUndeliveredRound + 1; i < round; i++) {
        // send QUERY to coordinator to trigger a consensus instance
        RoundQuery* query = new RoundQuery(msg::ROUND_QUERY);
        query->setSenderName(fullName);
        query->setDestName(coord.c_str());
        query->setRound(i);
        if (coord.compare(fullName) != 0) {
            transmit(query, coord, coordGate);
        } else {
            delete query;
            // fake the send-to-self process
            toPropose.insert(i);
        }
    }

    if (!Qp.empty() && firtUndeliveredRound == round) {

//        EV << "Is Qp empty? " << Qp.empty() << endl;
//        for(map<int, map<unsigned long, string> >::iterator it = Qp.begin(); it != Qp.end(); it++){
//            EV << "Is " << it->first << " empty? " << it->second.empty() << endl;
//        }

        map<int, map<unsigned long, string> > E;
        int roundToQuery = 0;
        bool completeRound = true;
        for (map<int, map<unsigned long, string> >::iterator it = Qp.begin();
                it != Qp.end(); it++) {
            roundToQuery = it->first;
            for (map<unsigned long, string>::iterator it2 = it->second.begin();
                    it2 != it->second.end(); it2++) {
                if (it2->second.empty()) {
                    completeRound = false;
                    break;
                }
            }
            if (completeRound) {
                E[it->first].insert(it->second.begin(), it->second.end());
            } else {
                break;
            }
        }

        if (!completeRound) {
            // send QUERY to coordinator to trigger a consensus instance
            RoundQuery* query = new RoundQuery(msg::ROUND_QUERY);
            query->setSenderName(fullName);
            query->setDestName(coord.c_str());
            query->setRound(roundToQuery);
            if (coord.compare(fullName) != 0) {
                transmit(query, coord, coordGate);
            } else {
                delete query;
                // fake the send-to-self process
                toPropose.insert(roundToQuery);
            }
        }

//        if(!Qp.empty()){
//            for(map<int, map<unsigned long, string> >::iterator it = Qp.begin();
//                    it != Qp.end(); it++) {
//                EV << "Qp is not empty" << endl;
//                if(!it->second.empty()){
//                    EV << "Qp[" << it->first <<"] is not empty" << endl;
//                    for(map<unsigned long, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
//                        EV << "Qp[" << it->first <<"]" << "[" << it2->first <<"] is: " << it2->second << endl;
//                    }
//                }
//            }
//        }

//        if(!E.empty()){
//            for(map<int, map<unsigned long, string> >::iterator it = E.begin();
//                    it != E.end(); it++) {
//                EV << "E is not empty" << endl;
//                if(!it->second.empty()){
//                    EV << "E[" << it->first <<"] is not empty" << endl;
//                    for(map<unsigned long, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
//                        EV << "E[" << it->first <<"]" << "[" << it2->first <<"] is: " << it2->second << endl;
//                    }
//                }
//            }
//        }

        for (map<int, map<unsigned long, string> >::iterator it = E.begin();
                it != E.end(); it++) {
            Qp.erase(it->first);
        }
        Qd.insert(E.begin(), E.end());

    }

    // for observation
    decodeQp();
    decodeQd();
}

void FastSMRNode::handleRoundQuery(RoundQuery* query) {
    int round = query->getRound();
    string sender = query->getSenderName();
    RoundQueryReply* rqr = NULL;
    if (Qd.find(round) != Qd.end()) {
        rqr = new RoundQueryReply(msg::ROUND_QUERY_REPLY);
        string events = util::convertMapToStr(Qd[round]);
        rqr->setEvents(events.c_str());
        rqr->setSenderName(fullName);
        rqr->setDestName(sender.c_str());
        rqr->setRound(round);
    } else if (Qp.find(round) != Qp.end()) {
        // check whether there is any missing event in Qp of the round
        bool complete;
        for (map<unsigned long, string>::iterator it = Qp[round].begin();
                it != Qp[round].end(); it++) {
            if (it->second.empty()) {
                complete = false;
                break;
            }
        }
        if (complete) {
            rqr = new RoundQueryReply(msg::ROUND_QUERY_REPLY);
            string events = util::convertMapToStr(Qp[round]);
            rqr->setEvents(events.c_str());
            rqr->setSenderName(fullName);
            rqr->setRound(round);
        }
    }

    if (rqr != NULL) {
        transmit(rqr, sender, neighbors[sender]);
    } else {
        toPropose.insert(round);
    }

    initConsensus();

    delete query;
}

void FastSMRNode::handleRoundQueryReply(RoundQueryReply* rqr) {
    int round = rqr->getRound();
    map<unsigned long, string> events;
    string eventStr = rqr->getEvents();
    util::convertStrToMap(eventStr, events);
    Qp.erase(round);
    Qp[round].insert(events.begin(), events.end());
    // for observation
    decodeQp();

//    if(!Qp.empty()){
//        for(map<int, map<unsigned long, string> >::iterator it = Qp.begin();
//                it != Qp.end(); it++) {
//            EV << "Qp 5 is not empty" << endl;
//            if(!it->second.empty()){
//                EV << "Qp[" << it->first <<"] is not empty" << endl;
//                for(map<unsigned long, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
//                    EV << "Qp[" << it->first <<"]" << "[" << it2->first <<"] is: " << it2->second << endl;
//                }
//            }
//        }
//    }

    handleEvent();

    delete rqr;
}

void FastSMRNode::initConsensus() {
    if (stage == EH && coord.compare(fullName) == 0) {
        for (set<int>::iterator it = toPropose.begin(); it != toPropose.end();
                it++) {
            int r = *it;
            if (proposed.find(r) == proposed.end()) {
                proposed.insert(r);
                ConsensusQuery* cq = new ConsensusQuery(msg::CONSENSUS_QUERY);
                cq->setSenderName(fullName);
                cq->setEpoch(epoch);
                cq->setGroupId(groupId);
                cq->setRound(r);

                broadcast(cq, neighbors);

                // fake the send-to-self process
                string received = roundEToString(r);
                map<unsigned long, string> events;
                if (!received.empty()) {
                    util::convertStrToMap(received, events);
                }
                proposals[r][fullName] = events;

                checkProposals(r);

                delete cq;
            }
        }
    }
}

void FastSMRNode::startPropose(ConsensusQuery * cq) {
    int r = cq->getRound();
    if (stage == EH) {
        string received = roundEToString(r);

        EV << "Create the proposal: " << received << endl;

        FSMRProposal* proposal = new FSMRProposal(msg::PROPOSAL);
        proposal->setSenderName(fullName);
        proposal->setEpoch(epoch);
        proposal->setGroupId(groupId);
        proposal->setRound(r);
        proposal->setReceived(received.c_str());
        if (coord.compare(fullName) != 0) {
            transmit(proposal, coord, coordGate);
        } else {
            delete proposal;
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

    delete cq;
}

string FastSMRNode::roundEToString(int r) {
    string roundEvents;
    if (Qd.find(r) != Qd.end()) {
        roundEvents = util::convertMapToStr(Qd[r]);
    } else if (Qp.find(r) != Qp.end()) {
        roundEvents = util::convertMapToStr(Qp[r]);
    } else {
        // empty round, indicating that round events have not been delivered
        roundEvents.clear();
    }

    return roundEvents;
}

void FastSMRNode::collectProposal(FSMRProposal* proposal) {
    string senderName = proposal->getSenderName();

    EV << "[" << simTime() << "s] " << fullName << " received PROPOSAL from "
              << senderName << endl;

    int e = proposal->getEpoch();
    int gid = proposal->getGroupId();
    string received = proposal->getReceived();
    int r = proposal->getRound();
    // message validation, TODO the first two conditions may not needed
//    if (neighbors.find(senderName) != neighbors.end()
//            && survivals.find(senderName) != survivals.end() && e == epoch
//            && gid == groupId && stage == EH) {
    if (e == epoch && gid == groupId && stage == EH) {
        map<unsigned long, string> events;
        if (!received.empty()) {
            util::convertStrToMap(received, events);
        }
        proposals[r][senderName] = events;
    }

    checkProposals(r);

    delete proposal;
}

void FastSMRNode::handleDecision(FSMRDecision* decision) {
    // validate message
    string sender = decision->getSenderName();
    int r = decision->getRound();
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
        if (!dec.empty()) {
            map<unsigned long, string> roundEvents;
            util::convertStrToMap(dec, roundEvents);
            Qp[r] = roundEvents;
            decodeQp();
            // invoke handle event
            handleEvent();
        }

        toPropose.erase(r);
        proposed.erase(r);
//        decisions[r] = roundEvents;

//        FSMRAgree* agreement = new FSMRAgree(msg::AGREEMENT);
//        agreement->setSenderName(fullName);
//        agreement->setEpoch(epoch);
//        agreement->setGroupId(groupId);
//        agreement->setRound(r);
//
//        transmit(agreement, coord, coordGate);

        consensus++;
    }

    delete decision;
}

//void FastSMRNode::collectAgreement(FSMRAgree* agreement) {
//    string senderName = agreement->getSenderName();
//    int r = agreement->getRound();
//
//    EV << "[" << simTime() << "s] " << fullName << " received AGREEMENT from "
//              << senderName << endl;
//
//    int e = agreement->getEpoch();
//    int gid = agreement->getGroupId();
//    // message validation, TODO the first two conditions may not needed
//    if (e == epoch && gid == groupId && stage == EH) {
//        agreements[r].push_back(senderName);
//
//        checkAgreements(r);
//    }
//
//    delete agreement;
//}
//
//void FastSMRNode::handleFinal(FSMRFinal* final) {
//
//    // validate message
//    string sender = final->getSenderName();
//    int r = final->getRound();
//
//    if (ev.isGUI()) {
//        string msg = "Received FINAL from Coord";
//        bubble(msg.c_str());
//    }
//    EV << "[" << simTime() << "s] " << fullName << " received decision from "
//              << sender << endl;
//
//    int e = final->getEpoch();
//    int gid = final->getGroupId();
//    if (e == epoch && gid == groupId && stage == EH) {
//        // move events from decisions to Qp
//        Qp[r].insert(decisions[r].begin(), decisions[r].end());
//
//        // TODO broadcast the committed round clients
////        Update* update = new Update(msg::UPDATE);
////        string content = util::convertMapToStr(decisions);
////        update->setContent(content.c_str());
//        // sourceName and destName must be set for module purge
////        update->setSourceName(fullName);
//        // broadcast update to clients
////        for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
////                iter++) {
////            cModule* module = iter();
////            if (strcmp(module->getModuleType()->getName(), "BasicSMRClient")
////                    == 0) {
////                string hostName = module->getFullName();
////                int gateId = createGates(module);
////                update->setDestName(hostName.c_str());
////                sendMsg(update->dup(), hostName, gateId);
////            }
////        }
////        delete update;
//
//        decisions.erase(r);
//
//        // empty agreements
//        agreements.erase(r);
//    }
//
//    delete final;
//
//}

void FastSMRNode::applyEvent() {
    if (appliedRound == 0 && Qd.find(appliedRound) == Qd.end()) {
        appliedRound = Qd.begin()->first;
    }

    map<int, string> temp;
    int firstIndex = 0;
    // check vector size to avoid error
    if (!Qa.empty()) {
        firstIndex = Qa.rbegin()->first;
    }
    if (Qd.find(appliedRound) != Qd.end()) {
//    if(!Qd.empty()){
//        int r = Qd.begin()->first;
        int r = appliedRound;
//        map<unsigned long, string> roundEvents = Qd.begin()->second;
        map<unsigned long, string> roundEvents = Qd[r];
        for (map<unsigned long, string>::iterator it = roundEvents.begin();
                it != roundEvents.end(); it++) {
            firstIndex++;
            string event = it->second;
            // only send non-empty updates
            if (event.compare("empty") != 0) {
                temp[firstIndex] = event;
            }
        }

        Qa.insert(temp.begin(), temp.end());
        if (!temp.empty()) {
            // broadcast the committed round clients
            Update* update = new Update(msg::UPDATE);
            string content = util::convertMapToStr(temp);

            EV << "Applied round: " << r;
            EV << "Applied events: " << content << endl;

            update->setContent(content.c_str());
            // sourceName and destName must be set for module purge
            update->setSourceName(fullName);
            // broadcast update to clients
            for (cModule::SubmoduleIterator iter(getParentModule());
                    !iter.end(); iter++) {
                cModule* module = iter();
                if (strcmp(module->getModuleType()->getName(), "FastSMRClient")
                        == 0) {
                    string hostName = module->getFullName();
                    int gateId = createGates(module);
                    update->setDestName(hostName.c_str());
                    sendMsg(update->dup(), hostName, gateId);
                }
            }
            delete update;
        }

        // move delivered events from Qd to backup Qd
//        Qd_bk[r].insert(roundEvents.begin(), roundEvents.end());
//        Qd.erase(Qd.begin());
//        decodeQd();
//        decodeQdbk();

//        escId = Qd.begin()->first;
//        string content = Qd.begin()->second;
//        Qd.erase(escId);
//        if (esc == 0) {
//            esc = sdbm::encode(content.c_str());
//        } else {
//            string text = util::longToString(esc + sdbm::encode(content.c_str()));
//            esc = sdbm::encode(text.c_str());
//        }

        appliedRound++;
    }

    scheduleAt(simTime() + applyCycle, applyTimeout);
}

void FastSMRNode::stageTransfer() {
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
                        // run the following function for each round r
                        for (set<int>::iterator it = proposed.begin();
                                it != proposed.end(); it++) {
                            initConsensus();
                            checkProposals(*it);
//                            checkAgreements(*it);
                        }
                    }
                }
            }
        }
    }
}

bool FastSMRNode::checkCoord() {
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
            proposed.clear();
//            agreements.clear();

            // send coord_query to other members
            CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
            coordChange->setCandidateName(fullName);
            cMessage* msg = broadcast(coordChange, neighbors);

            // fake the send-to-self process
            coordChange = dynamic_cast<CCQuery*>(msg);
            FSMRLastState ls;
            // load member list
            vector<string> members = util::loadMapKey(neighbors, 1, fullName);
            ls.groupMembers = members;
            ls.epoch = epoch;
            ls.groupId = groupId;
            ls.escId = escId;
            ls.esc = esc;
            // no need for state sync among existing members
//            ls.DL.insert(Qd.begin(), Qd.end());
//            ls.DL.insert(decisions.begin(), decisions.end());
            lastStates[fullName] = ls;
            checkLastStates();

            delete coordChange;
        }
        return true;
    } else {
        return false;
    }
}

bool FastSMRNode::checkConfig() {
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
            proposed.clear();
//            agreements.clear();

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
//            ready.DL.insert(Qd.begin(), Qd.end());
//            ready.DL.insert(decisions.begin(), decisions.end());
            readies[fullName] = ready;
            checkReadies();

            delete reconfig;
        }
        return true;
    } else {
        return false;
    }
}

void FastSMRNode::handleCCQuery(CCQuery* cc) {
    string candidateName = cc->getCandidateName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change from " << candidateName << endl;

    if (survivals.find(candidateName) != survivals.end()) {

        string candidate = this->coordinator();
        if (candidateName.compare(candidate) == 0) {

            stage = CC;
            coord.clear();

            FSMRCCQueryReply* ccr = new FSMRCCQueryReply(
                    msg::COORD_CHANGE_REPLY);
            ccr->setSenderName(fullName);
            ccr->setEpoch(epoch);
            ccr->setGroupId(groupId);
            // send events for state sync
//            ccr->setQd(buildQdForSync().c_str());
//            ccr->setQp(buildQpForSync().c_str());
            // TODO
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

void FastSMRNode::handleGRQuery(GRQuery* gr) {

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
//        grr->setState(buildStateForSync().c_str());
        transmit(grr, senderName, survivals[senderName]);
    }

    delete gr;
}

void FastSMRNode::handleCCNAck(CCNAck* ccnack) {
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

void FastSMRNode::handleCCQueryReply(FSMRCCQueryReply* ccr) {
    string sender = ccr->getSenderName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change reply from " << sender << endl;

    // only handle messages from survived members, TODO but this condition may not needed
    if (neighbors.find(sender) != neighbors.end()
            && survivals.find(sender) != survivals.end()) {
        FSMRLastState ls;
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
        // for state sync
//        string QdStr = ccr->getQd();
//        string QpStr = ccr->getQp();
//        util::convertStrToMap(QdStr, ls.Qd);
//        util::convertStrToMap(QpStr, ls.Qp);

        // TODO sync Qs is still needed

        lastStates[sender] = ls;

        checkLastStates();
    }

    delete ccr;
}

void FastSMRNode::handleGRQueryReply(GRQueryReply* grr) {
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
//        string state = grr->getState();
//        util::convertStrToMap(state, ready.DL);

        // TODO sync Qs is still needed

        readies[sender] = ready;
    }

    checkReadies();

    delete grr;
}

void FastSMRNode::handleCCEnd(FSMRCCEnd* cce) {
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

            if (!isMember) {
                // load synchronized Qd and Qp
                Qd.clear();
                string QdStr = cce->getQd();
                util::convertStrToMap(QdStr, Qd);
                decodeQd();
                Qp.clear();
                string QpStr = cce->getQp();
                util::convertStrToMap(QpStr, Qp);
                decodeQp();
                escId = cce->getEscId();
                esc = cce->getEsc();
                string senderStr = cce->getSenders();
                util::convertStrToMap(senderStr, senders);
                // compute the sor for each client;
                double current = simTime().dbl();
                double cycle = roundCycle.dbl();
                double start = cycleStart.dbl();
                round = floor((current - start) / cycle);
                for (map<unsigned long, int>::iterator it = senders.begin();
                        it != senders.end(); it++) {
//                    sors[it->first] = it->second + round;
                    // TODO calculation of sor needs to be added later
                    sors[it->first] = 0 + round;
                }
                isMember = true;
            }

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

void FastSMRNode::handleGREnd(FSMRGREnd* gre) {
    string senderName = gre->getSenderName();
    int e = gre->getEpoch();
    int gid = gre->getGroupId();
//    if (senderName.compare(coord) == 0 && e == epoch && gid == groupId && stage != CC) {
    if (e == epoch && gid == groupId && stage != CC) {
        if (!isMember) {
            // load synchronized DL
            Qd.clear();
            string QdStr = gre->getQd();
            util::convertStrToMap(QdStr, Qd);
            decodeQd();
            Qp.clear();
            string QpStr = gre->getQp();
            util::convertStrToMap(QpStr, Qp);
            decodeQp();
            escId = gre->getEscId();
            esc = gre->getEsc();
            string senderStr = gre->getSenders();
            util::convertStrToMap(senderStr, senders);
            // compute the sor for each client;
            double current = simTime().dbl();
            double cycle = roundCycle.dbl();
            double start = cycleStart.dbl();
            round = floor((current - start) / cycle);
            for (map<unsigned long, int>::iterator it = senders.begin();
                    it != senders.end(); it++) {
//                    sors[it->first] = it->second + round;
                // TODO calculation of sor needs to be added later
                sors[it->first] = 0 + round;
            }
            isMember = true;
        }

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

void FastSMRNode::checkLastStates() {

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

        FSMRLastState ls = computeLastState();
        FSMRCCEnd* cce = new FSMRCCEnd(msg::LOAD_COORD);
        cce->setSenderName(fullName);
        cce->setEpoch(ls.epoch);
        cce->setGroupId(ls.groupId);
        // concatenate Qd, Qp to string content
        string QdStr = util::convertMapToStr(ls.Qd);
        string QpStr = util::convertMapToStr(ls.Qp);
        cce->setQd(QdStr.c_str());
        cce->setQp(QpStr.c_str());
        cce->setSenders(util::convertMapToStr(senders).c_str());
        cce->setEscId(ls.escId);
        cce->setEsc(ls.esc);
        // concatenate neighbor list to string content
        string members = util::convertListToStr(ls.groupMembers, 0);
        cce->setGroupMembers(members.c_str());
        cMessage* msg = broadcast(cce, neighbors);

        // fake the send-to-self process,  switch back to the event handling stage
        cce = check_and_cast<FSMRCCEnd*>(msg);
        handleCCEnd(cce);
    }
}

void FastSMRNode::checkReadies() {
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

            FSMRReady ready = computeReady();
            FSMRGREnd* gre = new FSMRGREnd(msg::LOAD_CONFIG);
            gre->setSenderName(fullName);
            gre->setEpoch(epoch);
            gre->setGroupId(groupId);
            // concatenate Qd, Qp to string content
            string QdStr = util::convertMapToStr(ready.Qd);
            string QpStr = util::convertMapToStr(ready.Qp);
            gre->setQd(QdStr.c_str());
            gre->setQp(QpStr.c_str());
            gre->setSenders(util::convertMapToStr(senders).c_str());
            gre->setEscId(ready.escId);
            gre->setEsc(ready.esc);
            // concatenate neighbor list to string content
            string members = util::convertListToStr(ready.groupMembers, 0);
            gre->setGroupMembers(members.c_str());
            cMessage* msg = broadcast(gre, neighborsTemp);

            // fake the send-to-self process,  switch back to the event handling stage
            gre = check_and_cast<FSMRGREnd*>(msg);
            handleGREnd(gre);
        }
    }

}

void FastSMRNode::checkProposals(int r) {
    if (stage == EH) {
        bool allReceived = true;
        map<string, int> survivedNeighbors = util::intersectMaps(survivals,
                neighbors);
        survivedNeighbors[fullName] = -1;
        // check whether all replies from  hosts on the Survivals list have been received
        for (map<string, int>::iterator it = survivedNeighbors.begin();
                it != survivedNeighbors.end(); ++it) {
            string hostName = it->first;
            if (proposals[r].find(hostName) == proposals[r].end()) {
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

            map<unsigned long, string> dec = decide(r);

            // create DECISION message
            FSMRDecision* decisionMsg = new FSMRDecision(msg::DECISION);
            decisionMsg->setSenderName(fullName);
            decisionMsg->setEpoch(epoch);
            decisionMsg->setGroupId(groupId);
            decisionMsg->setRound(r);

            if (dec.empty()) {
                decisionMsg->setDecision("");
            } else {
                decisionMsg->setDecision(util::convertMapToStr(dec).c_str());
            }

            // broadcast DECISION message
            cMessage* msg = broadcast(decisionMsg, neighbors);

            // empty proposals
            proposals.erase(r);

            // fake the send-to-self process
            decisionMsg = check_and_cast<FSMRDecision*>(msg);
//            string decision = decisionMsg->getDecision();
//            util::convertStrToMap(decision, decisions);
//            agreements.push_back(fullName);
//            delete decisionMsg;
            handleDecision(decisionMsg);
        }
    }
}

map<unsigned long, string> FastSMRNode::decide(int r) {
    map<unsigned long, string> decision;
    bool unreceived = false;
    vector<unsigned long> keys;
    // retrieve the keys - i.e., clients
    for (map<string, map<unsigned long, string> >::iterator it =
            proposals[r].begin(); it != proposals[r].end(); it++) {
        if (it->second.empty()) {
            unreceived = true;
        } else {
            for (map<unsigned long, string>::iterator it2 = it->second.begin();
                    it2 != it->second.end(); it2++) {
                keys.push_back(it2->first);
            }
        }
    }

    // for each key, look for the non-missing events
    for (size_t i = 0; i < keys.size(); i++) {
        unsigned long key = keys[i];
        for (map<string, map<unsigned long, string> >::iterator it =
                proposals[r].begin(); it != proposals[r].end(); it++) {
            if (!it->second.empty() && !it->second[key].empty()) {
                decision[key] = it->second[key];
                break;
            }
        }
    }

    // if there is an missing event in the round and at least one host hasn't receiev the round, leave the round for future decision
    for (size_t i = 0; i < keys.size(); i++) {
        if (decision[keys[i]].empty()) {
            if (unreceived) {
                decision.clear();
                break;
            } else {
                decision[keys[i]] = "empty";
            }
        }
    }

    return decision;
}

//void FastSMRNode::checkAgreements(int r) {
//    if (stage == EH) {
//        bool allReceived = true;
//        map<string, int> survivedNeighbors = util::intersectMaps(survivals,
//                neighbors);
//        survivedNeighbors[fullName] = -1;
//        // check whether all replies from  hosts on the Survivals list have been received
//        for (map<string, int>::iterator it = survivedNeighbors.begin();
//                it != survivedNeighbors.end(); ++it) {
//            string hostName = it->first;
//            if (find(agreements[r].begin(), agreements[r].end(), hostName)
//                    == agreements[r].end()) {
//                allReceived = false;
//                break;
//            }
//        }
//
//        if (allReceived) {
//
//            if (ev.isGUI()) {
//                string msg = "Received all Agreements";
//                bubble(msg.c_str());
//            }
//
//            FSMRFinal* final = new FSMRFinal(msg::FINAL);
//            final->setSenderName(fullName);
//            final->setEpoch(epoch);
//            final->setGroupId(groupId);
//            final->setRound(r);
//
//            // broadcast FINAL message
//            cMessage* msg = broadcast(final, neighbors);
//
//            // fake the send-to-self process
//            final = check_and_cast<FSMRFinal*>(msg);
//            handleFinal(final);
//        }
//    }
//}

//string FastSMRNode::buildQdForSync() {
//    string QdStr = util::convertMapToStr(Qd);
//    return QdStr;
//}
//
//string FastSMRNode::buildQpForSync() {
//    // merge decisions to Qp
//    for (map<int, map<unsigned long, string> >::iterator it = decisions.begin();
//            it != decisions.end(); it++) {
//        int r = it->first;
//        Qp[r].insert(it->second.begin(), it->second.end());
//    }
//    string QpStr = util::convertMapToStr(Qp);
//    return QpStr;
//}

string FastSMRNode::coordinator() {
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

FSMRLastState FastSMRNode::computeLastState() {
    // compute the sync state: use the Qd of Coordinator and merge Qp from all nodes in the group
    FSMRLastState ls;
    ls.Qd = Qd;
    ls.Qp = Qp;

    // initial value for comparison purpose
    ls.epoch = epoch;
    ls.groupId = groupId;
    for (map<string, FSMRLastState>::iterator it = lastStates.begin();
            it != lastStates.end(); it++) {
        LastState tmp = it->second;
        if (tmp.epoch > ls.epoch) {
            ls.epoch = tmp.epoch;
        }
        if (tmp.groupId >= ls.groupId) {
            ls.groupId = tmp.groupId;
            ls.groupMembers = tmp.groupMembers;
        }
    }
    return ls;
}

FSMRReady FastSMRNode::computeReady() {
    FSMRReady ready;
    ready.Qd = Qd;
    ready.Qp = Qp;

    // retrieve new group members from G'
    for (map<string, int>::iterator it = neighborsTemp.begin();
            it != neighborsTemp.end(); it++) {
        string pair = it->first + ":" + util::intToStr(seniorities[it->first]);
        ready.groupMembers.push_back(pair);
    }
    string selfPair = string(fullName) + ":"
            + util::intToStr(seniorities[fullName]);
    ready.groupMembers.push_back(selfPair);

    return ready;
}

cMessage* FastSMRNode::broadcast(cMessage* message, map<string, int>& hosts) {
    GatebasedModule::multicast(message, hosts);
    return message;
}

void FastSMRNode::decodeQr() {
//    Qr_w.clear();
//    for (map<unsigned long, map<int, string> >::iterator it = Qr.begin();
//            it != Qr.end(); it++) {
//        unsigned long senderId = it->first;
//        string event = util::convertMapToStr(it->second);
//        Qr_w[senderId] = event;
//    }
}
void FastSMRNode::decodeQp() {
//    Qp_w.clear();
//    for (map<int, map<unsigned long, string> >::iterator it = Qp.begin();
//            it != Qp.end(); it++) {
//        int r = it->first;
//        string event = util::convertMapToStr(it->second);
//        Qp_w[r] = event;
//    }
}
void FastSMRNode::decodeQd() {
//    Qd_w.clear();
//    for (map<int, map<unsigned long, string> >::iterator it = Qd.begin();
//            it != Qd.end(); it++) {
//        int r = it->first;
//        string event = util::convertMapToStr(it->second);
//        Qd_w[r] = event;
//    }
}

void FastSMRNode::decodeQdbk() {
    Qd_bk_w.clear();
    for (map<int, map<unsigned long, string> >::iterator it = Qd_bk.begin();
            it != Qd_bk.end(); it++) {
        int r = it->first;
        string event = util::convertMapToStr(it->second);
        Qd_bk_w[r] = event;
    }
}

void FastSMRNode::fail() {
    state = Off;
    par("state") = state;
}
