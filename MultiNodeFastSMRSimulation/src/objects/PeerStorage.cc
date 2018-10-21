//
// Copyright (C) 2010 Karlsruhe Institute of Technology (KIT),
//                    Institute of Telematics
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
 * @file PeerStorage.cc
 * @author Ingmar Baumgart
 */

#include <PeerInfo.h>

#include "PeerStorage.h"

PeerStorage::~PeerStorage()
{
    PeerHashMap::iterator it;
    for (it = peerHashMap.begin(); it != peerHashMap.end(); it++) {
        delete it->second.info;
    }
}

size_t PeerStorage::size()
{
    return peerHashMap.size();
}

const PeerHashMap::iterator PeerStorage::find(const IPvXAddress& ip)
{
    return peerHashMap.find(ip);
}

const PeerHashMap::iterator PeerStorage::begin()
{
    return peerHashMap.begin();
}

const PeerHashMap::iterator PeerStorage::end()
{
    return peerHashMap.end();
}

size_t PeerStorage::offsetSize()
{
    return 1<<2;
}

uint8_t PeerStorage::calcOffset(bool bootstrapped, bool malicious)
{
    uint8_t offset = 0;
    if (bootstrapped) offset += 1<<0;
    if (malicious) offset += 1<<1;
    return offset;
}

//void PeerStorage::insertMapIteratorIntoVector(PeerVector& peerVector,
//                                              PeerHashMap::iterator it)
//{
//    PeerInfo* peerInfo = it->second.info;
//
//    bool bootstrapped = false;
//    for (AddrPerOverlayVector::iterator oit = it->second.addrVector.begin();
//            oit != it->second.addrVector.end(); oit++) {
//
//        bootstrapped |= oit->bootstrapped;
//    }
//
//    bool malicious = peerInfo->isMalicious();
//    size_t partition = peerInfo->getTypeID();
//    size_t offset = calcOffset(bootstrapped, malicious);
//    size_t partitionIndex = partition*offsetSize()+offset;
//
//#if 0
//    std::cout << "INSERT " << it->first << " partitionIndex:"
//              << partitionIndex
//              << " bootstrapped:" << bootstrapped << " malicious:"
//              << malicious << std::endl;
//#endif
//
//    if (peerVector.size() < (partition + 1)*offsetSize()) {
//        int i = peerVector.size();
//        peerVector.resize(offsetSize()*(partition + 1));
//        freeVector.resize(offsetSize()*(partition + 1));
//        while (i < (int)(peerVector.size())) {
//            peerVector[i++].reserve(30000);
//            freeVector[i++].reserve(30000);
//        }
//    }
//
//    size_t index = -1;
//
//    if ((freeVector.size() >= (partition + 1)*offsetSize()) &&
//            (freeVector[partitionIndex].size())) {
//
//        index = freeVector[partitionIndex].back();
//        freeVector[partitionIndex].pop_back();
//        peerVector[partitionIndex][index] = it;
//        //std::cout << "\t REUSING position " << index << std::endl;
//    } else {
//        index = peerVector[partitionIndex].size();
//        peerVector[partitionIndex].push_back(it);
//        //std::cout << "\t APPENDING at position " << index << std::endl;
//    }
//
//    it->second.peerVectorIndex = index;
//}

//void PeerStorage::removeMapIteratorFromVector(PeerVector& peerVector,
//                                              PeerHashMap::iterator it)
//{
//    PeerInfo* peerInfo = it->second.info;
//
//    bool bootstrapped = false;
//    for (AddrPerOverlayVector::iterator oit = it->second.addrVector.begin();
//            oit != it->second.addrVector.end(); oit++) {
//
//        bootstrapped |= oit->bootstrapped;
//    }
//
//    bool malicious = peerInfo->isMalicious();
//    size_t partition = peerInfo->getTypeID();
//    size_t offset = calcOffset(bootstrapped, malicious);
//    size_t index = it->second.peerVectorIndex;
//    size_t partitionIndex = partition*offsetSize()+offset;
//
//    if (peerVector[partitionIndex].size() == (index + 1)) {
//        peerVector[partitionIndex].pop_back();
//    } else {
//        peerVector[partitionIndex][index] = peerHashMap.end();
//        freeVector[partitionIndex].push_back(index);
//    }
//    //std::cout << "ERASE " << it->first << " partitionIndex:" << partitionIndex
//    //          << " index: " << index << std::endl;
//}

std::pair<const PeerHashMap::iterator, bool> PeerStorage::insert(const std::pair<IPvXAddress, BootstrapEntry>& element)
{
    std::pair<PeerHashMap::iterator, bool> ret;

    ret = peerHashMap.insert(element);

    if (ret.second) {
//        insertMapIteratorIntoVector(globalPeerVector, ret.first);
    }

    return ret;
}

void PeerStorage::erase(const PeerHashMap::iterator it)
{
//    removeMapIteratorFromVector(globalPeerVector, it);
    delete it->second.info;
    peerHashMap.erase(it);
}

void PeerStorage::setBootstrapped(const PeerHashMap::iterator it,
                                  int32_t overlayId, bool bootstrapped)
{
    if (it == peerHashMap.end()) {
        throw cRuntimeError("GlobalNodeList::setBootstrapped(): Node not found!");
    }

//    removeMapIteratorFromVector(globalPeerVector, it);
    //std::cout << "setBootstrapped: " << bootstrapped << " overlayId: "
    //          << overlayId << std::endl;

    AddrPerOverlayVector::iterator oit = it->
            second.addrVector.getIterForOverlayId(overlayId);

    if (oit != it->second.addrVector.end()) {
        oit->bootstrapped = bootstrapped;
    } else if (bootstrapped) {
        throw cRuntimeError("PeerStorage::setBootstrapped: No valid entry found!");
    }

//    insertMapIteratorIntoVector(globalPeerVector, it);
}
