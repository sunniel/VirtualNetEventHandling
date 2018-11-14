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

#include "ReliableUDP.h"

Define_Module(ReliableUDP);

void ReliableUDP::initialize() {
    Copt = simulation.getSystemModule()->par("Copt");
    BW = simulation.getSystemModule()->par("BW");
    MTU = simulation.getSystemModule()->par("MTU");
    mu = simulation.getSystemModule()->par("jitter_mu");
    sigma = simulation.getSystemModule()->par("jitter_sigma");
    pktLossRate = simulation.getSystemModule()->par("pktLossRate");
}

void ReliableUDP::handleMessage(cMessage *msg) {
    if (msg->getArrivalGateId()
            == this->gateHalf("uplink", cGate::INPUT)->getId()) {
        // process packet from upper layer
        handleOutgoingMessage(check_and_cast<cPacket*>(msg));

    } else if (msg->getArrivalGateId()
            == this->gateHalf("downlink", cGate::INPUT)->getId()) {
        // process packet from remote host
        handleIncomingMessage(check_and_cast<cPacket*>(msg));
    }
}

void ReliableUDP::handleOutgoingMessage(cPacket* packet) {
    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
            packet->getControlInfo());
    IPvXAddress srcAddr = udpCtrl->getSrcAddr();
    IPvXAddress destAddr = udpCtrl->getDestAddr();
    GlobalNodeList* globalNodeList = GlobalNodeListAccess().get();
    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
    if (peerInfo != NULL) {
        SimpleInfo* simpleInfo =
                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
        SimpleNodeEntry* destEntry = simpleInfo->getEntry();

        // calculate the minimum one-way latency, in unit of millisecond
        double distance = GlobalParametersAccess().get()->getDistance(srcAddr,
                destAddr);
        double Dmin = 1000.0 * (distance / Copt);
        // calculate one-way jitter
        double j = lognormal(mu, sigma) / 2;
        // calculate the communication delay
        double Dcomm = MTU / BW;
        //    double Dcomm = truncnormal(j, j);
        // calculate the variable latency
        double Dvar = exponential(1 / j);
        // calculate the whole latency, in millisecond
        simtime_t delay = (Dmin + Dcomm + Dvar) / 1000;

        if (uniform(0, 1) > pktLossRate) {
            cSimpleModule::sendDirect(packet, delay, 0,
                    destEntry->getUdpIPv4Gate());
        } else {
            delete packet;
        }
    } else {
        delete packet;
    }
}

void ReliableUDP::handleIncomingMessage(cPacket* packet) {
    send(packet, this->gateHalf("uplink", cGate::OUTPUT));
}
