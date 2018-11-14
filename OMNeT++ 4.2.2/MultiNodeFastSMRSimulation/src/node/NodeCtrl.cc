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

#include "NodeCtrl.h"

Define_Module(NodeCtrl);

NodeCtrl::NodeCtrl() :
        lastDelivered(-1, 0, 0) {
    esc = 0;
    escId = 0;
    // to make sure the first round is round 0
    round = -1;
    consensus = 0;
    deliveryRound = 0;
    appliedRound = 0;
    round_query = 0;
    isMember = false;
//    lastApplied = -1;
    sequence = 0;
    STAGE_NUM = stage::NODE_CTRL_INIT;

    updateTimeout = new cMessage(msg::UPDATE_CYCLE);
    ECTimeout = new cMessage(msg::EC_START);
    GCTimeout = new cMessage(msg::GC_TIMEOUT);
    hbTimeout = new HBTimeout(msg::HEARTBEAT_TIMEOUT);
}

NodeCtrl::~NodeCtrl() {
    if (updateTimeout != NULL) {
        cancelAndDelete(updateTimeout);
    }
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

int NodeCtrl::numInitStages() const {
    return STAGE_NUM;
}

void NodeCtrl::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();

        gId = sdbm::encode(par("groupName"));
        roundCycle = par("roundCycle");
        //    applyCycle = par("applyCycle");
        cycleStart = par("cycleStart");
        next_round_time = cycleStart;
        gcCycle = par("GCCycle");

        WATCH(coord);
        WATCH(mode);
        WATCH(epoch);
        WATCH(groupId);
        WATCH_SET(neighbors);
        WATCH_SET(neighborsTemp);
        WATCH_SET(survivals);
        WATCH_MAP(seniorities);
        WATCH_MAP(clients);
        WATCH(esc);
        WATCH(escId);
        WATCH_MAP(toPropose);
        WATCH_SET(proposed);
        WATCH_MAP(senders);
        WATCH_MAP(Nc);
        WATCH_MAP(JL);
        WATCH_MAP(Nr);
        WATCH(round);
        WATCH(deliveryRound);
        WATCH(appliedRound);

        WATCH_MAP(Qr);
        WATCH_MAP(Qd);
        WATCH_MAP(Qp);
        //    WATCH_MAP(Qa);
        //    WATCH_MAP(Qs);
        WATCH_MAP(Bs);
        //    WATCH_MAP(Qsend);
        WATCH(lastDelivered);
        //    WATCH(lastApplied);
        WATCH(consensus);
        WATCH(round_query);
        WATCH_SET(query_round);
        WATCH(next_round_time);
    }

    if (stage == 1) {
        // init purger
        // add self to purger
//        Purger* purger = getPurger();
//        purger->addHost(fullName);

        // init host neighbors
        groupId = 0;
        // initial stage to avoid starting from EH
        mode = GR;
        epoch = 0;
        ipAddress = loadIPAddress();
        //            scheduleAt(simTime() + roundCycle, EHTimeout);
        //            scheduleAt(simTime() + applyCycle, updateTimeout)
    }
}

void NodeCtrl::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::HEARTBEAT)) {
        HBProbe* hb = check_and_cast<HBProbe*>(msg);
        handleHeartbeat(hb);

        // stage transfer
        stageTransfer();

        // check whether there is any event in Qp
        handleEvent();

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
        receiveEvent(event);
    } else if (msg->isName(msg::CONSENSUS_QUERY)) {
        ConsensusQuery * cq = check_and_cast<ConsensusQuery*>(msg);
        this->startPropose(cq);
    } else if (msg->isName(msg::PROPOSAL)) {
        Proposal* proposal = check_and_cast<Proposal*>(msg);
        collectProposal(proposal);
    } else if (msg->isName(msg::DECISION)) {
        Decision* decision = check_and_cast<Decision*>(msg);
        handleDecision(decision);
    } else if (msg->isName(msg::UPDATE_CYCLE)) {
        disseminateEvent();
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

void NodeCtrl::handleHeartbeat(HBProbe *hb) {
    UDPControlInfo *hbCtrl = check_and_cast<UDPControlInfo *>(
            hb->getControlInfo());
    IPvXAddress& srcAddr = hbCtrl->getSrcAddr();
    rendezvous = srcAddr;
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
        // TODO May leave neighbors change to GR.
        // If so, then map<string, int> map3 = intersectMaps(neighbors, survivals) should be applied
        neighbors.erase(toRemove[i]);
        survivals.erase(toRemove[i]);
        // remove seniority
        seniorities.erase(toRemove[i]);
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
        initNode();
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
//    EV << "[" << simTime() << "s] " << fullName << " sends a heartbeat response"
//              << endl;
    hbr->setControlInfo(udpControlInfo);
    sendReliable(hbr);

    // set heartbeat timeout
    cancelEvent(hbTimeout);
    simtime_t cycle = getParentModule()->par("HBcycle");
    scheduleAt(simTime() + cycle, hbTimeout);

    // delete the received msg
    delete hb;
}

void NodeCtrl::handleHeartbeatTimeout(HBTimeout* hbt) {
    UnderlayConfiguratorAccess().get()->revokeNode(ipAddress);
}

void NodeCtrl::initNode() {
    groupId = 1;
    neighbors.insert(survivals.begin(), survivals.end());
    for (set<string>::iterator it = neighbors.begin(); it != neighbors.end();
            it++) {
        seniorities[*it] = 1;
    }
    isMember = true;

    scheduleAt(cycleStart, ECTimeout);
    scheduleAt(cycleStart, updateTimeout);
    scheduleAt(cycleStart + gcCycle, GCTimeout);
}

void NodeCtrl::handleJoin(InitJoin* initJoin) {
    int seq_s = initJoin->getSeq();
    unsigned long senderId = initJoin->getSenderId();
    string senderName = initJoin->getSourceName();
    simtime_t sendTime = initJoin->getJoinTime();
    UDPControlInfo* initJoinCtrl = check_and_cast<UDPControlInfo *>(
            initJoin->getControlInfo());
    IPvXAddress srcAddr = initJoinCtrl->getSrcAddr();
    string addrStr = srcAddr.get4().str();
    clients[senderName] = addrStr;

    // let join time be (20 cycles, including the span to the subsequent cycle) later
    simtime_t current = readClock();
    double timeToCycleStart = sendTime.dbl() - cycleStart.dbl();
    // join time is calculated from the sending time
    double joinTime =
            timeToCycleStart >= 0 ?
                    (sendTime.dbl() + roundCycle.dbl()
                            - fmod(timeToCycleStart, roundCycle.dbl())
                            + 19 * roundCycle.dbl()) :
                    (cycleStart.dbl() + 20 * roundCycle.dbl());
    double recvTime = joinTime + roundCycle.dbl();
    int joinRound = static_cast<int>(ceil(
            (recvTime - cycleStart.dbl()) / roundCycle.dbl()));
//    NSenders[senderId] = seq_s + 1 - joinRound;
    senders[senderId] = seq_s + 1 - joinRound;
    // time event receiving start
    Nc[senderId] = joinRound;
    JL[senderId] = seq_s;
    // add update dissemination recipient
//    recipients[senderId] = 0 - joinRound;
    Nr[senderId] = joinRound;
    map<string, string> addresses;
    addresses[clients[senderName]] = senderName;
//    tuple_rg rg(addresses, -1);
    Rg[senderId] = addresses;
//    vector<string> qsend;
//    Qsend[senderId] = qsend;

    EV << "[debug] cycle start: " << cycleStart << endl;
    EV << "[debug] message timestamp: " << sendTime << endl;
    EV << "[debug] current time: " << current << endl;
    EV << "[debug] join sequence (seq_s + 1): " << seq_s + 1 << endl;
    EV << "[debug] join time (t_s): " << joinTime << endl;
    EV << "[debug] receive time (t_recv): " << recvTime << endl;
    EV << "[debug] join round: " << joinRound << endl;

    Join* join = new Join(msg::JOIN);
    join->setSenderName(fullName);
    join->setCycleLength(roundCycle);
    join->setJoinTime(joinTime);
    join->setSenderId(gId);
//    join->setJoinTime(cycleStart - roundCycle);
    UDPControlInfo* udpControlInfo = new UDPControlInfo();
    udpControlInfo->setDestAddr(srcAddr);
    udpControlInfo->setSrcAddr(ipAddress);
    join->setControlInfo(udpControlInfo);
    sendReliable(join);

    delete initJoin;
}

void NodeCtrl::receiveEvent(Event* event) {
    unsigned long senderId = event->getSenderId();
    int seq = event->getSeq();
//    if (seq >= sors[senderId]) {
    string content = event->getContent();
    tuple_qr t(senderId, seq);
    Qr[t] = content;
//    }

    delete event;
}

void NodeCtrl::collectRoundEvents() {
    if (isMember) {
        round++;
        EV << fullName << " round: " << round << endl;

        // check sender start round
//        for (map<unsigned long, int>::iterator it = Nc.begin();
//                it != Nc.end(); it++) {
//            unsigned long senderId = it->first;
//            int joinRound = it->second;
//            if(round >= joinRound){
//                senders[senderId] = NSenders[senderId];
//            }
//        }

        // on Coordinator, if the events of the round have been received from other hosts before
//        if (Qp.find(round) == Qp.end()) {
//            map<unsigned long, string> QR;

        // determine QR
        for (map<unsigned long, int>::iterator it = senders.begin();
                it != senders.end(); it++) {
            unsigned long senderId = it->first;
            if (Nc[senderId] > round) {
                continue;
            }

            // determine the sequence of round: seq_s + (r - r_s) where r = round
            int sor = it->second + round;

            // check whether the event of the round has been received
            tuple_qr qrKey(senderId, sor);
            tuple_qp qpKey(round, senderId, sor);
            if (Qr.count(qrKey) > 0) {
//                    QR[senderId] = Qr[t];
                Qp[qpKey] = Qr[qrKey];
                // remove events from Qr
                Qr.erase(qrKey);

            } else {
//                    QR[senderId].clear();
                Qp[qpKey] = event::MISS_EVENT;
            }
        }

        // collect events of previous rounds
        for (int i = 0; i < round; i++) {
            for (map<unsigned long, int>::iterator it = senders.begin();
                    it != senders.end(); it++) {
                unsigned long senderId = it->first;
                if (Nc[senderId] > i) {
                    continue;
                }
                // determine the sequence of round: seq_s + (r - r_s) where r = round
                int sor = it->second + i;

                tuple_qr qrKey(senderId, sor);
//                tuple_qp qpKey(round, senderId, sor);
                tuple_qp qpKey(i, senderId, sor);
                if (Qr.count(qrKey) > 0) {
                    Qp[qpKey] = Qr[qrKey];
                    // remove events from Qr
                    Qr.erase(qrKey);

                }
            }
        }

        // handle un-processed consensus events
        for (map<int, string>::iterator it = Qu.begin(); it != Qu.end(); it++) {
            int r = it->first;
            if (r <= round) {
                string seqQueryRange = it->second;
                ConsensusQuery* cq = new ConsensusQuery(msg::CONSENSUS_QUERY);
                cq->setRound(r);
                cq->setEpoch(epoch);
                cq->setGroupId(groupId);
                cq->setSenderName(fullName);
                cq->setSeqRanges(seqQueryRange.c_str());
                startPropose(cq);
            }
        }

        // send QR to the coordinator, if there is no missing event in it
//            if (complete && coord.compare(fullName) != 0) {
//                CycleEvent* ce = new CycleEvent(msg::CYCLE_EVENT);
//                ce->setSenderName(fullName);
//                ce->setRound(round);
//                string cycleEvents = util::convertMapToStr(QR);
//                ce->setCycleEvents(cycleEvents.c_str());
//                transmit(ce, coord, coordGate);
//            }

        // prepare for the next cycle
        next_round_time += roundCycle;
        scheduleAt(next_round_time, ECTimeout);

        // handle event
        handleEvent();

//        }
    }
}

void NodeCtrl::deliverCycleEvent(CycleEvent* ce) {
//    int round = ce->getRound();
//    map<unsigned long, string> cycleEvents;
//    string cycleEventStr = ce->getCycleEvents();
//    util::convertStrToMap(cycleEventStr, cycleEvents);
//
//    if (Qd.find(round) == Qd.end()) {
//        Qp.erase(round);
//        Qp[round].insert(cycleEvents.begin(), cycleEvents.end());
//
//        // for observation
////        decodeQp();
//    }
//
//    delete ce;
}

void NodeCtrl::handleEvent() {

    // retrieve and handle the events of missing rounds first
//    int firtUndeliveredRound = 0;
//    if (!Qp.empty()) {
//        firtUndeliveredRound = Qp.rbegin()->first;
//    } else if (!Qd.empty()) {
//        firtUndeliveredRound = Qd.rbegin()->first;
//    }

//    EV << "current first undelivered round: " << firtUndeliveredRound << endl;

//    for (int i = firtUndeliveredRound + 1; i < round; i++) {
//        // send QUERY to coordinator to trigger a consensus instance
//        RoundQuery* query = new RoundQuery(msg::ROUND_QUERY);
//        query->setSenderName(fullName);
//        query->setDestName(coord.c_str());
//        query->setRound(i);
//        if (coord.compare(fullName) != 0) {
//            transmit(query, coord, coordGate);
//        } else {
//            delete query;
//            // fake the send-to-self process
//            toPropose.insert(i);
//        }
//    }

//    if (!Qp.empty() && firtUndeliveredRound == round) {

    while (round > deliveryRound) {
        deliveryRound++;

        // check whether round r is ready
        if (proposed.count(deliveryRound) > 0) {
            deliveryRound--;
            break;
        }

        // tuple (round#, sender ID, sequence number, event content)
        typedef tuple<int, unsigned long, int> tuple_d;
        map<tuple_d, string> Delivery;

        if (Qc.count(deliveryRound) > 0) {
            pair<multimap<int, tuple_qc>::iterator
                    , std::multimap<int, tuple_qc>::iterator> ret =
                    Qc.equal_range(deliveryRound);
            for (multimap<int, tuple_qc>::iterator it = ret.first;
                    it != ret.second; ++it) {
                int key = it->first;
                tuple_qc tuple = it->second;
                tuple_d dKey(key, get<0>(tuple), get<1>(tuple));
                Delivery[dKey] = get<2>(tuple);
            }
        } else {
            // tuple (round#, sender ID, sequence number, event content)
            typedef tuple<int, unsigned long, int> tuple_e;
            map<tuple_e, string> Events;

            for (map<unsigned long, int>::iterator it = senders.begin();
                    it != senders.end(); it++) {
                // calculate the lowest sequence number of the sender
                unsigned long senderId = it->first;
                if (Nc[senderId] > deliveryRound) {
                    continue;
                }
                int lowestDeliverable = JL[senderId] + 1;
                // calculate the sequence number of the round
                int sor = it->second + deliveryRound;
                for (int i = lowestDeliverable; i <= sor; i++) {
                    int r = deliveryRound - (sor - i);
                    tuple_e eKey(deliveryRound, senderId, i);
                    tuple_qp pKey(r, senderId, i);
                    Events[eKey] = Qp[pKey];
                }
            }

            // check whether Events contains any missing event
            bool completeRound = true;
            for (map<tuple_e, string>::iterator it = Events.begin();
                    it != Events.end(); it++) {
                string event = it->second;
                if (event.compare(event::MISS_EVENT) == 0) {
                    completeRound = false;
                    break;
                }
            }
            if (!completeRound) {
                round_query++;
                query_round.insert(deliveryRound);

                // calculate the range of sequence number for each sender for query
                stringstream ss;
                for (map<unsigned long, int>::iterator it = senders.begin();
                        it != senders.end(); it++) {
                    unsigned long senderId = it->first;
                    if (Nc[senderId] > deliveryRound) {
                        continue;
                    }
                    int lowestDeliverable = JL[senderId] + 1;
                    int sor = it->second + deliveryRound;
                    ss << senderId << "," << lowestDeliverable << "," << sor
                            << ";";
                }
                string seqRanges = ss.str();
                // If contain any missing event, send QUERY to coordinator to trigger a consensus instance
                RoundQuery* query = new RoundQuery(msg::ROUND_QUERY);
                query->setSenderName(fullName);
                query->setDestName(coord.c_str());
                query->setRound(deliveryRound);
                query->setSeqRanges(seqRanges.c_str());
                UDPControlInfo* udpControlInfo = new UDPControlInfo();
                IPvXAddress coordAddr = getIPAddress(coord);
                udpControlInfo->setDestAddr(coordAddr);
                udpControlInfo->setSrcAddr(ipAddress);
                query->setControlInfo(udpControlInfo);
                transmit(query, coordAddr);
                // mark the round undeliverable
                deliveryRound--;
                break;
            } else {
                Delivery.insert(Events.begin(), Events.end());
            }
        }
        if (!Delivery.empty()) {
            // deliver events to Qd
//                int lambda = 0;
            for (map<tuple_d, string>::iterator it = Delivery.begin();
                    it != Delivery.end(); it++) {
                tuple_d dKey = it->first;
                int r = get<0>(dKey);
                unsigned long senderId = get<1>(dKey);
                int seq = get<2>(dKey);
                string event = it->second;
                tuple_qd key(r, senderId, seq);
                Qd[key] = event;
                // update JL
                if (event.compare(event::EMPTY_EVENT) != 0
                        && (JL.count(senderId) == 0 || JL[senderId] < seq)) {
                    JL[senderId] = seq;
                }
//                    lambda++;
            }
            applyEvent();
        }
    }
}

void NodeCtrl::handleRoundQuery(RoundQuery* query) {
    UDPControlInfo *queryCtrl = check_and_cast<UDPControlInfo *>(
            query->getControlInfo());
    IPvXAddress& srcAddr = queryCtrl->getSrcAddr();
    int round = query->getRound();
    string sender = query->getSenderName();
    RoundQueryReply* rqr = NULL;
    string senderSeqRanges = query->getSeqRanges();
    // tuple(sender ID, lowerest sequence number, sequene of the round)
    typedef tuple<int, int> tuple_seqRange;
    map<unsigned long, tuple_seqRange> senderInfos;
    util::unpackSenderSeqRanges(senderSeqRanges, senderInfos);

    // tuple (round#, sender ID, sequence number, event content)
    typedef tuple<int, unsigned long, int> tuple_r;
    map<tuple_r, string> Responses;
    for (map<unsigned long, tuple_seqRange>::iterator it = senderInfos.begin();
            it != senderInfos.end(); it++) {
        // retrieve the range of sequence number
        unsigned long senderId = it->first;
        tuple_seqRange seqs = it->second;
        int lowestDeliverable = get<0>(seqs);
        int sor = get<1>(seqs);
        for (int i = lowestDeliverable; i <= sor; i++) {
            int r = round - (sor - i);
            tuple_r eKey(round, senderId, i);
            tuple_qp pKey(r, senderId, i);
            tuple_qd key(round, senderId, i);
            if (Qd.count(key) > 0) {
                Responses[eKey] = Qd[key];
            } else if (Qp.count(pKey) > 0) {
                Responses[eKey] = Qp[pKey];
            } else {
                // round event collection not yet reached
                return;
            }
        }
    }

    // check whether Events contains any missing event
    bool completeRound = true;
    for (map<tuple_r, string>::iterator it = Responses.begin();
            it != Responses.end(); it++) {
        string event = it->second;
        if (event.compare(event::MISS_EVENT) == 0) {
            completeRound = false;
            break;
        }
    }
    if (!completeRound) {
        // Trigger a consensus instance
        if (toPropose.count(round) == 0) {
            toPropose[round] = senderSeqRanges;
            initConsensus();
        }
    } else {
        rqr = new RoundQueryReply(msg::ROUND_QUERY_REPLY);
        string events = util::convertMapToStr(Responses);
        rqr->setEvents(events.c_str());
        rqr->setSenderName(fullName);
        rqr->setRound(round);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        rqr->setControlInfo(udpControlInfo);
        transmit(rqr, srcAddr);
    }

    delete query;
}

void NodeCtrl::handleRoundQueryReply(RoundQueryReply* rqr) {
    int round = rqr->getRound();
    string eventStr = rqr->getEvents();
    typedef tuple<int, unsigned long, int> tuple_e;
    map<tuple_e, string> events;
    util::convertStrToMap(eventStr, events);
    for (map<tuple_e, string>::iterator it = events.begin(); it != events.end();
            it++) {
        tuple_e eKey = it->first;
        string event = it->second;
        tuple_qc content(get<1>(eKey), get<2>(eKey), event);
        Qc.insert(pair<int, tuple_qc>(round, content));
    }

    handleEvent();

    delete rqr;
}

void NodeCtrl::initConsensus() {
    if (mode == EH && coord.compare(ipAddress.get4().str()) == 0) {
        for (map<int, string>::iterator it = toPropose.begin();
                it != toPropose.end(); it++) {
            int r = it->first;
            if (proposed.find(r) == proposed.end()) {
                proposed.insert(r);
                ConsensusQuery* cq = new ConsensusQuery(msg::CONSENSUS_QUERY);
                cq->setSenderName(fullName);
                cq->setEpoch(epoch);
                cq->setGroupId(groupId);
                cq->setRound(r);
                string senderSeqRanges = it->second;
                cq->setSeqRanges(senderSeqRanges.c_str());
                multicast(cq, survivedMemberSet());
            }
        }
    }
}

void NodeCtrl::startPropose(ConsensusQuery * cq) {
    UDPControlInfo *cqCtrl = check_and_cast<UDPControlInfo *>(
            cq->getControlInfo());
    IPvXAddress& srcAddr = cqCtrl->getSrcAddr();
    int r = cq->getRound();
    string senderSeqRanges = cq->getSeqRanges();
    if (mode == EH) {
        string received = roundEToString(r, senderSeqRanges);

        EV << "Create the proposal: " << received << endl;

        if (!received.empty()) {
            if (proposed.count(r) == 0) {
                proposed.insert(r);
            }

            Proposal* proposal = new Proposal(msg::PROPOSAL);
            proposal->setSenderName(fullName);
            proposal->setEpoch(epoch);
            proposal->setGroupId(groupId);
            proposal->setRound(r);
            proposal->setReceived(received.c_str());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            proposal->setControlInfo(udpControlInfo);
            transmit(proposal, srcAddr);
        } else {
            Qu[r] = senderSeqRanges;
        }
    } else {
        // for debug purpose
        EV << "[" << simTime() << "s] " << fullName
                  << " cannot start a new consensus instance" << endl;
        if (mode != EH) {
            EV << "[" << simTime() << "s] " << fullName
                      << " is not in the event handling mode" << endl;
        } else {
            EV << "[" << simTime() << "s] " << fullName
                      << " has not finished the last consensus instance"
                      << endl;
        }
    }

    delete cq;
}

string NodeCtrl::roundEToString(int round, string senderSeqRanges) {
    string roundEvents = "";

    // tuple(sender ID, lowerest sequence number, sequene of the round)
    typedef tuple<int, int> tuple_seqRange;
    map<unsigned long, tuple_seqRange> senderInfos;
    util::unpackSenderSeqRanges(senderSeqRanges, senderInfos);

    // tuple (round#, sender ID, sequence number, event content)
    typedef tuple<int, unsigned long, int> tuple_r;
    map<tuple_r, string> Responses;
    for (map<unsigned long, tuple_seqRange>::iterator it = senderInfos.begin();
            it != senderInfos.end(); it++) {
        // retrieve the range of sequence number
        unsigned long senderId = it->first;
        tuple_seqRange seqs = it->second;
        int lowestDeliverable = get<0>(seqs);
        int sor = get<1>(seqs);
        for (int i = lowestDeliverable; i <= sor; i++) {
            int r = round - (sor - i);
            tuple_r eKey(round, senderId, i);
            tuple_qp pKey(r, senderId, i);
            tuple_qd key(round, senderId, i);
            if (Qd.count(key) > 0) {
                Responses[eKey] = Qd[key];
            } else if (Qp.count(pKey) > 0) {
                Responses[eKey] = Qp[pKey];
            }
        }
    }

    if (!Responses.empty()) {
        roundEvents = util::convertMapToStr(Responses);
    } else {
        roundEvents.clear();
    }

    return roundEvents;
}

void NodeCtrl::collectProposal(Proposal* proposal) {
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
    int r = proposal->getRound();

    if (e == epoch && gid == groupId && mode == EH) {
        typedef tuple<int, unsigned long, int> tuple_e;
        map<tuple_e, string> events;
        if (!received.empty()) {
            util::convertStrToMap(received, events);
        }
        map<tuple_ppKey, string> eTuples;
        for (map<tuple_e, string>::iterator it = events.begin();
                it != events.end(); it++) {
            tuple_e eKey = it->first;
            string event = it->second;
            tuple_ppKey ppKey(get<1>(eKey), get<2>(eKey));
            eTuples[ppKey] = event;
        }
        tuple_pKey pKey(r, senderAddr);
        proposals[pKey] = eTuples;
    }

    checkProposals(r);

    delete proposal;
}

void NodeCtrl::checkProposals(int r) {
    if (mode == EH) {
        bool allReceived = true;
        set<string> memberList = survivedMemberSet();
        // check whether all replies from  hosts on the Survivals list have been received
        for (set<string>::iterator it = memberList.begin();
                it != memberList.end(); it++) {
            string host = *it;
            tuple_pKey pKey(r, host);
            if (proposals.count(pKey) == 0) {
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

            map<tuple<int, unsigned long, int> , string> dec = decide(r,
                    memberList);

            // create DECISION message
            Decision* decisionMsg = new Decision(msg::DECISION);
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
            multicast(decisionMsg, memberList);

            // empty proposals
            for (set<string>::iterator it = memberList.begin();
                    it != memberList.end(); it++) {
                string host = *it;
                tuple_pKey pKey(r, host);
                proposals.erase(pKey);
            }
        }
    }
}

map<tuple<int, unsigned long, int> , string> NodeCtrl::decide(int round,
        set<string> memberList) {
    // tuple(sender ID, lowerest sequence number, sequene of the round)
    typedef tuple<int, int> tuple_seqRange;
    map<unsigned long, tuple_seqRange> senderInfos;
    util::unpackSenderSeqRanges(toPropose[round], senderInfos);

    // tuple (round#, sender ID, sequence number, event content)
    map<tuple<int, unsigned long, int> , string> Decisions;
    for (map<unsigned long, tuple_seqRange>::iterator it = senderInfos.begin();
            it != senderInfos.end(); it++) {
        // retrieve the range of sequence number
        unsigned long senderId = it->first;
        tuple_seqRange seqs = it->second;
        int lowestDeliverable = get<0>(seqs);
        int sor = get<1>(seqs);
        for (int j = lowestDeliverable; j <= sor; j++) {
//            int r = rnd - (sor - j);
            for (set<string>::iterator it = memberList.begin();
                    it != memberList.end(); it++) {
                string host = *it;
                tuple_pKey pKey(round, host);
                map<tuple_ppKey, string> events = proposals[pKey];
                tuple_ppKey ppKey(senderId, j);
                string event = events[ppKey];
                tuple<int, unsigned long, int> dKey(round, senderId, j);

//                EV << "[Debug] host = " << hostName << " senderId = "
//                        << senderId << " seq = " << j << endl;

                if (Decisions.count(dKey) == 0
                        || Decisions[dKey].compare(event::EMPTY_EVENT) == 0) {
                    if (event.compare(event::MISS_EVENT) != 0) {
                        Decisions[dKey] = event;

//                        EV << "[Debug] non-missing event = " << event << endl;

                    } else {
                        Decisions[dKey] = event::EMPTY_EVENT;
//                        EV << "[Debug] missing event = " << event << endl;
                    }
                } else {
//                    EV << "[Debug] event already decided" << endl;
                }
            }

        }
    }

    return Decisions;
}

void NodeCtrl::handleDecision(Decision* decision) {
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
    if (e == epoch && gid == groupId && mode == EH) {

        toPropose.erase(r);
        proposed.erase(r);

        if (!dec.empty()) {
            // tuple (round#, sender Id, sequence#)
            typedef tuple<int, unsigned long, int> tuple_e;
            map<tuple_e, string> roundEvents;
            util::convertStrToMap(dec, roundEvents);
            for (map<tuple_e, string>::iterator it = roundEvents.begin();
                    it != roundEvents.end(); it++) {
                tuple_e eKey = it->first;
                string event = it->second;
                tuple_qc content(get<1>(eKey), get<2>(eKey), event);
                Qc.insert(pair<int, tuple_qc>(get<0>(eKey), content));
            }

            handleEvent();
        }

        consensus++;
    }

    delete decision;
}

void NodeCtrl::applyEvent() {
    // serialize events in Qd, deliver non-empty events, and remove empty events
    map<tuple_qd, string>::iterator it = Qd.begin();
    if (get<0>(lastDelivered) != -1) {
        tuple_qd key(get<0>(lastDelivered), get<1>(lastDelivered),
                get<2>(lastDelivered));
        it = Qd.find(key);
        it++;
    }
    for (; it != Qd.end(); it++) {
        tuple_qd key = it->first;
        string event = it->second;
        if (event.compare(event::EMPTY_EVENT) != 0) {
            EventApply* apply = new EventApply(msg::EVENT_APPLY);
            apply->setEvent(event.c_str());
            send(apply, this->gateHalf("uplink", cGate::OUTPUT));
        }
        lastDelivered = tuple<int, unsigned long, int>(get<0>(key), get<1>(key),
                get<2>(key));
    }
}

void NodeCtrl::applyUpdate(UpdateApply* updateApply) {
    string event = updateApply->getContent();
    // assign update to the message sending buffer (Br)
    Bs[sequence] = event;
    // assign update to the queues in Qsend
    for (map<unsigned long, int>::iterator it3 = Nr.begin(); it3 != Nr.end();
            it3++) {
        unsigned long recipientId = it3->first;
        tuple_qsend qsendValue(sequence, event);
        Qsend[recipientId].push_back(qsendValue);
    }
    sequence++;
    delete updateApply;
}

void NodeCtrl::disseminateEvent() {
    // disseminate updates to interested secondary copies
    for (map<unsigned long, int>::iterator it = Nr.begin(); it != Nr.end();
            it++) {
        unsigned long recipientId = it->first;
        if (it->second > round) {
            continue;
        }

        Update* update = new Update(msg::UPDATE);
        vector<tuple_qsend>& qsendValues = Qsend[recipientId];
        if (!qsendValues.empty()) {
            tuple_qsend value = qsendValues[0];
            string content = get<1>(value);
            update->setContent(content.c_str());
            update->setIndex(get<0>(value));
            qsendValues.erase(qsendValues.begin());
        } else {
            update->setContent(event::EMPTY_EVENT);
        }
        update->setSenderId(gId);
        update->setSourceName(fullName);
        update->setSeq(round);
        map<string, string> addresses = Rg[recipientId];
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
//            sendMsg(update->dup());
            sendReliable(updateDup);
        }
        delete update;
    }

    // prepare for the next cycle
    scheduleAt(readClock() + roundCycle, updateTimeout);
}

void NodeCtrl::gossip() {
    Gossip* gossip = new Gossip(msg::GOSSIP_GC);
    gossip->setSourceName(fullName);
    gossip->setSendTime(readClock());
    gossip->setLastRound(deliveryRound);
    // broadcast DECISION message
    multicast(gossip, survivedMemberSet());

    // reset gossip timer
    scheduleAt(readClock() + gcCycle, GCTimeout);
}

void NodeCtrl::handleGossip(Gossip* gossip) {
    UDPControlInfo *gossipCtrl = check_and_cast<UDPControlInfo *>(
            gossip->getControlInfo());
    IPvXAddress& srcAddr = gossipCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    simtime_t ts = gossip->getSendTime();
    int lastRound = gossip->getLastRound();
    if (histories.count(sender) == 0 || ts > get<1>(histories[sender])) {
        histories[sender] = tuple_h(lastRound, ts);
    }

    set<string> survivedMembers = survivedMemberSet();
    if (includes(histories | map_keys, survivedMembers)) {
        vector<int> lastRounds;
        for (map<string, tuple_h>::iterator it = histories.begin();
                it != histories.end(); it++) {
            lastRounds.push_back(get<0>(it->second));
        }
        int lastCommRound = *min_element(lastRounds.begin(), lastRounds.end());
        vector<tuple_qp> purge_qp;
        vector<tuple_qd> purge_qd;
        vector<int> purge_qc;
        vector<int> purge_qa;
        int lastDeliveredRound = get<0>(lastDelivered);
        int lastRemove =
                lastCommRound <= lastDeliveredRound ?
                        lastCommRound : lastDeliveredRound;
        for (map<tuple_qd, string>::iterator it = Qd.begin(); it != Qd.end();
                it++) {
            tuple_qd key = it->first;
            int r = get<0>(key);
            if (r <= lastRemove) {
                purge_qd.push_back(key);
            }
        }
        for (map<tuple_qp, string>::iterator it = Qp.begin(); it != Qp.end();
                it++) {
            tuple_qp key = it->first;
            int r = get<0>(key);
            if (r <= lastCommRound) {
                purge_qp.push_back(key);
            }
        }
        for (map<int, tuple_qc>::iterator it = Qc.begin(); it != Qc.end();
                it++) {
            int r = it->first;
            if (r <= lastCommRound) {
                purge_qc.push_back(r);
            }
        }
        for (size_t i = 0; i < purge_qd.size(); i++) {
            Qd.erase(purge_qd[i]);
        }
        for (size_t i = 0; i < purge_qp.size(); i++) {
            Qp.erase(purge_qp[i]);
        }
        for (size_t i = 0; i < purge_qc.size(); i++) {
            Qc.erase(purge_qc[i]);
        }
    }

    delete gossip;
}

void NodeCtrl::stageTransfer() {
    if (isMember) {
        // check whether coordinator change needs to be triggered
        bool triggerCood = checkCoord();
        string addrStr = ipAddress.get4().str();
        if (!triggerCood) {
            if (mode == CC) {
                if (coordCand.compare(addrStr) == 0) {
                    checkLastStates();
                }
            } else {
                // check whether group reconfiguration needs to be triggered
                bool triggerReconfig = checkConfig();
                if (!triggerReconfig) {
                    if (mode == GR) {
                        if (coord.compare(addrStr) == 0) {
                            checkReadies();
                        }
                    } else {
                        if (coord.compare(addrStr) == 0) {
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
}

bool NodeCtrl::checkCoord() {
    string addrStr = ipAddress.get4().str();
    // check whether coordinator is survived
    if (seniorities[addrStr] > 0 && coordCand.compare(addrStr) != 0
            && ((coord.compare(addrStr) != 0)
                    && survivals.find(coord) == survivals.end())) {

        if (ev.isGUI()) {
            string msg = "coordinator " + coord + " has failed";
            bubble(msg.c_str());
        }
        EV << "[" << simTime() << "s] " << fullName << " found coordinator "
                  << coord << " has failed" << endl;

        // if not, go to coordinator change mode
        mode = CC;
        coord.clear();

        // compute the coordinator candidate
        string candidate = coordinator();
        string addrStr = ipAddress.get4().str();
        if (candidate.compare(addrStr) == 0) {
            if (ev.isGUI()) {
                string msg = string(fullName) + " became coordinator candidate";
                bubble(msg.c_str());
            }

            EV << "[" << simTime() << "s] " << fullName
                      << " requested to become the new coordinator" << endl;

            // if self is the coordinator candidate
            coordCand = addrStr;
            lastStates.clear();
            neighborsTemp.clear();
            proposals.clear();
            proposed.clear();
//            agreements.clear();

            // send coord_query to other members
            CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
            coordChange->setCandidate(addrStr.c_str());
            multicast(coordChange, survivedMemberSet());
        }
        return true;
    } else {
        return false;
    }
}

bool NodeCtrl::checkConfig() {
    bool triggerGR = false;
//    for (map<string, int>::iterator it = survivals.begin();
//            it != survivals.end(); it++) {
//        string hostName = it->first;
//        if (neighbors.find(hostName) == neighbors.end()
//                && neighborsTemp.find(hostName) == neighborsTemp.end()) {
//            triggerGR = true;
//            break;
//        }
//    }
    if (triggerGR) {
//        // TODO need to verify the location of G' assignment
//        neighborsTemp.clear();
//        neighborsTemp.insert(survivals.begin(), survivals.end());
//
//        // coordinator initiate
//        if (coord.compare(fullName) == 0) {
//            if (ev.isGUI()) {
//                string msg = "Start a GR";
//                bubble(msg.c_str());
//            }
//            EV << "[" << simTime() << "s] " << fullName
//                      << " start a reconfiguration" << endl;
//
//            mode = GR;
//
//            groupId++;
//            readies.clear();
//            proposals.clear();
//            proposed.clear();
////            agreements.clear();
//
//            // send G' to all hosts on the Survivals list
//            GRQuery* reconfig = new GRQuery(msg::GROUP_RECONFIG);
//            reconfig->setSenderName(fullName);
//            reconfig->setEpoch(epoch);
//            reconfig->setGroupId(groupId);
//            cMessage* msg = broadcast(reconfig, neighborsTemp);
//
//            // fake the send-to-self process
//            reconfig = dynamic_cast<GRQuery*>(msg);
//            Ready ready;
//            // load member list
//            vector<string> members = util::loadMapKey(neighbors, 1, fullName);
//            ready.groupMembers = members;
//            ready.epoch = reconfig->getEpoch();
//            ready.groupId = reconfig->getGroupId();
//            ready.escId = escId;
//            ready.esc = esc;
////            ready.DL.insert(Qd.begin(), Qd.end());
////            ready.DL.insert(decisions.begin(), decisions.end());
//            readies[fullName] = ready;
//            checkReadies();
//
//            delete reconfig;
//        }
        return true;
    } else {
        return false;
    }
}

void NodeCtrl::handleCCQuery(CCQuery* cc) {
    string candidateAddr = cc->getCandidate();
    UDPControlInfo *ccCtrl = check_and_cast<UDPControlInfo *>(
            cc->getControlInfo());
    IPvXAddress& srcAddr = ccCtrl->getSrcAddr();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change from " << candidateAddr << endl;

    if (survivals.count(candidateAddr) > 0) {

        string candidate = coordinator();
        if (candidateAddr.compare(candidate) == 0) {

            mode = CC;
            coord.clear();

            CCQueryReply* ccr = new CCQueryReply(msg::COORD_CHANGE_REPLY);
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
            string members = util::convertSetToStr(neighbors, 0);
            ccr->setGroupMembers(members.c_str());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            ccr->setControlInfo(udpControlInfo);
            transmit(ccr, srcAddr);
        } else {
            CCNAck* ccr = new CCNAck(msg::CC_NACK);
            ccr->setSenderName(fullName);
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            ccr->setControlInfo(udpControlInfo);
            transmit(ccr, srcAddr);
        }
    } else if (!isMember) {
        CCNAck* ccr = new CCNAck(msg::CC_NACK);
        ccr->setSenderName(fullName);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        ccr->setControlInfo(udpControlInfo);
        transmit(ccr, srcAddr);
    }

    delete cc;
}

void NodeCtrl::handleGRQuery(GRQuery* gr) {
//
//    string senderName = gr->getSenderName();
//    int e = gr->getEpoch();
//    int gid = gr->getGroupId();
//
//    EV << "[" << simTime() << "s] " << fullName
//              << " received reconfiguration query from " << senderName << endl;
//
//    // message validation
//    if (e >= epoch && gid > groupId && mode != CC) {
//
//        mode = GR;
//
//        if (ev.isGUI()) {
//            string msg = "GR is triggered";
//            bubble(msg.c_str());
//        }
//        EV << "[" << simTime() << "s] " << fullName
//                  << " entered the reconfiguration mode" << endl;
//
//        groupId = gr->getGroupId();
//
//        // update coordinator info for new members
//        if (epoch == 0) {
//            epoch = e;
//            coord = senderName;
//            coordGate = survivals[coord];
//        }
//
//        GRQueryReply* grr = new GRQueryReply(msg::RECONFIG_READY);
//        grr->setSenderName(fullName);
//        // concatenate neighbor list to string content
//        string members = util::convertMapKeyToStr(neighbors, 1, fullName);
//        grr->setGroupMembers(members.c_str());
//        grr->setEpoch(epoch);
//        grr->setGroupId(groupId);
//        grr->setEscId(escId);
//        grr->setEsc(esc);
//        // send events for state sync
////        grr->setState(buildStateForSync().c_str());
//        transmit(grr, senderName, survivals[senderName]);
//    }
//
//    delete gr;
}

void NodeCtrl::handleCCNAck(CCNAck* ccnack) {
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

        // send coord_query to the reply host
        if (survivals.count(sender) > 0) {
            CCQuery* coordChange = new CCQuery(msg::COORD_CHANGE);
            coordChange->setCandidate(addrStr.c_str());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(srcAddr);
            udpControlInfo->setSrcAddr(ipAddress);
            coordChange->setControlInfo(udpControlInfo);
            transmit(coordChange, srcAddr);
        }
    } else {
        coordCand.clear();
    }

    delete ccnack;
}

void NodeCtrl::handleCCQueryReply(CCQueryReply* ccr) {
    UDPControlInfo *ccrCtrl = check_and_cast<UDPControlInfo *>(
            ccr->getControlInfo());
    IPvXAddress& srcAddr = ccrCtrl->getSrcAddr();
    string sender = srcAddr.get4().str();
    string senderName = ccr->getSenderName();

    EV << "[" << simTime() << "s] " << fullName
              << " received coordinator change reply from " << senderName
              << endl;

    // only handle messages from survived members, TODO but this condition may not needed
    set<string> survivedMembers = survivedMemberSet();
    if (survivedMembers.count(sender) > 0) {
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

void NodeCtrl::checkLastStates() {

    bool allReceived = true;

    set<string> survivedMembers = survivedMemberSet();
    // check whether all replies from  hosts on the Survivals list have been received
    for (set<string>::iterator it = survivedMembers.begin();
            it != survivedMembers.end(); ++it) {
        string hostName = *it;
        if (lastStates.find(hostName) == lastStates.end()) {
            allReceived = false;
            break;
        }
    }

    if (allReceived) {
        if (ev.isGUI()) {
            string msg = "Received all LAST_STATE";
            bubble(msg.c_str());
            EV << "[" << simTime() << "s] " << fullName << msg << endl;
        }

        LastState ls = computeLastState();
        CCEnd* cce = new CCEnd(msg::LOAD_COORD);
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
        multicast(cce, survivedMembers);
    }
}

void NodeCtrl::handleGRQueryReply(GRQueryReply* grr) {
//    string sender = grr->getSenderName();
//    int e = grr->getEpoch();
//    int gid = grr->getGroupId();
//
//    EV << "[" << simTime() << "s] " << fullName
//              << " received group reconfiguration reply from " << sender
//              << endl;
//
//    // only handle messages from survived hosts in neighborsTemp, TODO but this condition may not needed
//    if (e == epoch && gid == groupId && mode != CC
//            && neighborsTemp.find(sender) != neighborsTemp.end()
//            && survivals.find(sender) != survivals.end()) {
//        Ready ready;
//        ready.epoch = grr->getEpoch();
//        ready.groupId = grr->getGroupId();
//        ready.escId = grr->getEscId();
//        ready.esc = grr->getEsc();
////        string state = grr->getState();
////        util::convertStrToMap(state, ready.DL);
//
//        // TODO sync Qs is still needed
//
//        readies[sender] = ready;
//    }
//
//    checkReadies();
//
//    delete grr;
}

void NodeCtrl::handleCCEnd(CCEnd* cce) {
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

            if (!isMember) {
                // load synchronized Qd and Qp
                Qd.clear();
                string QdStr = cce->getQd();
                util::convertStrToMap(QdStr, Qd);
                Qp.clear();
                string QpStr = cce->getQp();
                util::convertStrToMap(QpStr, Qp);
                escId = cce->getEscId();
                esc = cce->getEsc();
                string senderStr = cce->getSenders();
                util::convertStrToMap(senderStr, senders);
                // compute the sor for each client;
                double current = readClock().dbl();
                double cycle = roundCycle.dbl();
                double start = cycleStart.dbl();
                round = floor((current - start) / cycle);
                for (map<unsigned long, int>::iterator it = senders.begin();
                        it != senders.end(); it++) {
//                    sors[it->first] = it->second + round;
                    // TODO calculation of sor needs to be added later
//                    sors[it->first] = 0 + round;
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
                if (survivals.count(hostName) > 0) {
                    neighbors.insert(hostName);
                }
            }

            coordCand.clear();
            mode = EH;

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

void NodeCtrl::handleGREnd(GREnd* gre) {
//    string senderName = gre->getSenderName();
//    int e = gre->getEpoch();
//    int gid = gre->getGroupId();
////    if (senderName.compare(coord) == 0 && e == epoch && gid == groupId && mode != CC) {
//    if (e == epoch && gid == groupId && mode != CC) {
//        if (!isMember) {
//            // load synchronized DL
//            Qd.clear();
//            string QdStr = gre->getQd();
//            util::convertStrToMap(QdStr, Qd);
////            decodeQd();
//            Qp.clear();
//            string QpStr = gre->getQp();
//            util::convertStrToMap(QpStr, Qp);
////            decodeQp();
//            escId = gre->getEscId();
//            esc = gre->getEsc();
//            string senderStr = gre->getSenders();
//            util::convertStrToMap(senderStr, senders);
//            // compute the sor for each client;
//            double current = simTime().dbl();
//            double cycle = roundCycle.dbl();
//            double start = cycleStart.dbl();
//            round = floor((current - start) / cycle);
//            for (map<unsigned long, int>::iterator it = senders.begin();
//                    it != senders.end(); it++) {
////                    sors[it->first] = it->second + round;
//                // TODO calculation of sor needs to be added later
//                sors[it->first] = 0 + round;
//            }
//            isMember = true;
//        }
//
//        // split string-style neighbors to list of neighbors
//        string members = gre->getGroupMembers();
//        vector<string> memberList;
//        util::splitString(members, ";", memberList);
//        // load new group member list
//        neighbors.clear();
//        seniorities.clear();
//        for (size_t i = 0; i < memberList.size(); i++) {
//            string value = memberList[i];
//            vector<string> pair;
//            util::splitString(value, ":", pair);
//            string hostName = pair[0];
//            int seniority = util::strToInt(pair[1]);
//            if (hostName.compare(fullName) != 0) {
//                if (survivals.find(hostName) != survivals.end()) {
//                    neighbors[hostName] = survivals[hostName];
//                    // update seniority
//                    seniorities[hostName] = seniority + 1;
//                }
//            } else {
//                // update seniority
//                seniorities[hostName] = seniority + 1;
//            }
//        }
//        // resume event handling
//        neighborsTemp.clear();
//        mode = EH;
//
//        if (ev.isGUI()) {
//            string msg = string(fullName) + " load the new configuration";
//            bubble(msg.c_str());
//        }
//        EV << "[" << simTime() << "s] " << fullName
//                  << " loaded the new configuration" << endl;
//    } else {
//        EV << "[" << simTime() << "s] reconfiguration from " << fullName
//                  << " is invalid" << endl;
//    }
//
//    delete gre;
}

void NodeCtrl::checkReadies() {
//    if (mode != CC) {
//        bool allReceived = true;
//        map<string, int> survivedNeighborsTemp = util::intersectMaps(survivals,
//                neighborsTemp);
//        survivedNeighborsTemp[fullName] = -1;
//        // check whether all replies from  hosts on the Survivals list have been received
//        for (map<string, int>::iterator it = survivedNeighborsTemp.begin();
//                it != survivedNeighborsTemp.end(); ++it) {
//            string hostName = it->first;
//            if (readies.find(hostName) == readies.end()) {
//                allReceived = false;
//                break;
//            }
//        }
//
//        if (allReceived) {
//
//            if (ev.isGUI()) {
//                string msg = "Received all LAST_STATE";
//                bubble(msg.c_str());
//            }
//
//            Ready ready = computeReady();
//            GREnd* gre = new GREnd(msg::LOAD_CONFIG);
//            gre->setSenderName(fullName);
//            gre->setEpoch(epoch);
//            gre->setGroupId(groupId);
//            // concatenate Qd, Qp to string content
//            string QdStr = util::convertMapToStr(ready.Qd);
//            string QpStr = util::convertMapToStr(ready.Qp);
//            gre->setQd(QdStr.c_str());
//            gre->setQp(QpStr.c_str());
//            gre->setSenders(util::convertMapToStr(senders).c_str());
//            gre->setEscId(ready.escId);
//            gre->setEsc(ready.esc);
//            // concatenate neighbor list to string content
//            string members = util::convertListToStr(ready.groupMembers, 0);
//            gre->setGroupMembers(members.c_str());
//            cMessage* msg = broadcast(gre, neighborsTemp);
//
//            // fake the send-to-self process,  switch back to the event handling mode
//            gre = check_and_cast<GREnd*>(msg);
//            handleGREnd(gre);
//        }
//    }
//
}

string NodeCtrl::coordinator() {
    string coordAddr = ipAddress.get4().str().c_str();
    unsigned long coordId = sdbm::encode(coordAddr.c_str());
    for (set<string>::iterator it = neighbors.begin(); it != neighbors.end();
            it++) {
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

LastState NodeCtrl::computeLastState() {
    // compute the sync state: use the Qd of Coordinator and merge Qp from all nodes in the group
    LastState ls;
    ls.Qd = Qd;
    ls.Qp = Qp;

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
    }
    return ls;
}

Ready NodeCtrl::computeReady() {
    Ready ready;
//    ready.Qd = Qd;
//    ready.Qp = Qp;
//
//    // retrieve new group members from G'
//    for (map<string, int>::iterator it = neighborsTemp.begin();
//            it != neighborsTemp.end(); it++) {
//        string pair = it->first + ":" + util::intToStr(seniorities[it->first]);
//        ready.groupMembers.push_back(pair);
//    }
//    string selfPair = string(fullName) + ":"
//            + util::intToStr(seniorities[fullName]);
//    ready.groupMembers.push_back(selfPair);
//
    return ready;
}

//void NodeCtrl::broadcast(cMessage* message, set<string> hosts) {
//    for (set<string>::iterator it = hosts.begin(); it != hosts.end(); it++) {
//        cMessage* msgDup = message->dup();
//        IPvXAddress dest = getIPAddress(*it);
//        UDPControlInfo* udpControlInfo = new UDPControlInfo();
//        udpControlInfo->setDestAddr(dest);
//        udpControlInfo->setSrcAddr(ipAddress);
//        msgDup->setControlInfo(udpControlInfo);
//        transmit(msgDup, dest);
//    }
//    delete message;
//}

//void NodeCtrl::sendMsg(cMessage* msg) {
//    send(msg, this->gateHalf("udplink", cGate::OUTPUT));
//}
//
//void NodeCtrl::sendReliable(cMessage* msg) {
//    send(msg, this->gateHalf("tcplink", cGate::OUTPUT));
//}

set<string> NodeCtrl::survivedMemberSet() {
    set<string> survivedMembers = util::intersectSets(survivals, neighbors);
    return survivedMembers;
}

vector<string> NodeCtrl::survivedMembers() {
    vector<string> memberList;
    set<string> survivedMembers = survivedMemberSet();
    for (set<string>::iterator it = survivedMembers.begin();
            it != survivedMembers.end(); it++) {
        memberList.push_back(*it);
    }
    return memberList;
}

IPvXAddress NodeCtrl::loadIPAddress() {
    InterfaceTable* ift = InterfaceTableAccess().get(getParentModule());
    string IPInterface = getParentModule()->par("IPInterface");
    InterfaceEntry* ie = ift->getInterfaceByName(IPInterface.c_str());
    IPv4InterfaceData* ipData = ie->ipv4Data();
    IPvXAddress addr(ipData->getIPAddress());
    return addr;
}

void NodeCtrl::syncClock() {
    Clock* clock = check_and_cast<Clock*>(
            getParentModule()->getSubmodule("clock"));
    clockError = clock->getError();
}
