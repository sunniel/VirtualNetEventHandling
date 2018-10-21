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

#ifndef UNRELIABLEHOST_H_
#define UNRELIABLEHOST_H_

#include <omnetpp.h>
#include <algorithm>
#include "GatebasedModule.h"
#include "Purger.h"
#include "sdbm.h"
#include "Util.h"
#include "Event_m.h"
#include "Heartbeat_m.h"
#include "HeartbeatResponse_m.h"
#include "CoordChange_m.h"
#include "CoordChangeResp_m.h"
#include "Update_m.h"
#include "Reconfig_m.h"
#include "ReconfigResp_m.h"

using namespace std;

class UnreliableHost: public GatebasedModule {
private:
    // Group members, a list of tuple (host full name, out-gate Id to the host) with self excluded.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, int> neighbors;
    map<string, int> neighborsTemp;
    // The Survivals list, from Rendezvous, self excluded.
    map<string, int> survivals;
    // store the seniority of self and neighbors, controlled by Rendezvous
    map<string, int> seniorities;
    string coord;
    int coordGate;
    vector<string> events;
    set<string> coordChangeReply;
    set<string> reconfigReply;
    int rendezvousId;
    // for test and debug purpose
    cMessage* failure;
    simtime_t lifespan;
    cMessage* hostInit;
    // TODO for coord failure test purpose
    cMessage* coordFail;
public:
    enum State {
        On, Off
    };
    State state;
    // 0 - Event Handling (EH); 1 - Group Reconfiguration (GR); 2 - Coordinator Change (CC)
    enum Stage {
        EH, GR, CC
    };
    Stage stage;

    UnreliableHost();
    virtual ~UnreliableHost();
    virtual void setRendezvous(int rendId) {
        this->rendezvousId = rendId;
    }

protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);
    virtual void initNeighbors();
    virtual void handleEvent(Event* event);
    virtual void handleUpdate(Update* event);
    virtual void handleHeartbeat(Heartbeat* hb);
    virtual void handleCoordChange(CoordChange* cc);
    virtual void handleCoordChangeResp(CoordChangeResp*ccr);
    virtual void handleReconfig(Reconfig* reconfig);
    virtual void transferStage();
    // true - trigger coordinator change, false - do not trigger coordinator change
    virtual bool checkCoord();
    // true - trigger reconfiguration, false - do not trigger reconfiguration
    virtual bool checkConfig();
    // compute the coordinator candidate
    virtual string coordinator();
    virtual void checkCoordChangeResp();
    // return the sent message for faking the send-to-self process
    virtual cMessage* broadcast(cMessage* message, map<string, int>& hosts);
    virtual void fail();
};

Define_Module(UnreliableHost);

#endif /* UNRELIABLEHOST_H_ */
