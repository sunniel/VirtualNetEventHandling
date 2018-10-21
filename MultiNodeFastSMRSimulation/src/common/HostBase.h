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
#include "IPvXAddress.h"
#include "GlobalNodeList.h"
#include "GlobalNodeListAccess.h"
#include "UDPControlInfo_m.h"
#include "SimpleNodeEntry.h"
#include "SimpleInfo.h"
#include "Constants.h"
#include "Util.h"

using namespace std;

// Gate-based simple module
class HostBase: public cSimpleModule {
private:
//    simtime_t delay;
//    simtime_t maxDelay;
//    double dropRate;
    int moduleId;

    GlobalNodeList* globalNodeList;
public:
    HostBase();
    virtual ~HostBase();
    virtual void setIPAddress(IPvXAddress& addr);
    virtual IPvXAddress getIPAddress();

protected:
    const char* fullName;
    IPvXAddress ipAddress;
    simtime_t clockError;

    // transmit function for sending a message to another node in the same group
    virtual IPvXAddress getIPAddress(string ipAddr);
    virtual void multicast(cMessage* message, set<IPvXAddress> destAddrs);
    virtual void multicast(cMessage* message, set<string> destAddrs);
    virtual void transmit(cMessage* msg, IPvXAddress dest);
    virtual void sendReliable(cMessage* msg);
    virtual void sendIdeal(cMessage* msg);
    virtual void sendMsg(cMessage* msg);

    virtual void syncClock();
    virtual simtime_t readClock();

    virtual void initialize();

    virtual void handleMessage(cMessage* msg);

    virtual void dispatchHandler(cMessage* msg) = 0;
};

#endif /* GATEBASEDMODULE_H_ */
