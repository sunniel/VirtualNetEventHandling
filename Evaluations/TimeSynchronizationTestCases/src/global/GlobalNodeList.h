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
 * @file GlobalNodeList.h
 * @author Markus Mauch, Robert Palmer
 */

#ifndef __GLOBALNODELIST_H__
#define __GLOBALNODELIST_H__

#include <map>
#include <vector>

#include <omnetpp.h>
#include "../objects/PeerStorage.h"

class TransportAddress;

using namespace omnetpp;


/**
 * Global module (formerly known as BootstrapList) that supports the node
 * bootstrap process and contains node specific underlay parameters,
 * malicious node states, etc...
 *
 * @author Markus Mauch, Robert Palmer
 */
class GlobalNodeList : public cSimpleModule
{
public:
    /**
     * Adds new peers to the peer set.
     *
     * Called automatically by the underlay,
     * when new peers are created.
     *
     * @param ip IPvXAddress of the peer to add
     * @param info underlay specific info of the peer to add
     */
    void addPeer(const IPvXAddress& ip, PeerInfo* info);


    /**
     * Removes a peer from the peerSet.
     *
     * Called automatically by the underlay,
     * when peers are removed.
     *
     * @param ip IPvXAddress of the peer to remove
     */
    virtual void killPeer(const IPvXAddress& ip);


    /**
     * Searches the peerSet for the specified node
     *
     * @param peer TransportAddress of the specified node
     * @return PeerInfo of the node or NULL if node is not in peerSet
     */
    virtual PeerInfo* getPeerInfo(const TransportAddress& peer);

    /**
     * Searches the peerSet for the specified node
     *
     * @param ip IPvXAddress of the specified node
     * @return PeerInfo of the node or NULL if node is not in peerSet
     */
    virtual PeerInfo* getPeerInfo(const IPvXAddress& ip);

    size_t getNumNodes() { return peerStorage.size(); };

    std::vector<IPvXAddress>* getAllIps();

protected:
    /**
     * Init member function of module
     */
    virtual void initialize();

    /**
     * HandleMessage member function of module
     *
     * @param msg messag to handle
     */
    virtual void handleMessage(cMessage* msg);

    PeerStorage peerStorage; /**< Set of nodes participating in the overlay */

private:
};

#endif
