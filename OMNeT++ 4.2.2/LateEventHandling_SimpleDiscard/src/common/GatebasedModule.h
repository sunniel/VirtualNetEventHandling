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

#ifndef GATEBASEDMODULE_H_
#define GATEBASEDMODULE_H_

#include <omnetpp.h>
#include "Constants.h"
#include "Purger.h"
#include "Util.h"

using namespace std;

// Gate-based simple module
class GatebasedModule: public cSimpleModule {
public:
    GatebasedModule();
    virtual ~GatebasedModule();

protected:
    const char* fullName;

    // transmit function for sending a message to another node in the same group
    virtual void multicast(cMessage* message, map<string, int>& hosts);
    virtual void transmit(cMessage* msg, string arrivalHostName, int gateid);
    virtual void sendReliable(cMessage* msg, string arrivalHostName, int gateid);
    virtual void sendReliable(cMessage* msg, string arrivalHostName, int gateid,
            const char* channelDisplayString);
    virtual void sendIdeal(cMessage* msg, string arrivalHostName, int gateid);
    virtual void sendIdeal(cMessage* msg, string arrivalHostName, int gateid,
            const char* channelDisplayString);
    void sendMsg(cMessage* msg, string arrivalHostName, int gateid);
    void sendMsg(cMessage* msg, string arrivalHostName, const char *gatename,
            int gateindex = -1);
    void sendMsg(cMessage* msg, string arrivalHostName, cGate *outputgate);

    virtual void initialize();

    virtual void handleMessage(cMessage* msg);

    virtual void dispatchHandler(cMessage* msg) = 0;

    // return the out-gate ID referring to the created connection
    virtual bool createConnection(cModule* module);
    virtual bool createReliableConnection(cModule* module,
            const char* channelDisplayString = DISPLAY_STR_CH_HIDDEN);
    virtual bool createIdealConnection(cModule* module,
            const char* channelDisplayString = DISPLAY_STR_CH_HIDDEN);
    // create pair of gates at the local and the remote module
    virtual int createGates(cModule* module);
    // moduleName: module full name
    virtual cModule* getModuleByName(string moduleName);
};

#endif /* GATEBASEDMODULE_H_ */
