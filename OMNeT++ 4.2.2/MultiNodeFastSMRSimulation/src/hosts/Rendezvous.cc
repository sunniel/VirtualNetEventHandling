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

#include "Rendezvous.h"

Define_Module(Rendezvous);

Rendezvous::Rendezvous() {
    hostCreated = 0;
    STAGE_NUM = stage::RENDEZVOUS_INIT;
}

Rendezvous::~Rendezvous() {
    disposeTimeouts();
}

int Rendezvous::numInitStages() const {
    return STAGE_NUM;
}

void Rendezvous::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        cycle = par("cycle");

        configurator = UnderlayConfiguratorAccess().get();

        WATCH(hostCreated);
        WATCH_SET(survivals);
    }

    if (stage == 1) {
        // initialize replica group
        // add new hosts at the beginning
        groupSize = getAncestorPar("groupSize");
        addNodes(groupSize, true);
    }
}

void Rendezvous::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::HEARTBEAT_RESPONSE)) {
        HBResponse* hbr = check_and_cast<HBResponse*>(msg);
        handleHeartbeatResp(hbr);
    } else if (msg->isName(msg::HEARTBEAT_TIMEOUT)) {
        HBTimeout* timeout = check_and_cast<HBTimeout*>(msg);
        handleTimeout(timeout);

        // add new hosts if the current number of hosts is lower than the threshold
        cModule* parent = this->getParentModule();
        int threshold = parent->par("groupSizeThreshold");
        int size = survivals.size();
        if (size < threshold) {
            this->addNodes(groupSize - size, false);
        }
    } else if (msg->isName(msg::REQ_CONNECT)) {
        handleClientConnect(msg);
    }
}

void Rendezvous::handleClientConnect(cMessage* msg) {
    string senderName = msg->getSenderModule()->getFullName();
    UDPControlInfo* msgCtrl = check_and_cast<UDPControlInfo *>(
            msg->getControlInfo());
    IPvXAddress& srcAddr = msgCtrl->getSrcAddr();
    ConnectReply* reply = new ConnectReply(msg::REPLY_CONNECT);
    stringstream sstream;
    for (set<IPvXAddress>::iterator it = survivals.begin();
            it != survivals.end(); ++it) {
        IPvXAddress address = *it;
        string addrStr = address.get4().str();
        sstream << addrStr << ";";
    }
    string addrs = sstream.str();
    reply->setHostAddrs(addrs.c_str());
    UDPControlInfo* udpControlInfo = new UDPControlInfo();
    udpControlInfo->setDestAddr(srcAddr);
    reply->setControlInfo(udpControlInfo);
    sendIdeal(reply);

    delete msg;
}

void Rendezvous::handleHeartbeatResp(HBResponse* hbr) {

    string host = hbr->getSourceName();
    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
            hbr->getControlInfo());
    IPvXAddress& srcAddr = udpCtrl->getSrcAddr();

//    if (ev.isGUI()) {
//        string msg = "Received HBR from " + host;
//        bubble(msg.c_str());
//    }

    if (timeouts.find(host) != timeouts.end()) {
        TimeoutScheduler* timer = timeouts[host];
        timer->timeout = simTime() + cycle;

        HBProbe *heartbeat = new HBProbe(msg::HEARTBEAT);
        stringstream sstream;
        for (map<IPvXAddress, string>::iterator it = routingTable.begin();
                it != routingTable.end(); ++it) {
            string moduleName = it->first.get4().str();
            sstream << moduleName << ";";
        }
        string members = sstream.str();
        heartbeat->setSurvivals(members.c_str());

//        int gateId = group[host];
        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(host.c_str());
        heartbeat->setTimestamp(simTime());
        heartbeat->setIsInit(false);
//        sendReliable(heartbeat, host, gateId);

        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(srcAddr);
        udpControlInfo->setSrcAddr(ipAddress);
        heartbeat->setControlInfo(udpControlInfo);
        sendReliable(heartbeat);

        // reset timer
        cancelEvent(timer->event);
        timer->event->setTimeout(timer->timeout);
        scheduleAt(timer->timeout, timer->event);
        // update the monitored timeout value
        timeouts[host] = timer;
    } else {
        EV << "[" << simTime() << "s] " << "Survivals no longer contains "
                  << host << endl;
    }
    // delete the received msg
    delete hbr;
}

void Rendezvous::handleTimeout(HBTimeout* timeout) {
    EV << "[" << simTime() << "s] " << "timeout: from "
              << timeout->getHostName() << " to " << fullName << endl;

    string host = timeout->getHostName();
    string hostAddr = timeout->getHostAddr();
    TimeoutScheduler* timer = timeouts[host];
    EV << "[" << simTime() << "s] " << "Rendezvous remove event host: "
              << timer->event->getHostName() << endl;

    if (ev.isGUI()) {
        string msg = "Remove " + host + " from group";
        bubble(msg.c_str());
    }

    IPvXAddress addr(hostAddr.c_str());
    survivals.erase(addr);
    routingTable.erase(addr);
    timeouts.erase(host);
    cancelAndDelete(timer->event);
}

void Rendezvous::addNodes(int num, bool isInit) {
    EV << "[" << simTime() << "s] create " << num << " new hosts" << endl;
    vector<IPvXAddress> newHosts;
    cModule* LC = getParentModule()->getParentModule();

    for (int i = 0; i < num; i++) {
        TransportAddress* nodeAddr = configurator->createNode(LC, hostCreated);
        string nodeName = LC->getSubmodule("node", hostCreated)->getFullName();
        hostCreated++;
        routingTable[nodeAddr->getIp()] = nodeName;
        survivals.insert(nodeAddr->getIp());
        newHosts.push_back(nodeAddr->getIp());
    }

    // initialize heartbeat
    stringstream sstream;
    for (map<IPvXAddress, string>::iterator it = routingTable.begin();
            it != routingTable.end(); ++it) {
        string moduleName = it->first.get4().str();
        sstream << moduleName << ";";
    }
    string members = sstream.str();

    for (size_t i = 0; i < newHosts.size(); i++) {
        IPvXAddress& ip = newHosts[i];
        string hostName = routingTable[ip];

        HBProbe *heartbeat = new HBProbe(msg::HEARTBEAT);
        heartbeat->setSurvivals(members.c_str());
        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(hostName.c_str());
        heartbeat->setTimestamp(simTime());
        heartbeat->setIsInit(isInit);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(ip);
        udpControlInfo->setSrcAddr(ipAddress);
        heartbeat->setControlInfo(udpControlInfo);
        sendReliable(heartbeat);

        // set heartbeat timeout
        TimeoutScheduler* te = new TimeoutScheduler();
        te->timeout = simTime() + cycle;
        HBTimeout* to = new HBTimeout(msg::HEARTBEAT_TIMEOUT);
        to->setHostAddr(ip.get4().str().c_str());
        to->setHostName(hostName.c_str());
        to->setTimeout(te->timeout);
        te->event = to;
        EV << "[" << simTime() << "s] " << hostName << " join the group"
                  << endl;
        timeouts[hostName] = te;
        scheduleAt(te->timeout, te->event);
    }

    newHosts.clear();
}

void Rendezvous::disposeTimeouts() {
    try {
        for (map<string, TimeoutScheduler*>::iterator it = timeouts.begin();
                it != timeouts.end(); ++it) {
            TimeoutScheduler* timer = it->second;
            if (timer != NULL) {
                if (timer->event != NULL) {
                    cancelAndDelete(timer->event);
                }
                delete timer;
            }
        }
        timeouts.clear();
    } catch (exception& e) {
        EV << e.what() << endl;
    }
}
