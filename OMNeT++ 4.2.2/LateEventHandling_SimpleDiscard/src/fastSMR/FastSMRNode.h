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

#ifndef FASTSMRNODE_H_
#define FASTSMRNODE_H_

#include <omnetpp.h>
#include <algorithm>
#include "GatebasedModule.h"
#include "Purger.h"
#include "sdbm.h"
#include "Util.h"
#include "CycleEvent_m.h"
#include "RoundQuery_m.h"
#include "RoundQueryReply_m.h"
#include "InitJoin_m.h"
#include "Join_m.h"
#include "ConsensusQuery_m.h"
#include "FSMRProposal_m.h"
#include "FSMRDecision_m.h"
#include "FSMRAgree_m.h"
#include "FSMRFinal_m.h"
#include "FSMRCCQueryReply_m.h"
#include "FSMRLastState.h"
#include "FSMRCCEnd_m.h"
#include "FSMRGREnd_m.h"
#include "FSMRReady.h"
#include "HBResponse_m.h"
#include "HBProbe_m.h"
#include "CCNAck_m.h"
#include "CCQuery_m.h"
#include "GRQuery_m.h"
#include "GRQueryReply_m.h"
#include "Event_m.h"
// headers having not been checked
#include "Update_m.h"

using namespace std;

class FastSMRNode: public GatebasedModule {
private:
    bool isMember;
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

    // store the address of clients
    map<string, int> clients;

    string coord;
    // coordinator candidate
    string coordCand;
    int coordGate;
    int epoch;

    map<string, FSMRLastState> lastStates;
    map<string, Ready> readies;

    int rendezvousId;
    simtime_t lifespan;
    simtime_t applyCycle;
    simtime_t roundCycle;
    simtime_t cycleStart;

    // for event collection
    // round ID, start from 1, since in collectRoundEvents, round number is increased first before collecting round events
    int round;
    // record the round of applied events
    int appliedRound;
    // time of the first round
//    simtime_t ts;
    // tuple (client ID, seq_s - r_s) where seq_s - r_s is the difference of start sequence and joining round
    map<unsigned long, int> senders;
    // data structures for event handling
    set<int> toPropose;
    set<int> proposed;
    // tuple (client ID, tuple (sequence #, event content))
    map<unsigned long, map<int, string> > Qr;
    // tuple (round#, list of tuple (client ID, event content)) of current round
    map<int, map<unsigned long, string> > Qp;
    // tuple (round#, list of tuple (client ID, event content))
    map<int, map<unsigned long, string> > Qd;
    // applied event queue, tuple (event index, event content)
    map<int, string> Qa;

    // store received proposal, in tuple (round#, (host name, (sender id, event))), for ordering
    map<int, map<string, map<unsigned long, string> > > proposals;
    // tuple (round#, (sender ID, host name)); no longer needed, since non-uniform agreement
//    map<int, map<unsigned long, string> > decisions;
    // tuple (round#, host name)
    map<int, vector<string> > agreements;
    // the generated state after merging some events
    unsigned long esc;
    // the ID of the last merged event
    int escId;

    // for test and debug purpose
    cMessage* failure;
    cMessage* purgerInit;
    cMessage* hostInit;
    cMessage* EHTimeout;
    cMessage* applyTimeout;
    cMessage* ECStart;

    // for data profile
    // Profile Qr
    map<unsigned long, string> Qr_w;
    // Profile Qp
    map<int, string> Qp_w;
    // Profile Qd
    map<int, string> Qd_w;
    // backup of Qd for observation, after moving Qd events to Qa
    map<int, map<unsigned long, string> > Qd_bk;
    // Profile Qd_bk
    map<int, string> Qd_bk_w;
    // number of consensus instances
    int consensus;
    // tuple (client ID, sor_i of the last finished round)
    map<unsigned long, int> sors;
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

    FastSMRNode();
    virtual ~FastSMRNode();

protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);

    virtual void initNeighbors();
    virtual void handleHeartbeat(HBProbe* hb);
    virtual void initNode();

    virtual void handleJoin(InitJoin* initJoin);

    virtual void stageTransfer();

    // coordinate change protocol
    // true - trigger coordinator change, false - do not trigger coordinator change
    virtual bool checkCoord();
    virtual void handleCCQuery(CCQuery* cc);
    virtual void handleCCNAck(CCNAck* ccnack);
    virtual void handleCCQueryReply(FSMRCCQueryReply* ccr);
    virtual void handleCCEnd(FSMRCCEnd* cce);
    virtual void checkLastStates();
    // compute the coordinator candidate
    virtual string coordinator();
    virtual FSMRLastState computeLastState();

    // group reconfiguration protocol
    // true - trigger reconfiguration, false - do not trigger reconfiguration
    virtual bool checkConfig();
    virtual void handleGRQuery(GRQuery* gr);
    virtual void handleGRQueryReply(GRQueryReply* grr);
    virtual void handleGREnd(FSMRGREnd* gre);
    virtual void checkReadies();
    virtual FSMRReady computeReady();

    // event handling protocol
    virtual void receiveEvent(Event* event);
    virtual void collectRoundEvents();
    virtual void deliverCycleEvent(CycleEvent* ce);

    virtual void handleEvent();
    virtual void handleRoundQuery(RoundQuery* query);
    virtual void handleRoundQueryReply(RoundQueryReply* rqr);

    virtual void initConsensus();
    virtual void startPropose(ConsensusQuery* cq);
    virtual void collectProposal(FSMRProposal* proposal);
    virtual void checkProposals(int r);
    virtual map<unsigned long, string> decide(int r);
    virtual void handleDecision(FSMRDecision* decision);
//    virtual void collectAgreement(FSMRAgree* agreement);
//    virtual void checkAgreements(int r);
//    virtual void handleFinal(FSMRFinal* final);
    virtual string roundEToString(int r);
    // apply the first event in Qd by encoding it and then adding it with the second value
    virtual void applyEvent();

    // return the sent message for faking the send-to-self process
    virtual cMessage* broadcast(cMessage* message, map<string, int>& hosts);

    string retrieveSenderInfo();
//    virtual string buildQdForSync();
    // prepare the data by concatenating Qp and decisions for state synchronization in CC and GR
//    virtual string buildQpForSync();
    virtual void decodeQr();
    virtual void decodeQp();
    virtual void decodeQd();
    virtual void decodeQdbk();

    virtual void fail();
};

#endif /* FASTSMRNODE_H_ */
