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

#ifndef BASICSMRCLIENT_H_
#define BASICSMRCLIENT_H_

#include <omnetpp.h>
#include <functional>
#include "Purger.h"
#include "GatebasedModule.h"
#include "sdbm.h"
#include "Event_m.h"
#include "Update_m.h"
#include "Timeout_m.h"

using namespace std;

class BasicSMRClient: public GatebasedModule {
private:
    unsigned long id;
    int seq;
    // tuple (update ID, update content)
    map<int, string> trace;
    cMessage* eventCycle;
    cMessage* clientInit;
    // for message drop test
    map<string, Timeout*> eventTimeouts;
    simtime_t timeoutCycle;

    // for result recording
    simsignal_t eventRoundTrip;
    // tuple (event sequence #, event sending time)
    map<string, simtime_t> eventTS;
    // tuple (event sequence #, update receiving time)
    map<string, simtime_t> updateTS;
    // tuple (event sequence #, interaction delay)
    map<string, simtime_t> delays;
    double timeoutEvents;
protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);
    virtual void finish();
public:
    BasicSMRClient();
    virtual ~BasicSMRClient();
};

#endif /* BASICSMRCLIENT_H_ */
