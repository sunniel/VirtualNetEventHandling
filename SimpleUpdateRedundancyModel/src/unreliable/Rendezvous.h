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

#ifndef RENDEZVOUS_H_
#define RENDEZVOUS_H_

#include <omnetpp.h>
#include "sdbm.h"
#include "GatebasedModule.h"
#include "Util.h"
#include "Purger.h"
#include "FairLossChannel.h"
#include "Timeout_m.h"
#include "Heartbeat_m.h"
#include "HeartbeatResponse_m.h"
#include "CoordResponse_m.h"

using namespace std;

class TimeoutScheduler {
private:
    int id;
public:
    simtime_t timeout;
    Timeout* event;
    TimeoutScheduler() {
        timeout = 0.0;
        event = NULL;
    }
    ~TimeoutScheduler() {
    }
};

class Rendezvous: public GatebasedModule {
private:
    // tuple (host full name, out gate Id to the host)
    map<string, int> group;
    // store the seniority of self and neighbors
    map<string, int> seniorities;
    // It is better to use pointer in map if the referenced object contains a pointer member.
    // The pointer should be released later.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, TimeoutScheduler*> timeouts;
    simtime_t cycle;
    // host creation counter
    int hostCreated;
    cMessage* rendezvousInit;
public:
    Rendezvous();
    virtual ~Rendezvous();

protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);
    virtual void initGroup();
    virtual void handleHeartbeatResp(HeartbeatResponse* hbr);
    virtual void handleTimeout(Timeout* timeout);
    virtual void handleHeartbeatInit(cMessage* msg);

    virtual void addNodes(int num);
    virtual void disposeTimeouts();
    virtual void printTimeouts();
    virtual string coordinator();
};

Define_Module(Rendezvous);
#endif /* RENDEZVOUS_H_ */
