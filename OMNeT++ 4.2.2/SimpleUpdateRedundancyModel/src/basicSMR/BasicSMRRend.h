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

#ifndef BASICSMRREND_H_
#define BASICSMRREND_H_

#include <omnetpp.h>
#include "sdbm.h"
#include "GatebasedModule.h"
#include "Util.h"
#include "Purger.h"
#include "FairLossChannel.h"
#include "HBTimeout_m.h"
#include "HBResponse_m.h"
#include "HBProbe_m.h"

using namespace std;

class TimeoutScheduler {
private:
    int id;
public:
    simtime_t timeout;
    HBTimeout* event;
    TimeoutScheduler() {
        timeout = 0.0;
        event = NULL;
    }
    ~TimeoutScheduler() {
    }
};

class BasicSMRRend: public GatebasedModule {
private:
    cMessage* rendezvousInit;
    cMessage* init;
    cMessage* hbInit;
public:
    BasicSMRRend();
    virtual ~BasicSMRRend();

protected:
    // tuple (host full name, out gate Id to the host)
    map<string, int> group;
    // It is better to use pointer in map if the referenced object contains a pointer member.
    // The pointer should be released later.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, TimeoutScheduler*> timeouts;
    simtime_t cycle;
    // host creation counter
    int hostCreated;

    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);
    virtual void initGroup();
    virtual void handleHeartbeatResp(HBResponse* hbr);
    virtual void handleTimeout(HBTimeout* timeout);
    virtual void handleHeartbeatInit(cMessage* msg);

    virtual void addNodes(int num, bool isInit);
    virtual void disposeTimeouts();
};

#endif /* BASICSMRREND_H_ */
