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

#include "ConsensusBasedCtrl.h"

Define_Module(ConsensusBasedCtrl);

ConsensusBasedCtrl::ConsensusBasedCtrl() {
    lastDelivered = -1;
    STAGE_NUM = stage::NODE_CTRL_INIT;
    cycleStart = -1;
    idle = true;

    ECTimeout = new cMessage(msg::EC_START);
    GCTimeout = new cMessage(msg::GC_TIMEOUT);
    hbTimeout = new HBTimeout(msg::HEARTBEAT_TIMEOUT);
}

ConsensusBasedCtrl::~ConsensusBasedCtrl() {
    if (ECTimeout != NULL) {
        cancelAndDelete(ECTimeout);
    }
    if (GCTimeout != NULL) {
        cancelAndDelete(GCTimeout);
    }
    if (hbTimeout != NULL) {
        cancelAndDelete(hbTimeout);
    }
}

int ConsensusBasedCtrl::numInitStages() const {
    return STAGE_NUM;
}

void ConsensusBasedCtrl::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();

        roundCycle = par("roundCycle");
        gcCycle = par("GCCycle");

        WATCH(coord);
        WATCH(mode);
        WATCH(epoch);
        WATCH(groupId);
        WATCH(gid_temp);
        WATCH_SET(members);
        WATCH_SET(membersTemp);
        WATCH_SET(survivals);
        WATCH_MAP(seniorities);
        WATCH_MAP(clients);

        WATCH_MAP(Qr);
        WATCH_MAP(Qd);
        WATCH_MAP(histories);
        WATCH(lastDelivered);
        WATCH(state.first);
        WATCH(state.second);
    }

    if (stage == 1) {
        // init host neighbors
        groupId = gid_temp = -1;
        // initial stage to avoid starting from EH
        mode = DORMANT;
        epoch = -1;
        ipAddress = loadIPAddress();
        getParentModule()->getDisplayString().setTagArg("t", 0,
                ipAddress.get4().str().c_str());

        QdLength = registerSignal("QdLength");
    }
}

void ConsensusBasedCtrl::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::HEARTBEAT)) {
        HBProbe* hb = check_and_cast<HBProbe*>(msg);
        handleHeartbeat(hb);
        // stage transfer
        stateTransfer();
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
        bool reconfig = cce->getReconfig();
        handleCCEnd(cce);
        // initialize application state
        ConsensusBasedApp* app =
                dynamic_cast<ConsensusBasedApp*>(this->getParentModule()->getSubmodule(
                        "app"));
        app->initState(state.second);
        // continue event delivery
        applyEvent();
    } else if (msg->isName(msg::CC_ACK)) {
        CCAck* ack = check_and_cast<CCAck*>(msg);
        handleCCAck(ack);
    } else if (msg->isName(msg::GROUP_RECONFIG)) {
        GRQuery* gr = check_and_cast<GRQuery*>(msg);
        handleGRQuery(gr);
    } else if (msg->isName(msg::GR_NACK)) {
        GRNAck* grnack = check_and_cast<GRNAck*>(msg);
        handleGRNAck(grnack);
    } else if (msg->isName(msg::RECONFIG_READY)) {
        GRQueryReply* grr = check_and_cast<GRQueryReply*>(msg);
        handleGRQueryReply(grr);
    } else if (msg->isName(msg::LOAD_CONFIG)) {
        GREnd* gre = check_and_cast<GREnd*>(msg);
        handleGREnd(gre);
        // initialize application state
        ConsensusBasedApp* app =
                dynamic_cast<ConsensusBasedApp*>(this->getParentModule()->getSubmodule(
                        "app"));
        app->initState(state.second);
        // continue event delivery
        applyEvent();
    } else if (msg->isName(msg::EVENT)) {
        // run on primary
        Event* event = check_and_cast<Event*>(msg);
        receiveEvent(event);
    } else if (msg->isName(msg::EC_START)) {
        initConsensus();
    } else if (msg->isName(msg::CONSENSUS_QUERY)) {
        ConsensusQuery * cq = check_and_cast<ConsensusQuery*>(msg);
        this->startPropose(cq);
    } else if (msg->isName(msg::CONSENSUS_NACK)) {
        ConsensusNAck * nack = check_and_cast<ConsensusNAck*>(msg);
        this->handleConsensusNAck(nack);
    } else if (msg->isName(msg::PROPOSAL)) {
        Proposal* proposal = check_and_cast<Proposal*>(msg);
        collectProposal(proposal);
    } else if (msg->isName(msg::DECISION)) {
        Decision* decision = check_and_cast<Decision*>(msg);
        handleDecision(decision);
    } else if (msg->isName(msg::CONSENSUS_ACK)) {
        ConsensusAck* ack = check_and_cast<ConsensusAck*>(msg);
        handleConsensusAck(ack);
    } else if (msg->isName(msg::INIT_JOIN)) {
        InitJoin* initJoin = check_and_cast<InitJoin*>(msg);
        handleJoin(initJoin);
    } else if (msg->isName(msg::GC_TIMEOUT)) {
        gossip();
    } else if (msg->isName(msg::GOSSIP_GC)) {
        Gossip* gossip = check_and_cast<Gossip*>(msg);
        handleGossip(gossip);
    } else if (msg->isName(msg::UPDATE_APPLY)) {
        UpdateApply* updateApply = check_and_cast<UpdateApply*>(msg);
        applyUpdate(updateApply);
    } else if (msg->isName(msg::HEARTBEAT_TIMEOUT)) {
        HBTimeout* hbt = check_and_cast<HBTimeout*>(msg);
        handleHeartbeatTimeout(hbt);
    }
}

void ConsensusBasedCtrl::handleHeartbeat(HBProbe *hb) {
    UDPControlInfo *hbCtrl = check_and_cast<UDPControlInfo *>(
            hb->getControlInfo());
    IPvXAddress& srcAddr = hbCtrl->getSrcAddr();
    rendezvous = srcAddr;
    LCName = hb->getLCName();
    string dest = hb->getSourceName();
    bool isInit = hb->getIsInit();
    string content = hb->getSurvivals();
    vector<string> hostAddrs;
    util::splitString(content, ";", hostAddrs);

    // change survivals list
    vector<string> toRemove;
    vector<string> toAdd;
    for (set<string>::iterator it = survivals.begin(); it != survivals.end();
            it++) {
        string host = *it;
        // apply the vector search algorithm
        if (find(hostAddrs.begin(), hostAddrs.end(), host) == hostAddrs.end()) {
            toRemove.push_back(host);
        }
    }
    for (size_t i = 0; i < hostAddrs.size(); i++) {
        if (survivals.count(hostAddrs[i]) == 0) {
            toAdd.push_back(hostAddrs[i]);
        }
    }
    // remove failed hosts
    for (size_t i = 0; i < toRemove.size(); i++) {
        survivals.erase(toRemove[i]);
    }
    // add new hosts
    for (size_t i = 0; i < toAdd.size(); i++) {
        string hostAddr = toAdd[i];
        survivals.insert(hostAddr);
    }
    hostAddrs.clear();
    toRemove.clear();
    toAdd.clear();

    if (isInit) {
        simtime_t starttime = hb->getStartTime();
        initNode(starttime);
    }

    // send heartbeat response
    HBResponse* hbr = new HBResponse(msg::HEARTBEAT_RESPONSE);
    hbr->setSourceName(fullName);
    hbr->setDestName(dest.c_str());
    // TODO may add clock error later
    hbr->setTimestamp(simTime());
    UDPControlInfo* udpControlInfo = new UDPControlInfo();
    udpControlInfo->setDestAddr(srcAddr);
    udpControlInfo->setSrcAddr(ipAddress);
    hbr->setControlInfo(udpControlInfo);
    sendFixedDelay(hbr);

    // set heartbeat timeout
    cancelEvent(hbTimeout);
    simtime_t cycle = getParentModule()->par("HBcycle");
    scheduleAt(simTime() + cycle, hbTimeout);

    // delete the received msg
    delete hb;
}

void ConsensusBasedCtrl::handleHeartbeatTimeout(HBTimeout* hbt) {
    UnderlayConfiguratorAccess().get()->revokeNode(ipAddress);
}

void ConsensusBasedCtrl::initNode(simtime_t starttime) {
    // group ID starts from 0
    groupId = gid_temp = 0;
    members.insert(survivals.begin(), survivals.end());
    for (set<string>::iterator it = members.begin(); it != members.end();
            it++) {
        seniorities[*it] = 1;
    }

    scheduleAt(simTime() + gcCycle, GCTimeout);
}

void ConsensusBasedCtrl::handleJoin(InitJoin* initJoin) {
    UDPControlInfo* initJoinCtrl = check_and_cast<UDPControlInfo *>(
            initJoin->getControlInfo());
    IPvXAddress srcAddr = initJoinCtrl->getSrcAddr();
    string addrStr = srcAddr.get4().str();

    if (coord.empty()) {
        ConnectReply* reply = new ConnectReply(msg::REPLY_CONNECT);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        reply->setControlInfo(udpControlInfo);
        reply->setLCName(initJoin->getLCName());
        sendFixedDelay(reply);
    } else {
        int seq_s = initJoin->getSeq();
        unsigned long senderId = initJoin->getSenderId();
        string senderName = initJoin->getSourceName();
        simtime_t sendTime = initJoin->getJoinTime();
        if (clients.count(senderName) == 0) { // to achieve idempotent INIT_JOIN message handling
            clients[senderName] = addrStr;
            // address of update recipients
            map<string, string> addresses;
            addresses[clients[senderName]] = senderName;
            //    tuple_rg rg(addresses, -1);
            Rg[senderId] = addresses;

            Join* join = new Join(msg::JOIN);
            join->setSenderName(fullName);
            join->setLCName(initJoin->getLCName());
            //    join->setJoinTime(cycleStart - roundCycle);
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            join->setControlInfo(udpControlInfo);
            sendFixedDelay(join);
        }
    }
    delete initJoin;
}

void ConsensusBasedCtrl::receiveEvent(Event* event) {
    unsigned long senderId = event->getSenderId();
    int seq = event->getSeq();
    if (senderSeqs.count(senderId) == 0
            || (senderSeqs.count(senderId) > 0 && senderSeqs[senderId] < seq)) {
        string content = event->getContent();
        tuple_qr t(senderId, seq);
        Qr[t] = boost::tuple<string, simtime_t>(content, simTime());
    }
    delete event;
}

void ConsensusBasedCtrl::initConsensus() {
    if (mode == EH && coord.compare(ipAddress.get4().str()) == 0 && idle) {
        idle = false;
        ConsensusQuery* cq = new ConsensusQuery(msg::CONSENSUS_QUERY);
        cq->setSenderName(fullName);
        cq->setEpoch(epoch);
        cq->setGroupId(groupId);
        // message size: 1 byte for epoch + 1 byte for group ID
        cq->setByteLength(1 + 1);
        r_multicast(cq, survivedMembers());
        GlobalStatisticsAccess().get()->increaseConsensusStart();
    }

    // prepare for the next cycle
    scheduleAt(simTime() + roundCycle, ECTimeout);
}

void ConsensusBasedCtrl::startPropose(ConsensusQuery* cq) {
    UDPControlInfo *cqCtrl = check_and_cast<UDPControlInfo *>(
            cq->getControlInfo());
    IPvXAddress& srcAddr = cqCtrl->getSrcAddr();
    string senderSeqRanges = cq->getSeqRanges();
    int e = cq->getEpoch();
    int gid = cq->getGroupId();
    // since this query changes "proposed", the checks on epoch and configuration ID is needed
    if (e == epoch && gid == groupId && mode == EH) {

        string received = util::convertMapToStr(Qr);

        if (hasGUI()) {
//            cout << "[" << simTime() << "][" << fullName
//                    << "] Create the proposal: " << received << endl;
        }

        Proposal* proposal = new Proposal(msg::PROPOSAL);
        proposal->setSenderName(fullName);
        proposal->setEpoch(epoch);
        proposal->setGroupId(groupId);
        proposal->setReceived(received.c_str());
        // message size: received event size + 1 byte for epoch + 1 byte for group ID
        proposal->setByteLength(received.size() + 2);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        proposal->setControlInfo(udpControlInfo);
        r_transmit(proposal, srcAddr);
    } else {
        ConsensusNAck* nack = new ConsensusNAck(msg::CONSENSUS_NACK);
        nack->setSenderName(fullName);
        nack->setEpoch(epoch);
        nack->setGroupId(groupId);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        nack->setControlInfo(udpControlInfo);
        r_transmit(nack, srcAddr);
    }

    delete cq;
}

void ConsensusBasedCtrl::collectProposal(Proposal* proposal) {
    UDPControlInfo *proposalCtrl = check_and_cast<UDPControlInfo *>(
            proposal->getControlInfo());
    IPvXAddress& srcAddr = proposalCtrl->getSrcAddr();
    string senderAddr = srcAddr.get4().str();
    string senderName = proposal->getSenderName();

    EV << "[" << simTime() << "s] " << fullName << " received PROPOSAL from "
              << senderName << " [" << senderAddr << "]" << endl;

    int e = proposal->getEpoch();
    int gid = proposal->getGroupId();
    string received = proposal->getReceived();

    if (e == epoch && gid == groupId && mode == EH) {
        map<tuple_qr, boost::tuple<string, simtime_t> > events;
        util::convertStrToMap(received, events);
        proposals[senderAddr] = events;
    }
    checkProposals();
    delete proposal;
}

void ConsensusBasedCtrl::handleConsensusNAck(ConsensusNAck* nack) {
    // reset consensus
    proposals.clear();
    idle = true;
    delete nack;
}

void ConsensusBasedCtrl::checkProposals() {
    if (mode == EH) {
        bool allReceived = true;
        set<string> memberList = survivedMembers();
        // check whether all replies from  hosts on the Survivals list have been received
        for (set<string>::iterator it = memberList.begin();
                it != memberList.end(); it++) {
            string host = *it;
            if (proposals.count(host) == 0) {
                allReceived = false;
                break;
            }
        }

        if (allReceived) {
            EV << "[" << simTime() << "s] " << fullName
                      << " start processing proposals" << endl;

            map<int, string> dec = decide();

            // create DECISION message
            Decision* decisionMsg = new Decision(msg::DECISION);
            decisionMsg->setSenderName(fullName);
            decisionMsg->setEpoch(epoch);
            decisionMsg->setGroupId(groupId);

            string decision = "";
            if (!dec.empty()) {
                decision = util::convertMapToStr(dec);
            }
            decisionMsg->setDecision(decision.c_str());
            // message size: decided event size + 1 byte for epoch + 1 byte for group ID
            decisionMsg->setByteLength(decision.size() + 2);

            // broadcast DECISION message
            r_multicast(decisionMsg, memberList);

            // empty proposals
            proposals.clear();
        }
    }
}

map<int, string> ConsensusBasedCtrl::decide() {
    // unique events, in ((sender_ID, event_sequence, event_content), arrival_time)
    map<boost::tuple<unsigned long, int, string>, simtime_t> E;
    for (map<string, map<tuple_qr, boost::tuple<string, simtime_t> > >::iterator it =
            proposals.begin(); it != proposals.end(); it++) {
        for (map<tuple_qr, boost::tuple<string, simtime_t> >::iterator it2 =
                it->second.begin(); it2 != it->second.end(); it2++) {
            unsigned long senderId = get<0>(it2->first);
            int eventSequence = get<1>(it2->first);
            if (senderSeqs.count(senderId) == 0
                    || (senderSeqs.count(senderId) > 0
                            && senderSeqs[senderId] < eventSequence)) {
                string event = get<0>(it2->second);
                simtime_t arrival = get<1>(it2->second);
                boost::tuple<unsigned long, int, string> key(senderId,
                        eventSequence, event);
                if (E.find(key) == E.end()) {
                    E[key] = arrival;
                } else if (E[key] > arrival) {
                    // assigned the earliest arrival time for each received event
                    E[key] = arrival;
                }
            }
        }
    }
    // sorted events
    multimap<simtime_t, boost::tuple<unsigned long, int, string> > SE;
    // sort events by arrival time
    for (map<boost::tuple<unsigned long, int, string>, simtime_t>::iterator it =
            E.begin(); it != E.end(); it++) {
        SE.insert( { it->second, it->first });
    }
    // event sequence
    map<int, string> eventSequence;
    // calculate the last delivered event index
    int lastEventIndex = Qd.empty() ? 0 : Qd.rbegin()->first;
    // assigned index to each event
    for (multimap<simtime_t, boost::tuple<unsigned long, int, string> >::iterator it =
            SE.begin(); it != SE.end(); it++) {
        lastEventIndex++;
        stringstream ss;
        ss << get<0>(it->second) << "," << get<1>(it->second) << ","
                << get<2>(it->second);
        eventSequence[lastEventIndex] = ss.str();
    }

    return eventSequence;
}

void ConsensusBasedCtrl::handleDecision(Decision* decision) {
    UDPControlInfo *msgCtrl = check_and_cast<UDPControlInfo *>(
            decision->getControlInfo());
    IPvXAddress& srcAddr = msgCtrl->getSrcAddr();
    // validate message
    string sender = decision->getSenderName();

    EV << "[" << simTime() << "s] " << fullName << " received decision from "
              << sender << endl;

    int e = decision->getEpoch();
    int gid = decision->getGroupId();
    string dec = decision->getDecision();
    if (e == epoch && gid == groupId && mode == EH) {
        if (!dec.empty()) {
            map<int, string> decisions;
            map<int, boost::tuple<unsigned long, int, string> > deliverys;
            util::convertStrToMap(dec, decisions);
            // remove the corresponding events from Qr
            for (map<int, string>::iterator it = decisions.begin();
                    it != decisions.end(); it++) {
                int eventIndex = it->first;
                string eventInString = it->second;
                vector<string> tokens;
                util::splitString(eventInString, ",", tokens);
                unsigned long senderId = util::strToLong(tokens[0]);
                int sequence = util::strToInt(tokens[1]);
                string content = tokens[2];
                tuple_qr qr_key(senderId, sequence);
                deliverys[eventIndex] =
                        boost::tuple<unsigned long, int, string>(senderId,
                                sequence, content);
                Qr.erase(qr_key);
                if (senderSeqs.count(senderId) == 0
                        || (senderSeqs.count(senderId) > 0
                                && senderSeqs[senderId] < sequence)) {
                    senderSeqs[senderId] = sequence;
                }
                // remove received events with earlier event sequence
                clearQr();
            }
            // move events from decisions to Qd
            Qd.insert(deliverys.begin(), deliverys.end());
            applyEvent();
        }

        /*
         * For uniformly agreeing on the end of a consensus. Otherwise, the leader is not able to know
         * whether other replicas have finish the decision handling step without assigning the idle flag
         * on other replicas. If a non-leader replica is assigned the idle replica, the control of their
         * flag in consensus re-start will be complicated and a new communication round must be involved.
         */
        ConsensusAck* ack = new ConsensusAck(msg::CONSENSUS_ACK);
        ack->setSenderName(fullName);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        ack->setControlInfo(udpControlInfo);
        r_transmit(ack, srcAddr);

    }

    delete decision;
}

void ConsensusBasedCtrl::handleConsensusAck(ConsensusAck* ack) {
    UDPControlInfo *ackCtrl = check_and_cast<UDPControlInfo *>(
            ack->getControlInfo());
    IPvXAddress& srcAddr = ackCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = ack->getSenderName();

    EV << "[" << simTime() << "s] " << fullName
              << " received consensus acknowledgement from " << senderName
              << endl;

    // only handle messages from survived members
    set<string> survived_members = survivedMembers();
    if (survived_members.count(sender) > 0) {
        consensusAcks.insert(sender);
        checkConsensusAcks();
    }

    delete ack;
}

void ConsensusBasedCtrl::checkConsensusAcks() {
    bool allReceived = true;

    set<string> survived_members = survivedMembers();
    // check whether all replies from  hosts on the Survivals list have been received
    for (set<string>::iterator it = survived_members.begin();
            it != survived_members.end(); ++it) {
        string hostName = *it;
        if (consensusAcks.find(hostName) == consensusAcks.end()) {
            allReceived = false;
            break;
        }
    }

    if (allReceived) {
        idle = true;
        // for statistics on the total number of consensus in a simulation run
        GlobalStatisticsAccess().get()->increaseConsensusEnd();
    }
}

void ConsensusBasedCtrl::applyEvent() {
    // serialize events in Qd and deliver events to the application layer
    map<int, boost::tuple<unsigned long, int, string> >::iterator it =
            Qd.begin();
    if (lastDelivered != -1) {
        if (Qd.find(lastDelivered) != Qd.end()) {
            it = Qd.find(lastDelivered);
            it++;
        }
    }
    for (; it != Qd.end(); it++) {
        int key = it->first;
        boost::tuple<unsigned long, int, string> event = it->second;
        ConsensusBasedEventApply* apply = new ConsensusBasedEventApply(
                msg::EVENT_APPLY);
        apply->setEventIndex(key);
        apply->setClientId(get<0>(event));
        apply->setSequence(get<1>(event));
        apply->setEvent(get<2>(event).c_str());
        send(apply, this->gateHalf("uplink", cGate::OUTPUT));
        lastDelivered = key;
    }
}

void ConsensusBasedCtrl::applyUpdate(UpdateApply* updateApply) {
    string event = updateApply->getEvent();
    // here, round is used as the event index
    state.first = updateApply->getRound();
    state.second = updateApply->getState();

    // broadcast handled events to recipients
    for (map<unsigned long, map<string, string> >::iterator it = Rg.begin();
            it != Rg.end(); it++) {
        unsigned long recipientId = it->first;
        Update* update = new Update(msg::UPDATE);
        update->setContent(event.c_str());
        update->setLCName(LCName.c_str());
        update->setSenderId(updateApply->getClientId());
        update->setSequence(updateApply->getSequence());
        update->setSourceName(fullName);
        // message size: content size + 1 byte for round number + 1 byte for logical computer identity
        update->setByteLength(event.size() + 2);
        map<string, string> addresses = it->second;
        for (map<string, string>::iterator it3 = addresses.begin();
                it3 != addresses.end(); it3++) {
            Update* updateDup = update->dup();
            IPvXAddress dest(it3->first.c_str());
            string hostName = it3->second;
            updateDup->setDestName(hostName.c_str());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(dest);
            udpControlInfo->setSrcAddr(ipAddress);
            updateDup->setControlInfo(udpControlInfo);
            sendReliable(updateDup);
        }
        delete update;
    }

    delete updateApply;
}

void ConsensusBasedCtrl::gossip() {
    Gossip* gossip = new Gossip(msg::GOSSIP_GC);
    gossip->setSourceName(fullName);
    gossip->setSendTime(readClock());
    gossip->setLastRound(lastDelivered);
    // broadcast DECISION message
    r_multicast(gossip, survivedMembers());

    // reset gossip timer
    scheduleAt(readClock() + gcCycle, GCTimeout);

    emit(QdLength, (double) Qd.size());
}

void ConsensusBasedCtrl::handleGossip(Gossip* gossip) {
    if (groupId >= 0) {
        UDPControlInfo *gossipCtrl = check_and_cast<UDPControlInfo *>(
                gossip->getControlInfo());
        IPvXAddress& srcAddr = gossipCtrl->getSrcAddr();
        string sender = srcAddr.get4().str();
        simtime_t ts = gossip->getSendTime();
        int lastRound = gossip->getLastRound();
        if (histories.count(sender) == 0 || ts > get<1>(histories[sender])) {
            histories[sender] = tuple_h(lastRound, ts);
        }

        set<string> survived_members = survivedMembers();

        // remove failed replicas in the histories buffer
        set<string> obsolete;
        for (auto elem : histories) {
            if (survived_members.count(elem.first) == 0) {
                obsolete.insert(elem.first);
            }
        }
        for (auto elem : obsolete) {
            histories.erase(elem);
        }

        // If histories contains the messages from all the survivedMembers, then garbage collection can proceed.
        if (includes(histories | map_keys, survived_members)) {
            vector<int> lastRounds;
            for (map<string, tuple_h>::iterator it = histories.begin();
                    it != histories.end(); it++) {
                lastRounds.push_back(get<0>(it->second));
            }
            int lastCommRound = *min_element(lastRounds.begin(),
                    lastRounds.end());
            vector<int> purge_qd;
            int lastDeliveredRound = lastDelivered;
            int lastRemove =
                    lastCommRound <= lastDeliveredRound ?
                            lastCommRound : lastDeliveredRound;
            for (map<int, boost::tuple<unsigned long, int, string> >::iterator it =
                    Qd.begin(); it != Qd.end(); it++) {
                int key = it->first;
                if (key <= lastRemove) {
                    purge_qd.push_back(key);
                }
            }
            for (size_t i = 0; i < purge_qd.size(); i++) {
                Qd.erase(purge_qd[i]);
            }
        }
    }
    delete gossip;
}

void ConsensusBasedCtrl::clearQr() {
    set<tuple_qr> QrKeys;
    for (auto elem : Qr) {
        unsigned long senderId_Qr = get<0>(elem.first);
        int sequence_Qr = get<1>(elem.first);
        if (senderSeqs.count(senderId_Qr) > 0
                && senderSeqs[senderId_Qr] > sequence_Qr) {
            QrKeys.insert(tuple_qr(senderId_Qr, sequence_Qr));
        }
    }
    for (auto elem : QrKeys) {
        Qr.erase(elem);
    }
}

void ConsensusBasedCtrl::stateTransfer() {
    if (groupId >= 0) {
        // check whether coordinator change needs to be triggered
        bool triggerCood = checkCoord();
        string addrStr = ipAddress.get4().str();
        if (!triggerCood) {
            if (mode == CC) {
                if (coordCand.compare(addrStr) == 0) {
                    checkCCSnapshos();
                    checkCCAcks();
                }
            } else {
                // check whether group reconfiguration needs to be triggered
                bool triggerReconfig = checkConfig();
                if (!triggerReconfig) {
                    if (mode == GR) {
                        if (coord.compare(addrStr) == 0) {
                            checkGRSnapshots();
                        }
                    } else {
                        if (coord.compare(addrStr) == 0) {
                            /*
                             * run the following function for each round r, in case that the waiting replica becomes failed
                             */
                            checkProposals();
                            checkConsensusAcks();
                        }
                    }
                }
            }
        }
    }
}

bool ConsensusBasedCtrl::checkCoord() {
    string addrStr = ipAddress.get4().str();
    /*
     * check whether coordinator is survived:
     * 1) not a new node;
     * 2) not a coordinator or coordinator candidate;
     * 3) the current coordinator does not survive.
     */
    if (seniorities[addrStr] > 0 && coordCand.compare(addrStr) != 0
            && ((coord.compare(addrStr) != 0)
                    && survivals.find(coord) == survivals.end())) {
        EV << "[" << simTime() << "s] " << fullName << " found coordinator "
                  << coord << " has failed" << endl;

        // if not, go to coordinator change mode
        mode = CC;
        coord.clear();
        // abort the current consensus instance
        idle = true;

        // compute the coordinator candidate
        string candidate = coordinator();
        string addrStr = ipAddress.get4().str();
        // if the node is a coordinator candidate
        if (candidate.compare(addrStr) == 0) {
            EV << "[" << simTime() << "s] " << fullName
                      << " requested to become the new coordinator" << endl;

            // if the node itself is the coordinator candidate
            coordCand = addrStr;
            CCSnapshots.clear();
            membersTemp.clear();

            // send coord_query to other members
            CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
            coordChange->setCandidate(addrStr.c_str());
            r_multicast(coordChange, survivedMembers());

            GlobalStatisticsAccess().get()->increaseLeaderElectionStart();
        }
        return true;
    } else {
        return false;
    }
}

string ConsensusBasedCtrl::coordinator() {
    string coordAddr = ipAddress.get4().str().c_str();
    unsigned long coordId = sdbm::encode(coordAddr.c_str());
    set<string> survived_members = survivedMembers();
    for (set<string>::iterator it = survived_members.begin();
            it != survived_members.end(); it++) {
        string addr = *it;
        unsigned long id = sdbm::encode(addr.c_str());
        if (seniorities[addr] > seniorities[coordAddr]
                || (seniorities[addr] == seniorities[coordAddr] && id > coordId)) {
            coordId = id;
            coordAddr = *it;
        }
    }
    return coordAddr;
}

void ConsensusBasedCtrl::handleCCQuery(CCQuery* cc) {
    UDPControlInfo *ccCtrl = check_and_cast<UDPControlInfo *>(
            cc->getControlInfo());
    IPvXAddress& srcAddr = ccCtrl->getSrcAddr();
    string candidateAddr = cc->getCandidate();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change from " << candidateAddr << endl;

    string candidate = coordinator();
    if (survivals.count(candidateAddr) > 0
            && candidateAddr.compare(candidate) == 0) {
        mode = CC;
        coord.clear();

        CCQueryReply* ccr = new CCQueryReply(msg::COORD_CHANGE_REPLY);
        ccr->setSenderName(fullName);
        string snapshot = buildCCSnapshot();
        ccr->setState(snapshot.c_str());
        // message size: snapshot size
        ccr->setByteLength(snapshot.size());
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        ccr->setControlInfo(udpControlInfo);
        r_transmit(ccr, srcAddr);
    } else {
        CCNAck* ccr = new CCNAck(msg::CC_NACK);
        ccr->setSenderName(fullName);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        ccr->setControlInfo(udpControlInfo);
        r_transmit(ccr, srcAddr);
    }

    delete cc;
}

string ConsensusBasedCtrl::buildCCSnapshot() {
    json snapshot;
    string Qdstr = util::convertMapToStr(Qd);
    snapshot["Qd"] = Qdstr;
    snapshot["epoch"] = epoch;
    snapshot["GID"] = groupId;
    string memberlist = util::convertSetToStr(members, 0);
    snapshot["G"] = memberlist;
    string senderSeqStr = util::convertMapToStr(senderSeqs);
    snapshot["senderSeqs"] = senderSeqStr;

    json lastState;
    lastState["index"] = state.first;
    lastState["state"] = state.second;
    snapshot["last_state"] = lastState;
    return snapshot.dump();
}

void ConsensusBasedCtrl::handleCCNAck(CCNAck* ccnack) {
    UDPControlInfo *ccnackCtrl = check_and_cast<UDPControlInfo *>(
            ccnack->getControlInfo());
    IPvXAddress& srcAddr = ccnackCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = ccnack->getSenderName();
    // retransmit coordinate change request
    string candidate = coordinator();
    string addrStr = ipAddress.get4().str();
    if (candidate.compare(addrStr) == 0) {
        EV << "[" << simTime() << "s] " << fullName
                  << " retransmit coordinator change request to " << senderName
                  << endl;

        // if self is the coordinator candidate
        coordCand = addrStr;

        // send coord_query to the reply host, if the host is still a live member
        if (survivals.count(sender) > 0 && members.count(sender) > 0) {
            CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
            coordChange->setCandidate(addrStr.c_str());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            coordChange->setControlInfo(udpControlInfo);
            r_transmit(coordChange, srcAddr);
        }
    } else {
        coordCand.clear();
    }

    delete ccnack;
}

void ConsensusBasedCtrl::handleCCQueryReply(CCQueryReply* ccr) {
    UDPControlInfo *ccrCtrl = check_and_cast<UDPControlInfo *>(
            ccr->getControlInfo());
    IPvXAddress& srcAddr = ccrCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = ccr->getSenderName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change reply from " << senderName
              << endl;

    // only handle messages from survived members, TODO but this condition may not needed
    set<string> survived_members = survivedMembers();
    if (survived_members.count(sender) > 0) {
        CCSnapshots[sender] = ccr->getState();
        checkCCSnapshos();
    }

    delete ccr;
}

void ConsensusBasedCtrl::checkCCSnapshos() {

    bool allReceived = true;

    set<string> survived_members = survivedMembers();
    // check whether all replies from  hosts on the Survivals list have been received
    for (set<string>::iterator it = survived_members.begin();
            it != survived_members.end(); ++it) {
        string hostName = *it;
        if (CCSnapshots.find(hostName) == CCSnapshots.end()) {
            allReceived = false;
            break;
        }
    }

    if (allReceived) {
        json snapshot = computeCCSnapshot();
        CCEnd* cce = new CCEnd(msg::LOAD_COORD);
        cce->setSenderName(fullName);
        cce->setEpoch(snapshot["epoch"]);
        bool reconfig = snapshot["reconfig"];
        cce->setReconfig(reconfig);
        cce->setQd(snapshot["Qd"].get<string>().c_str());
        cce->setInit(snapshot["init"].dump().c_str());
        if (reconfig) {
            cce->setGroupId(snapshot["GID"]);
            cce->setGroupMembers(snapshot["memberlist"].get<string>().c_str());
        }
        // message size: 1 byte for epoch + snapshot size
        cce->setByteLength(1 + snapshot.dump().size());
        r_multicast(cce, survived_members);
    }
}

json ConsensusBasedCtrl::computeCCSnapshot() {
    json snapshot;
    json init;
    bool reconfig = false;
    int highest_epoch = epoch;
    map<unsigned long, int> highest_senderSeqs;
    map<int, boost::tuple<unsigned long, int, string> > Qd_merge;
    for (auto elem : CCSnapshots) {
        string replica_state = elem.second;
        json jState = json::parse(replica_state);
        // retrieve the highest epoch
        int replica_epoch = jState["epoch"];
        if (replica_epoch > highest_epoch) {
            highest_epoch = replica_epoch;
        }
        /*
         * Check whether state sync is needed (i.e., last reconfiguration has been done).
         */
        int replica_gid = jState["GID"];
        if (replica_gid != groupId) {
            reconfig = true;
        }

        // get the highest sequence of events for each event sender
        string senderSeqStr = jState["senderSeqs"];
        if (!senderSeqStr.empty()) {
            map<unsigned long, int> senderSeqs_temp;
            util::convertStrToMap(senderSeqStr, senderSeqs_temp);
            for (auto elem : senderSeqs_temp) {
                unsigned long senderId = elem.first;
                int sequence = elem.second;
                if (highest_senderSeqs.count(senderId) == 0
                        || highest_senderSeqs[senderId] < sequence) {
                    highest_senderSeqs[senderId] = sequence;
                }
            }
        }

        // merge decisions
        string qd_str = jState["Qd"].get<string>();
        if (!qd_str.empty()) {
            map<int, boost::tuple<unsigned long, int, string> > Qd_temp;
            util::convertStrToMap(qd_str, Qd_temp);
            for (map<int, boost::tuple<unsigned long, int, string> >::iterator it =
                    Qd_temp.begin(); it != Qd_temp.end(); it++) {
                int eventIndex = it->first;
                boost::tuple<unsigned long, int, string> event = it->second;
                Qd_merge[eventIndex] = event;
            }
        }
    }
    snapshot["epoch"] = highest_epoch;
    snapshot["reconfig"] = reconfig;
    string merged_Qd = util::convertMapToStr(Qd_merge);
    snapshot["Qd"] = merged_Qd;
    json localState = json::parse(CCSnapshots[ipAddress.get4().str()]);
    init["initial_state"] = localState["last_state"];
    init["senderSeqs"] = util::convertMapToStr(highest_senderSeqs);

    if (reconfig) {
        int highest_gid = groupId;
        string latest_members;
        for (auto elem : CCSnapshots) {
            string replica_state = elem.second;
            json jState = json::parse(replica_state);

            /*
             * retrieve the latest configuration: if some replica has seen a new configuration, use the new one.
             * Otherwise, use the existing one.
             */
            int replica_gid = jState["GID"];
            if (replica_gid >= highest_gid) {
                highest_gid = replica_gid;
                latest_members = jState["memberlist"];
            }
        }

        snapshot["GID"] = highest_gid;
        snapshot["memberlist"] = latest_members;
        string senderAddrList = util::convertMapToStr(clients);
        string RgList = util::convertMapToStr(Rg);
        init["senderAddrs"] = senderAddrList;
        init["Rg"] = RgList;
    }

    snapshot["init"] = init;

    return snapshot;
}

void ConsensusBasedCtrl::handleCCEnd(CCEnd* cce) {
    UDPControlInfo *cceCtrl = check_and_cast<UDPControlInfo *>(
            cce->getControlInfo());
    IPvXAddress& srcAddr = cceCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = cce->getSenderName();
    if (survivals.count(sender) > 0) {
        string c = coordinator();
        if (c.compare(sender) == 0) {
            coord = sender;
            epoch = cce->getEpoch() + 1;
            groupId = cce->getGroupId();

            // state sync
            string init_str = cce->getInit();
            json init = json::parse(init_str);
            json initialState = init["initial_state"];
            state.first = initialState["index"];
            state.second = initialState["state"];
            lastDelivered = state.first;

            // load Qd
            string Qd_str = cce->getQd();
            util::convertStrToMap(Qd_str, Qd);
            // update the sequence of delivered events for each event sender
            string senderSeqsStr = init["senderSeqs"];
            util::convertStrToMap(senderSeqsStr, senderSeqs);

            // garbage collection for Qr
            clearQr();

            // reset consensus
            proposals.clear();

            if (cce->getReconfig()) {
                // state sync for any unfinished reconfiguration
                if (groupId < 0) {
                    scheduleAt(simTime() + gcCycle, GCTimeout);
                }

                groupId = gid_temp = cce->getGroupId();

                string senderAddrList = init["senderAddrs"];
                util::convertStrToMap(senderAddrList, clients);
                string RgList = init["Rg"];
                util::convertStrToMap(RgList, Rg);

                // load the new group configuration
                string memberlist = cce->getGroupMembers();
                map<string, int> groupMembers;
                util::convertStrToMap(memberlist, groupMembers);
                // load new group member list
                seniorities.clear();
                members.clear();
                for (map<string, int>::iterator it = groupMembers.begin();
                        it != groupMembers.end(); it++) {
                    string host = it->first;
                    int seniority = it->second;
                    members.insert(it->first);
                    if (survivals.count(host) > 0 && members.count(host) > 0) {
                        // update seniority
                        seniorities[host] = seniority + 1;
                    }
                }
            }

            coordCand.clear();

            if (coord.compare(ipAddress.get4().str()) != 0) {
                mode = EH;
            }

            CCAck* ack = new CCAck(msg::CC_ACK);
            ack->setSenderName(fullName);
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            ack->setControlInfo(udpControlInfo);
            r_transmit(ack, srcAddr);

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

void ConsensusBasedCtrl::handleCCAck(CCAck* ack) {
    UDPControlInfo *ackCtrl = check_and_cast<UDPControlInfo *>(
            ack->getControlInfo());
    IPvXAddress& srcAddr = ackCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = ack->getSenderName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change acknowledgement from "
              << senderName << endl;

    // only handle messages from survived members, TODO but this condition may not needed
    set<string> survived_members = survivedMembers();
    if (survived_members.count(sender) > 0) {
        CCAcks.insert(sender);
        checkCCAcks();
    }

    delete ack;
}

void ConsensusBasedCtrl::checkCCAcks() {
    bool allReceived = true;

    set<string> survived_members = survivedMembers();
    // check whether all replies from  hosts on the Survivals list have been received
    for (set<string>::iterator it = survived_members.begin();
            it != survived_members.end(); ++it) {
        string hostName = *it;
        if (CCAcks.find(hostName) == CCAcks.end()) {
            allReceived = false;
            break;
        }
    }

    if (allReceived) {
        mode = EH;
        if (!ECTimeout->isScheduled()) {
            // prepare for the next cycle
            scheduleAt(simTime() + roundCycle, ECTimeout);
        }
        GlobalStatisticsAccess().get()->increaseLeaderElectionEnd();
    }
}

bool ConsensusBasedCtrl::checkConfig() {
    bool triggerGR = false;
    set<string> newMembers;
    set_difference(survivals.begin(), survivals.end(), members.begin(),
            members.end(), inserter(newMembers, newMembers.begin()));
    // Upon R \ G ¡Ù empty ¡Ä R ¡Ù G_T (see the original paper)
    if (newMembers.size() > 0 && survivals != membersTemp) {
        membersTemp = survivals;
        triggerGR = true;
    }

    if (triggerGR) {
        // coordinator initiate
        if (coord.compare(ipAddress.get4().str()) == 0) {
            EV << "[" << simTime() << "s] " << fullName
                      << " start a reconfiguration" << endl;

            mode = GR;
            // abort the current consensus instance
            idle = true;

            // in case a GR is interrupted by another GR
            gid_temp = gid_temp > groupId ? (gid_temp + 1) : (groupId + 1);
            // for interrupting another reconfiguration
            GRSnapshots.clear();

            // send G' to all hosts on the Survivals list
            GRQuery* reconfig = new GRQuery(msg::GROUP_RECONFIG);
            reconfig->setSenderName(fullName);
            reconfig->setEpoch(epoch);
            reconfig->setGroupId(gid_temp);
            r_multicast(reconfig, membersTemp);

            GlobalStatisticsAccess().get()->increaseReconfigurationStart();
        }
        return true;
    } else {
        return false;
    }
}

void ConsensusBasedCtrl::handleGRQuery(GRQuery* gr) {
    IPvXAddress& srcAddr = check_and_cast<UDPControlInfo *>(
            gr->getControlInfo())->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = gr->getSenderName();
    int e = gr->getEpoch();
    int gid = gr->getGroupId();

    EV << "[" << simTime() << "s] " << fullName
              << " received reconfiguration query from " << senderName << endl;

    // message validation
    if (e >= epoch && gid >= groupId && mode != CC) {

        mode = GR;
        EV << "[" << simTime() << "s] " << fullName
                  << " entered the reconfiguration mode" << endl;

        gid_temp = gid;

        // update coordinator info for new members
        if (groupId < 0) {
            epoch = e;
            coord = sender;
        }

        GRQueryReply* grr = new GRQueryReply(msg::RECONFIG_READY);
        grr->setSenderName(fullName);
        grr->setEpoch(epoch);
        grr->setGroupId(gid_temp);
        // send events for state sync
        string snapshot = buildGRSnapshot();
        grr->setState(snapshot.c_str());
        // message size: snapshot size + 1 byte for epoch + 1 byte for group ID
        grr->setByteLength(snapshot.size() + 2);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        grr->setControlInfo(udpControlInfo);

        r_transmit(grr, srcAddr);
    } else {
        // in case of GID competition, or that the last leader election end messae has not been received
        GRNAck* nack = new GRNAck(msg::GR_NACK);
        nack->setSenderName(fullName);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        nack->setControlInfo(udpControlInfo);
        r_transmit(nack, srcAddr);
    }

    delete gr;
}

string ConsensusBasedCtrl::buildGRSnapshot() {
    json snapshot;
    string Qdstr = util::convertMapToStr(Qd);
    snapshot["Qd"] = Qdstr;
    string senderSeqStr = util::convertMapToStr(senderSeqs);
    snapshot["senderSeqs"] = senderSeqStr;

    json lastState;
    lastState["index"] = state.first;
    lastState["state"] = state.second;
    snapshot["last_state"] = lastState;

    return snapshot.dump();
}

void ConsensusBasedCtrl::handleGRNAck(GRNAck* grnack) {
    UDPControlInfo *grnackCtrl = check_and_cast<UDPControlInfo *>(
            grnack->getControlInfo());
    IPvXAddress& srcAddr = grnackCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = grnack->getSenderName();
    // restart the reconfiguration process with a higher temporary GID
    if (survivals.count(sender)) {
        membersTemp.clear();
        int replica_gid = grnack->getGidTemp();
        gid_temp = replica_gid > gid_temp ? (replica_gid + 1) : (gid_temp + 1);
    }

    delete grnack;
}

void ConsensusBasedCtrl::handleGRQueryReply(GRQueryReply* grr) {
    IPvXAddress& srcAddr = check_and_cast<UDPControlInfo *>(
            grr->getControlInfo())->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = grr->getSenderName();
    int e = grr->getEpoch();
    int gid = grr->getGroupId();

    EV << "[" << simTime() << "s] " << fullName
              << " received group reconfiguration reply from " << senderName
              << endl;

    // only handle messages from survived hosts in neighborsTemp
    if (e == epoch && gid == gid_temp && mode == GR
            && coord.compare(ipAddress.get4().str()) == 0
            && membersTemp.count(sender) > 0 && survivals.count(sender) > 0) {
        GRSnapshots[sender] = grr->getState();
        checkGRSnapshots();
    }

    delete grr;
}

void ConsensusBasedCtrl::checkGRSnapshots() {
    if (mode != CC) {
        bool allReceived = true;
        set<string> intersection;
        set_intersection(survivals.begin(), survivals.end(),
                membersTemp.begin(), membersTemp.end(),
                inserter(intersection, intersection.begin()));
        // check whether all replies from  hosts on the Survivals list have been received
        for (set<string>::iterator it = intersection.begin();
                it != intersection.end(); ++it) {
            string host = *it;
            if (GRSnapshots.count(host) == 0) {
                allReceived = false;
                break;
            }
        }

        if (allReceived) {
            json snapshot = computeGRSnapshot();

            if (hasGUI()) {
//                cout << "[" << simTime() << "][" << fullName << "] snapshot: "
//                        << snapshot << endl;
            }

            GREnd* gre = new GREnd(msg::LOAD_CONFIG);
            gre->setSenderName(fullName);
            gre->setEpoch(epoch);
            gre->setGroupId(gid_temp);
            gre->setQd(snapshot["Qd"].get<string>().c_str());
            gre->setInit(snapshot["init"].dump().c_str());
            gre->setGroupMembers(snapshot["memberlist"].get<string>().c_str());
            // message size: 1 byte for epoch + 1 byte for group Id + snapshot size
            gre->setByteLength(1 + 1 + snapshot.dump().size());
            r_multicast(gre, intersection);
        }
    }
}

json ConsensusBasedCtrl::computeGRSnapshot() {
    json snapshot;
    json init;
    map<unsigned long, int> highest_senderSeqs;
    map<int, boost::tuple<unsigned long, int, string> > Qd_merge;
    for (map<string, string>::iterator it = GRSnapshots.begin();
            it != GRSnapshots.end(); it++) {
        string replica = it->first;
        string replica_state = it->second;
        json jstate = json::parse(replica_state);

        if (hasGUI()) {
//            cout << "[" << simTime() << "][" << fullName
//                    << "]  state of replica [" << replica << "]: " << jstate
//                    << endl;
        }

        // get the highest sequence of events for each event sender
        string senderSeqStr = jstate["senderSeqs"];
        if (!senderSeqStr.empty()) {
            map<unsigned long, int> senderSeqs_temp;
            util::convertStrToMap(senderSeqStr, senderSeqs_temp);
            for (auto elem : senderSeqs_temp) {
                unsigned long senderId = elem.first;
                int sequence = elem.second;
                if (highest_senderSeqs.count(senderId) == 0
                        || highest_senderSeqs[senderId] < sequence) {
                    highest_senderSeqs[senderId] = sequence;
                }
            }
        }

        // merge decisions
        string qd_str = jstate["Qd"].get<string>();
        if (!qd_str.empty()) {
            map<int, boost::tuple<unsigned long, int, string> > Qd_temp;
            util::convertStrToMap(qd_str, Qd_temp);
            for (map<int, boost::tuple<unsigned long, int, string> >::iterator it =
                    Qd_temp.begin(); it != Qd_temp.end(); it++) {
                int eventIndex = it->first;
                boost::tuple<unsigned long, int, string> event = it->second;
                Qd_merge[eventIndex] = event;
            }
        }
    }
    string merged_Qd = util::convertMapToStr(Qd_merge);
    snapshot["Qd"] = merged_Qd;
    json localState = json::parse(GRSnapshots[ipAddress.get4().str()]);
    init["initial_state"] = localState["last_state"];
    init["senderSeqs"] = util::convertMapToStr(highest_senderSeqs);

    // prepare initialization state for new replicas
    string senderAddrList = util::convertMapToStr(clients);
    string RgList = util::convertMapToStr(Rg);
    init["senderAddrs"] = senderAddrList;
    init["Rg"] = RgList;
    snapshot["init"] = init;

    // retrieve new group members from G'
    // maps of (IPAddress, age) pair
    map<string, int> groupMembers;
    for (set<string>::iterator it = membersTemp.begin();
            it != membersTemp.end(); it++) {
        string memberAddr = *it;
        int memberAge = seniorities[memberAddr];
        groupMembers[*it] = memberAge;
    }
    string memberlist = util::convertMapToStr(groupMembers);
    snapshot["memberlist"] = memberlist;

    return snapshot;
}

void ConsensusBasedCtrl::handleGREnd(GREnd* gre) {
    IPvXAddress& srcAddr = check_and_cast<UDPControlInfo *>(
            gre->getControlInfo())->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = gre->getSenderName();
    int e = gre->getEpoch();
    int gid = gre->getGroupId();
//    if (e == epoch && gid == groupId && mode == GR
//            && coord.compare(sender) == 0) {
    if (e == epoch && gid == gid_temp && mode == GR
            && coord.compare(sender) == 0) {
        // Sync inital states
        string init_str = gre->getInit();
        json init = json::parse(init_str);
        if (groupId < 0) {
            scheduleAt(simTime() + gcCycle, GCTimeout);
        }

        groupId = gid_temp;

        string senderAddrList = init["senderAddrs"];
        util::convertStrToMap(senderAddrList, clients);
        string RgList = init["Rg"];
        util::convertStrToMap(RgList, Rg);
        json initialState = init["initial_state"];
        state.first = initialState["index"];
        state.second = initialState["state"];
        lastDelivered = state.first;

        // load Qd and E (i.e., decided events)
        string Qd_str = gre->getQd();
        util::convertStrToMap(Qd_str, Qd);
        // update the sequence of delivered events for each event sender
        string senderSeqsStr = init["senderSeqs"];
        util::convertStrToMap(senderSeqsStr, senderSeqs);
        // garbage collection for Qr
        clearQr();

        // reset consensus
        proposals.clear();

        // load the new group configuration
        string memberlist = gre->getGroupMembers();
        map<string, int> groupMembers;
        util::convertStrToMap(memberlist, groupMembers);
        // load new group member list
        seniorities.clear();
        members.clear();
        for (map<string, int>::iterator it = groupMembers.begin();
                it != groupMembers.end(); it++) {
            string host = it->first;
            int seniority = it->second;
            members.insert(it->first);
            if (survivals.count(host) > 0 && members.count(host) > 0) {
                // update seniority
                seniorities[host] = seniority + 1;
            }
        }

        // resume event handling
        membersTemp.clear();
        mode = EH;

        // notify configuration change to senders
        if (coord.compare(ipAddress.get4().str()) == 0) {
            set<string> configs;
            for (auto elem : members) {
                configs.insert(elem);
            }
            string configList = util::convertSetToStr(configs, 0);
            set<string> eventSenderList;
            for (auto elem : clients) {
                string senderAddr = elem.second;
                eventSenderList.insert(senderAddr);
            }
            ConfigUpdate* configeUpdate = new ConfigUpdate(msg::CONFIG_UPDATE);
            configeUpdate->setConfigs(configList.c_str());
            configeUpdate->setSourceName(fullName);
            configeUpdate->setLCName(LCName.c_str());
            // message size: size of the new configuration list + size of the logical computer name
            configeUpdate->setByteLength(configList.size() + LCName.size());
            r_multicast(configeUpdate, eventSenderList);

            GlobalStatisticsAccess().get()->increaseReconfigurationEnd();
        }

        EV << "[" << simTime() << "s] " << fullName
                  << " loaded the new configuration" << endl;
    } else {
        EV << "[" << simTime() << "s] reconfiguration from " << fullName
                  << " is invalid" << endl;
    }

    delete gre;
}

set<string> ConsensusBasedCtrl::survivedMembers() {
    set<string> survived_members = util::intersectSets(survivals, members);
    return survived_members;
}

IPvXAddress ConsensusBasedCtrl::loadIPAddress() {
    InterfaceTable* ift = InterfaceTableAccess().get(getParentModule());
    string IPInterface = getParentModule()->par("IPInterface");
    InterfaceEntry* ie = ift->getInterfaceByName(IPInterface.c_str());
    IPv4InterfaceData* ipData = ie->ipv4Data();
    IPvXAddress addr(ipData->getIPAddress());
    return addr;
}

void ConsensusBasedCtrl::syncClock() {
    Clock* clock = check_and_cast<Clock*>(
            getParentModule()->getSubmodule("clock"));
    clockError = clock->getError();
}
