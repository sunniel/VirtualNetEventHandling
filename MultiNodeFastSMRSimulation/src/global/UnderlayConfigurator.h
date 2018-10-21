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

#include <omnetpp.h>
#include "IPAddress.h"
#include "IPvXAddress.h"
#include "SimpleNodeEntry.h"
#include "SimpleInfo.h"
#include "GlobalNodeListAccess.h"
#include "GlobalParametersAccess.h"
#include "InterfaceTableAccess.h"
#include "ChurnGeneratorAccess.h"
#include "TransportAddress.h"
#include "HostBase.h"
#include "Failure_m.h"

using namespace std;

/**
 * Base class for configurators of different underlay models
 *
 * @author Stephan Krause, Bernhard Heep
 */
class UnderlayConfigurator : public cSimpleModule
{
public:
    UnderlayConfigurator();
    virtual ~UnderlayConfigurator();
    TransportAddress* createLogicComputer();
    TransportAddress* createNode(cModule* parent, int nodeCreated);
    TransportAddress* registerEndpoint(cModule* host);
    void removeNode(IPvXAddress& nodeAddr);
    /**
     * Address of the Rendezvous
     */
    void removeLogicComputer(IPvXAddress& rendezvousAddr);
    void revokeNode(IPvXAddress& nodeAddr);

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
    virtual void disposeFailures();
    virtual void initLogicComputer();

private:
    bool init;
    bool simulationEndingSoon;
    bool transitionTimeFinished;
    unsigned int initCounter;
    // logic copmuter creation counter
    int lcCreated;

    uint32 nextFreeAddress;
    GlobalNodeList* globalNodeList;
    GlobalParameters* parameterList;

    map<string, Failure*> failures;
    cMessage* initLC;
};

#endif
