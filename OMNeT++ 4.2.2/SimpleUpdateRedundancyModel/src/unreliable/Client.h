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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <functional>
#include <omnetpp.h>
#include <sstream>
#include "Purger.h"
#include "GatebasedModule.h"
#include "Util.h"
#include "sdbm.h"
#include "CoordResponse_m.h"
#include "Event_m.h"
#include "Update_m.h"
#include "Timeout_m.h"

using namespace std;

class Client: public GatebasedModule {
private:
    unsigned long id;
    int seq;
    string rendezvous;
    int rendezvousId;
    string coord;
    int coordId;
    cMessage* eventCycle;
    cMessage* clientInit;
    map<string, int> dests;
    map<int, Timeout*> eventTimeouts;
    simtime_t timeoutCycle;

    // for result recording
    simsignal_t eventRoundTrip;
    // tuple (event sequence #, event sending time)
    map<int, simtime_t> eventTS;
    // tuple (event sequence #, update receiving time)
    map<int, simtime_t> updateTS;
    // tuple (event sequence #, interaction delay)
    map<int, simtime_t> delays;
    double timeoutEvents;
protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);
    virtual void finish();
public:
    Client();
    virtual ~Client();
};

#endif /* CLIENT_H_ */
