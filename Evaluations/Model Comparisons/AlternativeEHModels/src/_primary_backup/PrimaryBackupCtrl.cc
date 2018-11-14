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

#include "PrimaryBackupCtrl.h"

Define_Module(PrimaryBackupCtrl);

PrimaryBackupCtrl::PrimaryBackupCtrl() {
    // to make sure the first round is round 0
    STAGE_NUM = stage::NODE_CTRL_INIT;
    hbTimeout = new HBTimeout(msg::HEARTBEAT_TIMEOUT);
}

PrimaryBackupCtrl::~PrimaryBackupCtrl() {
    if (hbTimeout != NULL) {
        cancelAndDelete(hbTimeout);
    }
}

int PrimaryBackupCtrl::numInitStages() const {
    return STAGE_NUM;
}

void PrimaryBackupCtrl::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();
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
    }
}

void PrimaryBackupCtrl::dispatchHandler(cMessage *msg) {
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
        // everything will only be initialized if a reconfiguration is involved in the leader election
        if (reconfig) {
            // initialize application state
            PrimaryBackupApp* app =
                    dynamic_cast<PrimaryBackupApp*>(this->getParentModule()->getSubmodule(
                            "app"));
            app->initState(state.second);
            // continue the event delivery
            applyEvent();
        }
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
        PrimaryBackupApp* app =
                dynamic_cast<PrimaryBackupApp*>(this->getParentModule()->getSubmodule(
                        "app"));
        app->initState(state.second);
        // continue the event delivery
        applyEvent();
    } else if (msg->isName(msg::EVENT)) {
        // run on primary
        Event* event = check_and_cast<Event*>(msg);
        receiveEvent(event);
    } else if (msg->isName(msg::INIT_JOIN)) {
        InitJoin* initJoin = check_and_cast<InitJoin*>(msg);
        handleJoin(initJoin);
    } else if (msg->isName(msg::UPDATE_APPLY)) {
        UpdateApply* updateApply = check_and_cast<UpdateApply*>(msg);
        applyUpdate(updateApply);
    } else if (msg->isName(msg::HEARTBEAT_TIMEOUT)) {
        HBTimeout* hbt = check_and_cast<HBTimeout*>(msg);
        handleHeartbeatTimeout(hbt);
    } else if (msg->isName(msg::UPDATE)) {
        PrimaryBackupUpdate* update = check_and_cast<PrimaryBackupUpdate*>(msg);
        receiveUpdate(update);
    }
}

void PrimaryBackupCtrl::handleHeartbeat(HBProbe *hb) {
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

void PrimaryBackupCtrl::handleHeartbeatTimeout(HBTimeout* hbt) {
    UnderlayConfiguratorAccess().get()->revokeNode(ipAddress);
}

void PrimaryBackupCtrl::initNode(simtime_t starttime) {
    // group ID starts from 0
    groupId = gid_temp = 0;
    members.insert(survivals.begin(), survivals.end());
    for (set<string>::iterator it = members.begin(); it != members.end();
            it++) {
        seniorities[*it] = 1;
    }
}

void PrimaryBackupCtrl::handleJoin(InitJoin* initJoin) {
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

void PrimaryBackupCtrl::receiveEvent(Event* event) {
    unsigned long senderId = event->getSenderId();
    int seq = event->getSeq();
    string content = event->getContent();
    tuple_qr t(senderId, seq);
    tuple_qd key(0, senderId, seq);
    Qd[key] = content;
    if (mode == EH) {
        applyEvent();
    }

    delete event;
}

void PrimaryBackupCtrl::applyEvent() {
    // only leader delivers events
    if (coord.compare(ipAddress.get4().str()) == 0) {
        // serialize events in Qd, deliver non-empty events, and remove empty events
        for (map<tuple_qd, string>::iterator it = Qd.begin(); it != Qd.end();
                it++) {
            tuple_qd key = it->first;
            string event = it->second;
            // exclude missing/empty events
            if (event.compare(event::EMPTY_EVENT) != 0) {
                PrimaryBackupEventApply* apply = new PrimaryBackupEventApply(
                        msg::EVENT_APPLY);
                apply->setEvent(event.c_str());
                apply->setRound(0);
                apply->setClientId(get<1>(key));
                apply->setSequence(get<2>(key));
                apply->setIsLeader(true);
                send(apply, this->gateHalf("uplink", cGate::OUTPUT));
            }
        }
    }
    Qd.clear();
}

void PrimaryBackupCtrl::applyUpdate(UpdateApply* updateApply) {
    PrimaryBackupUpdateApply* stateApply =
            dynamic_cast<PrimaryBackupUpdateApply*>(updateApply);
    string event = stateApply->getEvent();
    if (stateApply->getStateIndex() > state.first) {
        state.first = stateApply->getStateIndex();
        state.second = stateApply->getState();
    }

    // broadcast handled events to recipients
    for (auto elem : Rg) {
        unsigned long recipientId = elem.first;

        Update* update = new Update(msg::UPDATE);
        update->setContent(event.c_str());
        update->setLCName(LCName.c_str());
        update->setSenderId(stateApply->getClientId());
        update->setSequence(stateApply->getSequence());
        update->setSourceName(fullName);
        // message size: content size + 1 byte for sender ID + byte for sequence number + 1 byte for logical computer identity
        update->setByteLength(event.size() + 1 + 1 + 1);
        map<string, string> addresses = elem.second;
        for (map<string, string>::iterator it = addresses.begin();
                it != addresses.end(); it++) {
            Update* updateDup = update->dup();
            IPvXAddress dest(it->first.c_str());
            string hostName = it->second;
            updateDup->setDestName(hostName.c_str());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(dest);
            udpControlInfo->setSrcAddr(ipAddress);
            updateDup->setControlInfo(udpControlInfo);
            sendReliable(updateDup);
        }
        delete update;
    }

    // broadcast handled events to neighbors
    set<string> memberlist = survivedMembers();
    PrimaryBackupUpdate* update = new PrimaryBackupUpdate(msg::UPDATE);
    update->setStateIndex(stateApply->getStateIndex());
    update->setState(stateApply->getState());
    update->setSourceName(fullName);
    // message size: 4 bytes for state size + 1 byte for state index
    update->setByteLength(4 + 1 + 1);
    r_multicast(update, memberlist);

    delete stateApply;
}

void PrimaryBackupCtrl::receiveUpdate(PrimaryBackupUpdate* update) {
    // exclude self-message on leader
    if (coord.compare(ipAddress.get4().str()) != 0) {
        StateApply* apply = new StateApply(msg::STATE_APPLY);
        apply->setStateIndex(update->getStateIndex());
        apply->setState(update->getState());
        send(apply, this->gateHalf("uplink", cGate::OUTPUT));

        // update last state indicator
        state.first = update->getStateIndex();
        state.second = update->getState();
    }
    delete update;
}

void PrimaryBackupCtrl::stateTransfer() {
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
                    }
                }
            }
        }
    }
}

bool PrimaryBackupCtrl::checkCoord() {
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
//            proposals.clear();
//            proposed.clear();

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

string PrimaryBackupCtrl::coordinator() {
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

void PrimaryBackupCtrl::handleCCQuery(CCQuery* cc) {
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

string PrimaryBackupCtrl::buildCCSnapshot() {
    json snapshot;
    string Qdstr = util::convertMapToStr(Qd);
    snapshot["Qd"] = Qdstr;
    snapshot["epoch"] = epoch;
    snapshot["GID"] = groupId;
    string memberlist = util::convertSetToStr(members, 0);
    snapshot["G"] = memberlist;

    json lastState;
    lastState["key"]["index"] = state.first;
    lastState["state"] = state.second;
    snapshot["last_state"] = lastState;
    return snapshot.dump();
}

void PrimaryBackupCtrl::handleCCNAck(CCNAck* ccnack) {
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

void PrimaryBackupCtrl::handleCCQueryReply(CCQueryReply* ccr) {
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

void PrimaryBackupCtrl::checkCCSnapshos() {

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
        if (reconfig) {
            cce->setGroupId(snapshot["GID"]);
            cce->setGroupMembers(snapshot["memberlist"].get<string>().c_str());
            cce->setInit(snapshot["init"].dump().c_str());
        }
        // message size: 1 byte for epoch + snapshot size
        cce->setByteLength(1 + snapshot.dump().size());
        r_multicast(cce, survived_members);
    }
}

json PrimaryBackupCtrl::computeCCSnapshot() {
    json snapshot;

    bool reconfig = false;
    int highest_epoch = epoch;
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
    }
    snapshot["epoch"] = highest_epoch;
    snapshot["reconfig"] = reconfig;

    if (reconfig) {
        json init;
        int highest_gid = groupId;
        string latest_members;
        int lastRound = -1;
        string longest_Qd;
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
        json localState = json::parse(CCSnapshots[ipAddress.get4().str()]);
        init["initial_state"] = localState["last_state"];

        // prepare initialization state for new replicas
        string senderAddrList = util::convertMapToStr(clients);
        string RgList = util::convertMapToStr(Rg);
        init["senderAddrs"] = senderAddrList;
        init["Rg"] = RgList;

        snapshot["init"] = init;
    }

    return snapshot;
}

void PrimaryBackupCtrl::handleCCEnd(CCEnd* cce) {
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

            if (cce->getReconfig()) {
                // state sync for any unfinished reconfiguration
                string init_str = cce->getInit();
                json init = json::parse(init_str);
                if (groupId < 0) {
                    // TODO anything to do here?
                }

                groupId = gid_temp = cce->getGroupId();

                string senderAddrList = init["senderAddrs"];
                util::convertStrToMap(senderAddrList, clients);
                string RgList = init["Rg"];
                util::convertStrToMap(RgList, Rg);
                json initialState = init["initial_state"];
                state.first = initialState["key"]["index"];
                state.second = initialState["state"];

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

void PrimaryBackupCtrl::handleCCAck(CCAck* ack) {
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

void PrimaryBackupCtrl::checkCCAcks() {
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

        // notify configuration change to senders
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

        GlobalStatisticsAccess().get()->increaseLeaderElectionEnd();
    }
}

bool PrimaryBackupCtrl::checkConfig() {
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

void PrimaryBackupCtrl::handleGRQuery(GRQuery* gr) {
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

string PrimaryBackupCtrl::buildGRSnapshot() {
    json snapshot;

    json lastState;
    lastState["key"]["index"] = state.first;
    lastState["state"] = state.second;
    snapshot["last_state"] = lastState;

    return snapshot.dump();
}

void PrimaryBackupCtrl::handleGRNAck(GRNAck* grnack) {
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

void PrimaryBackupCtrl::handleGRQueryReply(GRQueryReply* grr) {
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

void PrimaryBackupCtrl::checkGRSnapshots() {
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
            gre->setInit(snapshot["init"].dump().c_str());
            gre->setGroupMembers(snapshot["memberlist"].get<string>().c_str());
            // message size: 1 byte for epoch + 1 byte for group Id + snapshot size
            gre->setByteLength(1 + 1 + snapshot.dump().size());
            r_multicast(gre, intersection);
        }
    }
}

json PrimaryBackupCtrl::computeGRSnapshot() {
    json snapshot;
    json init;

    // in case of empty Qd, set initial_state to the local application state
    json localState = json::parse(GRSnapshots[ipAddress.get4().str()]);
    init["initial_state"] = localState["last_state"];

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

void PrimaryBackupCtrl::handleGREnd(GREnd* gre) {
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
            // TODO anything to do here???
        }

        groupId = gid_temp;

        string senderAddrList = init["senderAddrs"];
        util::convertStrToMap(senderAddrList, clients);
        string RgList = init["Rg"];
        util::convertStrToMap(RgList, Rg);
        json initialState = init["initial_state"];
        state.first = initialState["key"]["index"];
        state.second = initialState["state"];

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

        EV << "[" << simTime() << "s] " << fullName
                  << " loaded the new configuration" << endl;
    } else {
        EV << "[" << simTime() << "s] reconfiguration from " << fullName
                  << " is invalid" << endl;
    }

    delete gre;
}

set<string> PrimaryBackupCtrl::survivedMembers() {
    set<string> survived_members = util::intersectSets(survivals, members);
    return survived_members;
}

IPvXAddress PrimaryBackupCtrl::loadIPAddress() {
    InterfaceTable* ift = InterfaceTableAccess().get(getParentModule());
    string IPInterface = getParentModule()->par("IPInterface");
    InterfaceEntry* ie = ift->getInterfaceByName(IPInterface.c_str());
    IPv4InterfaceData* ipData = ie->ipv4Data();
    IPvXAddress addr(ipData->getIPAddress());
    return addr;
}

void PrimaryBackupCtrl::syncClock() {
    Clock* clock = check_and_cast<Clock*>(
            getParentModule()->getSubmodule("clock"));
    clockError = clock->getError();
}
