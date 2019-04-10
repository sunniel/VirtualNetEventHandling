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

#ifndef _UNDERLAYCONFIGURATOR_H_
#define _UNDERLAYCONFIGURATOR_H_

#include <cstdint>
#include <omnetpp.h>
#include "../common/HostBase.h"
#include "GlobalNodeListAccess.h"
#include "GlobalParametersAccess.h"
#include "ChurnGeneratorAccess.h"
#include "CoordinatorAccess.h"
#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"
#include "../objects/TransportAddress.h"
#include "../objects/ClientInfo.h"
#include "../others/InterfaceTableAccess.h"
#include "../messages/Failure_m.h"

using namespace std;
using namespace omnetpp;

/**
 * Base class for configurators of different underlay models
 *
 * @author Stephan Krause, Bernhard Heep
 */
class UnderlayConfigurator: public cSimpleModule {
public:
    UnderlayConfigurator();
    virtual ~UnderlayConfigurator();
    TransportAddress* createLogicComputer();
    TransportAddress* createNode(cModule* parent, int nodeCreated);
    TransportAddress* registerEndpoint(cModule* host);
    void removeNode(IPvXAddress& nodeAddr);
    void removeLogicComputer(IPvXAddress& rendezvousAddr);
    void revokeNode(IPvXAddress& nodeAddr);
    void handleClientInit(cMessage* msg);
    void disposeFailures();
protected:
    /**
     * OMNeT number of init stages
     */
    int numInitStages() const;
    /**
     * OMNeT init methods
     */
    virtual void initialize(int stage);
    /**
     * Node mobility simulation
     *
     * @param msg timer-message
     */
    virtual void handleMessage(cMessage* msg);

private:
    // logic computer creation counter
    int lcCreated;
    int clientCreated;

    uint32_t nextFreeAddress;
    GlobalNodeList* globalNodeList;
    GlobalParameters* parameterList;
    Coordinator* coordinator;

    // record the time of death of each node, in (node_address, node_death_time)
    map<string, simtime_t> death_schedule;
    map<string, Failure*> failures;
};

#endif
