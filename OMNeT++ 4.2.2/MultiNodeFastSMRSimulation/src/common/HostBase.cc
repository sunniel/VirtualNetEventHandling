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

#include "HostBase.h"

HostBase::HostBase() {
    // TODO Auto-generated constructor stub
}

HostBase::~HostBase() {
    // TODO Auto-generated destructor stub
}

void HostBase::initialize() {
    fullName = getFullName();
    moduleId = getId();
//    maxDelay = simulation.getSystemModule()->par("maxDelay");
//    dropRate = simulation.getSystemModule()->par("dropRate").doubleValue();
    globalNodeList = GlobalNodeListAccess().get();
    clockError = 0;
    WATCH(ipAddress);
    WATCH(moduleId);
    WATCH(clockError);
}

IPvXAddress HostBase::getIPAddress(string ipAddr) {
    IPvXAddress addr(ipAddr.c_str());
    return addr;
}

IPvXAddress HostBase::getIPAddress() {
    return ipAddress;
}

void HostBase::multicast(cMessage* message, set<IPvXAddress> destAddrs) {
    for (set<IPvXAddress>::iterator it = destAddrs.begin();
            it != destAddrs.end(); ++it) {
        cMessage* msgDup = message->dup();
        IPvXAddress dest = *it;
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(dest);
        udpControlInfo->setSrcAddr(ipAddress);
        msgDup->setControlInfo(udpControlInfo);
        transmit(msgDup, dest);
    }
    delete message;
}

void HostBase::multicast(cMessage* message, set<string> destAddrs) {
    for (set<string>::iterator it = destAddrs.begin(); it != destAddrs.end();
            it++) {
        cMessage* msgDup = message->dup();
        IPvXAddress dest = getIPAddress(*it);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(dest);
        udpControlInfo->setSrcAddr(ipAddress);
        msgDup->setControlInfo(udpControlInfo);
        transmit(msgDup, dest);
    }
    delete message;
}

void HostBase::transmit(cMessage* msg, IPvXAddress dest) {
    if (dest == ipAddress) {
        sendIdeal(msg);
    } else {
        sendMsg(msg);
    }
}

//void HostBase::sendReliable(cMessage* msg) {
//    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
//            msg->getControlInfo());
//    IPvXAddress destAddr = udpCtrl->getDestAddr();
//    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
//    if (peerInfo != NULL) {
//        SimpleInfo* simpleInfo =
//                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
//        SimpleNodeEntry* destEntry = simpleInfo->getEntry();
//
//        delay = simulation.getSystemModule()->par("delay");
//        cSimpleModule::sendDirect(msg, delay <= maxDelay ? delay : maxDelay, 0,
//                destEntry->getTcpIPv4Gate());
//    } else {
//        delete msg;
//    }
//}

void HostBase::sendIdeal(cMessage* msg) {
    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
            msg->getControlInfo());
    IPvXAddress destAddr = udpCtrl->getDestAddr();
    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
    if (peerInfo != NULL) {
        SimpleInfo* simpleInfo =
                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
        SimpleNodeEntry* destEntry = simpleInfo->getEntry();

        cSimpleModule::sendDirect(msg, 0, 0, destEntry->getUdpIPv4Gate());
    } else {
        delete msg;
    }
}

//void HostBase::sendMsg(cMessage* msg) {
//    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
//            msg->getControlInfo());
//    IPvXAddress destAddr = udpCtrl->getDestAddr();
//    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
//    if (peerInfo != NULL) {
//        SimpleInfo* simpleInfo =
//                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
//        SimpleNodeEntry* destEntry = simpleInfo->getEntry();
//
//        delay = simulation.getSystemModule()->par("delay");
//        if (uniform(0, 1) > dropRate) {
//            cSimpleModule::sendDirect(msg, delay <= maxDelay ? delay : maxDelay,
//                    0, destEntry->getUdpIPv4Gate());
//        } else {
//            delete msg;
//        }
//    } else {
//        delete msg;
//    }
//}

void HostBase::sendMsg(cMessage* msg) {
    send(msg, this->gateHalf("udplink", cGate::OUTPUT));
}

void HostBase::sendReliable(cMessage* msg) {
    send(msg, this->gateHalf("tcplink", cGate::OUTPUT));
}

void HostBase::syncClock() {
    clockError = 0;
}

simtime_t HostBase::readClock() {
    return simTime() + clockError;
}

void HostBase::setIPAddress(IPvXAddress& addr) {
    ipAddress = addr;
}

void HostBase::handleMessage(cMessage* msg) {

    dispatchHandler(msg);
}
