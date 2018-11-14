//
// Copyright (C) 2006 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file GlobalNodeList.cc
 * @author Markus Mauch, Robert Palmer, Ingmar Baumgart
 */

#include <iostream>
#include <hashWatch.h>
#include "GlobalNodeList.h"

Define_Module(GlobalNodeList);

std::ostream& operator<<(std::ostream& os, const BootstrapEntry& entry)
{
    for (AddrPerOverlayVector::const_iterator it2 = entry.addrVector.begin();
            it2 != entry.addrVector.end(); it2++) {

//        NodeHandle* nodeHandle = dynamic_cast<NodeHandle*>(it2->ta);

        os << "Overlay " << it2->overlayId << ": " << *(it2->ta);

//        if (nodeHandle) {
//            os << " (" << nodeHandle->getKey() << ")";
//        }
//
//        if (it2->bootstrapped == false) {
//            os << " [NOT BOOTSTRAPPED]";
//        }
    }

    os << " " << *(entry.info);

    return os;
}

void GlobalNodeList::initialize()
{
//    maxNumberOfKeys = par("maxNumberOfKeys");
//    keyProbability = par("keyProbability");
//    isKeyListInitialized = false;
    WATCH_UNORDERED_MAP(peerStorage.getPeerHashMap());
//    WATCH_VECTOR(keyList);
//    WATCH(landmarkPeerSize);

//    landmarkPeerSize = 0;
//
//    for (int i = 0; i < MAX_NODETYPES; i++) {
//        landmarkPeerSizePerType[i] = 0;
//    }
//
//    preKilledNodes = 0;
//
//    if (par("maliciousNodeChange")) {
//        if ((double) par("maliciousNodeProbability") > 0)
//            error("maliciousNodeProbability and maliciousNodeChange are not supported concurrently");
//
//        cMessage* msg = new cMessage("maliciousNodeChange");
//        scheduleAt(simTime() + (int) par("maliciousNodeChangeStartTime"), msg);
//        maliciousNodesVector.setName("MaliciousNodeRate");
//        maliciousNodesVector.record(0);
//        maliciousNodeRatio = 0;
//    }
//
//    for (int i=0; i<MAX_NODETYPES; i++) {
//        for (int j=0; j<MAX_NODETYPES; j++) {
//            connectionMatrix[i][j] = true;
//        }
//    }
//
//    globalStatistics = GlobalStatisticsAccess().get();
//
//    cMessage* timer = new cMessage("oracleTimer");
//
//    scheduleAt(simTime(), timer);
}

void GlobalNodeList::handleMessage(cMessage* msg)
{
//    if (msg->isName("oracleTimer")) {
//        RECORD_STATS(globalStatistics->recordOutVector(
//                     "GlobalNodeList: Number of nodes", peerStorage.size()));
//        scheduleAt(simTime() + 50, msg);
//    } else {
//        opp_error("GlobalNodeList::handleMessage: Unknown message type!");
//    }
}

void GlobalNodeList::addPeer(const IPvXAddress& ip, PeerInfo* info)
{
    BootstrapEntry temp;
    temp.info = info;
//    temp.info->setPreKilled(false);

    peerStorage.insert(std::make_pair(ip, temp));

//    if (uniform(0, 1) < (double) par("maliciousNodeProbability") ||
//            (par("maliciousNodeChange") && uniform(0, 1) < maliciousNodeRatio)) {
//        setMalicious(TransportAddress(ip), true);
//    }
//
//    if (peerStorage.size() == 1) {
//        // we need at least one inoffensive bootstrap node
//        setMalicious(TransportAddress(ip), false);
//    }
}

void GlobalNodeList::removePeer(const TransportAddress& peer,
                                int32_t overlayId)
{
    PeerHashMap::iterator it = peerStorage.find(peer.getIp());

    if (it != peerStorage.end()) {
        peerStorage.setBootstrapped(it, overlayId, false);
    }
}

void GlobalNodeList::killPeer(const IPvXAddress& ip)
{
    PeerHashMap::iterator it = peerStorage.find(ip);
    if (it != peerStorage.end()) {
//        if (it->second.info->isPreKilled()) {
//            it->second.info->setPreKilled(false);
//            preKilledNodes--;
//        }
//
//        // if valid NPS landmark: decrease landmarkPeerSize
//        PeerInfo* peerInfo = it->second.info;
//        if (peerInfo->getNpsLayer() > -1) {
//            landmarkPeerSize--;
//            landmarkPeerSizePerType[it->second.info->getTypeID()]--;
//        }

        peerStorage.erase(it);
    }
}

PeerInfo* GlobalNodeList::getPeerInfo(const TransportAddress& peer)
{
    return getPeerInfo(peer.getIp());
}

PeerInfo* GlobalNodeList::getPeerInfo(const IPvXAddress& ip)
{
    PeerHashMap::iterator it = peerStorage.find(ip);

    if (it == peerStorage.end())
        return NULL;
    else
        return it->second.info;
}

std::vector<IPvXAddress>* GlobalNodeList::getAllIps()
{
    std::vector<IPvXAddress>* ips = new std::vector<IPvXAddress>;

    const PeerHashMap::iterator it = peerStorage.begin();

    while (it != peerStorage.end()) {
        ips->push_back(it->first);
    }

    return ips;
}
