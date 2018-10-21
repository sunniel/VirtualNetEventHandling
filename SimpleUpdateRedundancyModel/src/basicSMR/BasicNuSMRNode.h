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

#ifndef BASICNUSMRNODE_H_
#define BASICNUSMRNODE_H_

#include <omnetpp.h>
#include <algorithm>
#include "GatebasedModule.h"
#include "Purger.h"
#include "sdbm.h"
#include "Util.h"
#include "LastState.h"
#include "Ready.h"
#include "HBResponse_m.h"
#include "HBProbe_m.h"
#include "CCQuery_m.h"
#include "CCNAck_m.h"
#include "CCQueryReply_m.h"
#include "CCEnd_m.h"
#include "GRQuery_m.h"
#include "GRQueryReply_m.h"
#include "GREnd_m.h"
#include "Event_m.h"
#include "Update_m.h"
#include "BSMRProposal_m.h"
#include "BSMRDecision_m.h"
#include "BSMRAgree_m.h"
#include "BSMRFinal_m.h"

using namespace std;

class BasicNuSMRNode: public GatebasedModule {
private:
    // Group members, a list of tuple (host full name, out-gate Id to the host) with self excluded.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, int> neighbors;
    int groupId;
    // for reconfiguration
    map<string, int> neighborsTemp;

    // The Survivals list, from Rendezvous, self excluded.
    map<string, int> survivals;

    // store the seniority of self and neighbors, controlled by Rendezvous
    map<string, int> seniorities;
    string coord;
    // coordinator candidate
    string coordCand;
    int coordGate;
    int epoch;

    map<string, LastState> lastStates;
    map<string, Ready> readies;

    int rendezvousId;
    simtime_t lifespan;
    simtime_t applyCycle;

    // data structures for event handling
    // tuple (event, arrival time)
    map<string, simtime_t> Qr;
    // tuple (event id (eid), event)
    map<int, string> Qd;
    // cycle to collect events from clients
    simtime_t EHCycle;
    // true - no consensus instance is running; false - some consensus instance is running
    bool idle;
    // store received proposal, in tuple (host name, (event, arrival time)), for ordering
    map<string, map<string, simtime_t> > proposals;
    map<int, string> decisions;
    // the generated state after merging some events
    unsigned long esc;
    // the ID of the last merged event
    int escId;

    // for test and debug purpose
    cMessage* failure;
    cMessage* hostInit;
    cMessage* EHTimeout;
    cMessage* applyTimeout;
    cMessage* purgerInit;
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

    BasicNuSMRNode();
    virtual ~BasicNuSMRNode();

protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);

    virtual void initNeighbors();
    virtual void handleHeartbeat(HBProbe* hb);

    virtual void stageTransfer();

    // coordinate change protocol
    // true - trigger coordinator change, false - do not trigger coordinator change
    virtual bool checkCoord();
    virtual void handleCCQuery(CCQuery* cc);
    virtual void handleCCNAck(CCNAck* ccnack);
    virtual void handleCCQueryReply(CCQueryReply* ccr);
    virtual void handleCCEnd(CCEnd* cce);
    virtual void checkLastStates();
    // compute the coordinator candidate
    virtual string coordinator();
    virtual LastState computeLastState();

    // group reconfiguration protocol
    // true - trigger reconfiguration, false - do not trigger reconfiguration
    virtual bool checkConfig();
    virtual void handleGRQuery(GRQuery* gr);
    virtual void handleGRQueryReply(GRQueryReply* grr);
    virtual void handleGREnd(GREnd* gre);
    virtual void checkReadies();
    virtual Ready computeReady();

    // even handling protocol
    virtual void handleEvent(Event* event);
    virtual void startPropose();
    virtual void collectProposal(BSMRProposal* proposal);
    // content string-style event list to map-style event list
    virtual void contentToEvents(string content,
            map<string, simtime_t>& events);
    virtual void checkProposals();
    virtual void handleDecision(BSMRDecision* decision);
    // compute the state for state synchronization in CC and GR
    virtual LastState syncState(map<string, LastState>& lsMap);
    virtual Ready syncState(map<string, Ready>& readyMap);
    // prepare the data by concatenating Qd and decisions for state synchronization in CC and GR
    virtual string buildStateForSync();
    virtual string QrToString();
    // apply the first event in Qd by encoding it and then adding it with the second value
    virtual void applyEvent();

    // return the sent message for faking the send-to-self process
    virtual cMessage* broadcast(cMessage* message, map<string, int>& hosts);

    virtual void fail();
};

#endif /* BASICNUSMRNODE_H_ */
