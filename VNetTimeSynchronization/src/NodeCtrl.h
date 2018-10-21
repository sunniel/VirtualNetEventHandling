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

#ifndef __FASTSMRSIMULATION_NODECTRL_H_
#define __FASTSMRSIMULATION_NODECTRL_H_

#include <algorithm>
#include <functional>
#include "GatebasedModule.h"
#include "sdbm.h"
#include "CycleEvent_m.h"
#include "RoundQuery_m.h"
#include "RoundQueryReply_m.h"
#include "InitJoin_m.h"
#include "Join_m.h"
#include "ConsensusQuery_m.h"
#include "Proposal_m.h"
#include "Decision_m.h"
#include "Agree_m.h"
#include "Final_m.h"
#include "CCQueryReply_m.h"
#include "LastState.h"
#include "CCEnd_m.h"
#include "GREnd_m.h"
#include "HBResponse_m.h"
#include "HBProbe_m.h"
#include "CCNAck_m.h"
#include "CCQuery_m.h"
#include "GRQuery_m.h"
#include "GRQueryReply_m.h"
#include "Event_m.h"
#include "Update_m.h"
#include "Gossip_m.h"
#include "EventApply_m.h"
#include "UpdateApply_m.h"
#include "Ready.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

using namespace std;
using namespace boost::tuples;
using namespace boost::adaptors;

/**
 * TODO - Generated class
 */
class NodeCtrl : public GatebasedModule
{

private:
    unsigned long gId;
    bool isMember;
    int groupId;

    /*
     * Network connections
     */
    // Group members, a list of tuple (host full name, out-gate Id to the host) with self excluded.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, int> neighbors;
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
    map<string, LastState> lastStates;
    map<string, Ready> readies;

    int rendezvousId;

    /*
     * Timers
     */
    simtime_t lifespan;
//    simtime_t applyCycle;
    simtime_t roundCycle;
    simtime_t cycleStart;
    simtime_t gcCycle;

    /*
     * Rounds
     */
    // for event collection
    // round ID, start from 1, since in collectRoundEvents, round number is increased first before collecting round events
    int round;
    int deliveryRound;
    // record the round of applied events
    int appliedRound;

    /*
     * Message receiving related data structures
     */
//    map<unsigned long, int> NSenders;
    // tuple (client ID, seq_s - r_s) where seq_s - r_s is the difference of start sequence and joining round
    map<unsigned long, int> senders;
    // tuple (sender ID, highest delivered sequence #)
    map<unsigned long, int> JL;
    // tuple (sender ID, start round) senders
    map<unsigned long, int> Nc;

    /*
     * Message sending related data structure
     */
    // tuple (client ID, seq_s - r_s) where seq_s - r_s is the difference of start sequence and joining round
    map<unsigned long, int> recipients;
    // tuple (sender ID, start round) recipients
    map<unsigned long, int> Nr;
    // Rg, msg sending indicators, in tuple (recipient/recipient group ID, recipient(s) end-point (port, name))
//    typedef tuple<map<int, string>, int> tuple_rg;
    map<unsigned long, map<int, string> > Rg;
    // message sending buffer, in tuple (sequence number, message content)
    // TODO Bs may only needed in debug, only sequence is needed to sort all outgoing messages
    map<int, string> Bs;
    // message sequence # for message send
    int sequence;

    /*
     * consensus related data structures
     */
    // data structures for event handling, in tuple (round #, {(sender Id, lowest seq, sor)}) where {(sender Id, lowest seq, sor)} is compressed to string
    map<int, string> toPropose;
    // unprocessed consensus instance, in tuple (round, {(sender Id, lowest seq, sor)}) where {(sender Id, lowest seq, sor)} is compressed to string
    map<int, string> Qu;
    set<int> proposed;
    map<int, string>CQs;
    simsignal_t signal_consensus_counter;
    int consensus_counter;

    /*
     * Event sequence ordering relate data structures
     */
    // tuple (sender ID, tuple (sequence #, event content))
//    map<unsigned long, map<int, string> > Qr;
    typedef tuple<unsigned long, int> tuple_qr;
    map<tuple_qr, string> Qr;
    // tuple (round#, sender ID, sequence number, event content)
    typedef tuple<int, unsigned long, int> tuple_qp;
    map<tuple_qp, string> Qp;
    // tuple (round#, sender ID, sequence number, event content)
    typedef tuple<unsigned long, int, string> tuple_qc;
    multimap<int, tuple_qc> Qc;
    // tuple (round#, sender ID, sequence number, event content))
    typedef tuple<int, unsigned long, int> tuple_qd;
    map<tuple_qd, string> Qd;
    // tuple (round#, sender ID, sequence number))
    tuple<int, unsigned long, int> lastDelivered;
    // message sending queue for each recipient, in tuple (recipient/recipient group ID, sequence #, message)
    typedef tuple<int, string> tuple_qsend;
    map<unsigned long, vector<tuple_qsend> > Qsend;


    // store received proposal, in tuple ((round#, host name), {(sender id, sequence #, event)}), for ordering
    typedef tuple<int, string> tuple_pKey;
    typedef tuple<unsigned long, int> tuple_ppKey;
    map<tuple_pKey, map<tuple_ppKey, string> > proposals;
    // the generated state after merging some events
    unsigned long esc;
    // the ID of the last merged event
    int escId;

    /*
     * For garbage collectoin gossip
     */
    // tuple (host name, last round, send time)
    typedef tuple<int, simtime_t> tuple_h;
    map<string, tuple_h> histories;

    /*
     * Events
     */
    // failure is for test and debug purpose
    cMessage* failure;
    cMessage* purgerInit;
    cMessage* hostInit;
    cMessage* EHTimeout;
    cMessage* updateTimeout;
    cMessage* ECTimeout;
    cMessage* GCTimeout;

    /*
     * for data profile
     */
    // backup of Qd for observation, after moving Qd events to Qa
    map<int, map<unsigned long, string> > Qd_bk;
    // Profile Qd_bk
    map<int, string> Qd_bk_w;
    // number of consensus instances
    int consensus;
    int round_query;
    set<int> query_round;
    simtime_t next_round_time;
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

    NodeCtrl();
    virtual ~NodeCtrl();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void dispatchHandler(cMessage *msg);

    virtual void initNeighbors();
    virtual void handleHeartbeat(HBProbe* hb);
    virtual void initNode();

    virtual void handleJoin(InitJoin* initJoin);

    virtual void stageTransfer();

    /*
     * coordinate reconfiguration protocol
     */
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

    /*
     * group reconfiguration protocol
     */
    // true - trigger reconfiguration, false - do not trigger reconfiguration
    virtual bool checkConfig();
    virtual void handleGRQuery(GRQuery* gr);
    virtual void handleGRQueryReply(GRQueryReply* grr);
    virtual void handleGREnd(GREnd* gre);
    virtual void checkReadies();
    virtual Ready computeReady();

    /*
     * event handling protocol
     */
    virtual void receiveEvent(Event* event);
    virtual void collectRoundEvents();
    virtual void deliverCycleEvent(CycleEvent* ce);

    virtual void handleEvent();
    virtual void handleRoundQuery(RoundQuery* query);
    virtual void handleRoundQueryReply(RoundQueryReply* rqr);

    virtual void initConsensus();
    virtual void startPropose(ConsensusQuery* cq);
    virtual void collectProposal(Proposal* proposal);
    virtual void checkProposals(int r);
    virtual map<tuple<int, unsigned long, int> , string> decide(int r,
            vector<string> neighborList);
    virtual void handleDecision(Decision* decision);
    virtual string roundEToString(int r, string senderSeqRanges);
    // apply the first event in Qd by encoding it and then adding it with the second value
    virtual void applyEvent();
    virtual void applyUpdate(UpdateApply* updateApply);
    virtual void disseminateEvent();

    /*
     * garbage collection protocols
     */
    virtual void gossip();
    virtual void handleGossip(Gossip* gossip);

    // return the sent message for faking the send-to-self process
    virtual cMessage* broadcast(cMessage* message, map<string, int> hosts);
    virtual map<string, int> survivedNeighborMap();
    virtual map<string, int> survivedMemberMap();
    virtual vector<string> survivedMembers();

    virtual void fail();

};

#endif
