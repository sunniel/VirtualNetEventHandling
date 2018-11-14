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

#ifndef __CONSENSUS_BASED_NODECTRL_H_
#define __CONSENSUS_BASED_NODECTRL_H_

#include <algorithm>
#include <functional>
#include "bits/streambuf_iterator.h"
#include "bits/algorithmfwd.h"
#include "../global/UnderlayConfiguratorAccess.h"
#include "../global/GlobalStatisticsAccess.h"
#include "../others/Clock.h"
#include "../common/sdbm.h"
#include "../common/HostBase.h"
#include "../common/Util.h"
#include "../common/MiscWatch.h"
#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"
#include "../messages/CycleEvent_m.h"
#include "../messages/RoundQuery_m.h"
#include "../messages/RoundQueryReply_m.h"
#include "../messages/InitJoin_m.h"
#include "../messages/Join_m.h"
#include "../messages/ConsensusQuery_m.h"
#include "../messages/ConsensusNAck_m.h"
#include "../messages/Proposal_m.h"
#include "../messages/Decision_m.h"
#include "../messages/Final_m.h"
#include "../messages/CCQueryReply_m.h"
#include "../messages/CCEnd_m.h"
#include "../messages/GREnd_m.h"
#include "../messages/HBResponse_m.h"
#include "../messages/HBProbe_m.h"
#include "../messages/CCNAck_m.h"
#include "../messages/CCAck_m.h"
#include "../messages/CCQuery_m.h"
#include "../messages/GRQuery_m.h"
#include "../messages/GRQueryReply_m.h"
#include "../messages/GRNAck_m.h"
#include "../messages/Event_m.h"
#include "../messages/Update_m.h"
#include "../messages/Gossip_m.h"
#include "../messages/ConsensusBasedEventApply_m.h"
#include "../messages/UpdateApply_m.h"
#include "../messages/HBTimeout_m.h"
#include "../messages/ConnectReply_m.h"
#include "../messages/ConfigUpdate_m.h"
#include "../messages/ConsensusAck_m.h"
#include "ConsensusBasedApp.h"
#include "../others/InterfaceTableAccess.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using namespace omnetpp;
using namespace boost;
using namespace boost::tuples;
using namespace boost::adaptors;
using json = nlohmann::json;

class ConsensusBasedCtrl: public HostBase {

private:
    // name of the logical computer
    string LCName;
    /*
     * groupId >=0 for an existing replica,
     * groupId < 0 for a new replica (before joining a group).
     */
    int groupId;

    /*
     * Network connections
     */
    // Group members, a list of tuple (host full name, out-gate Id to the host) with self included.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
//    map<string, int> neighbors;
    set<string> members;
    // for reconfiguration
//    map<string, int> neighborsTemp;
    set<string> membersTemp;
    // The Survivals list, from Rendezvous, self excluded.
//    map<string, int> survivals;
    set<string> survivals;
    // store the seniority of self and neighbors, controlled by Rendezvous
    map<string, int> seniorities;
    // store the address of clients
//    map<string, IPvXAddress> clients;
    map<string, string> clients;

    // the ipv4 address of the coordinator (i.e., group leader)
    string coord;
    // coordinator candidate
    string coordCand;
    /*
     * epoch >=0 for an existing replica,
     * epoch < 0 for a new replica (before joining a group).
     */
    int epoch;
    /*
     * Temporary groupId for group reconfiguration. gid_temp will be copied to groupId
     * only after the new configuration is loaded.
     */
    int gid_temp;
    // the set of <replica address, snapshot> pairs
    map<string, string> CCSnapshots;
    // the set of <replica address, snapshot> pairs
    map<string, string> GRSnapshots;
    set<string> CCAcks;

    IPvXAddress rendezvous;

    /*
     * Timers
     */
//    simtime_t applyCycle;
    simtime_t roundCycle;
    simtime_t cycleStart;
    simtime_t gcCycle;
    simtime_t next_round_time;

    /*
     * Rounds
     */

    /*
     * event receiving related data structures
     */
    // sequences of applied events of sender
    map<unsigned long, int> senderSeqs;
    // record the address of event senders, in (sender ID, sender address)
    map<unsigned long, string> SenderAddrs;

    /*
     * update sending related data structure
     */
    // Recipient groups, msg sending indicators, in tuple (recipient/recipient group ID, recipient(s) address (address, name))
    map<unsigned long, map<string, string> > Rg;

    /*
     * Data structures for event sequence ordering
     */
    // key of events in Qr, in (sender ID, sequence #)
    typedef boost::tuple<unsigned long, int> tuple_qr;
    // event receipt queue, in (key, (event_content, arrival_time))
    map<tuple_qr, boost::tuple<string, simtime_t> > Qr;
    // event delivery queue, in (delivery_sequence, (senderId, event_sequence, event content)
    map<int, boost::tuple<unsigned long, int, string> > Qd;

    /*
     * Data structures for event delivery to the application layer
     */
    int lastDelivered;
    // the applied state from the application layer, with the key corresponding to the key in Qd
    pair<int, unsigned long> state;

    /*
     * Data structure for consensus
     */
    bool idle;
    // store received proposal, in tuple (host name, (event, arrival time)), for ordering
    map<string, map<tuple_qr, boost::tuple<string, simtime_t> > > proposals;
    set<string> consensusAcks;

    /*
     * For garbage collectoin gossip
     */
    // tuple (host, last_index_in_Qd, send_time)
    typedef boost::tuple<int, simtime_t> tuple_h;
    map<string, tuple_h> histories;

    /*
     * Events
     */
    // failure is for test and debug purpose
    cMessage* ECTimeout;
    cMessage* GCTimeout;
    HBTimeout* hbTimeout;

    int STAGE_NUM;

    /*
     * for result recording
     */
    simsignal_t QdLength;

    void handleHeartbeat(HBProbe* hb);
    void handleHeartbeatTimeout(HBTimeout* hbt);
    void initNode(simtime_t starttime);
    // handle new event senders (client or neighbors) incoming
    void handleJoin(InitJoin* initJoin);

    void stateTransfer();

    /*
     * coordinate reconfiguration protocol
     */
    // true - trigger coordinator change, false - do not trigger coordinator change
    bool checkCoord();
    // compute the coordinator candidate
    string coordinator();
    void handleCCQuery(CCQuery* cc);
    string buildCCSnapshot();
    void handleCCNAck(CCNAck* ccnack);
    void handleCCQueryReply(CCQueryReply* ccr);
    void checkCCSnapshos();
    json computeCCSnapshot();
    void handleCCEnd(CCEnd* cce);
    void handleCCAck(CCAck* ccack);
    // for uniform agreement on the end of leader election
    void checkCCAcks();

    /*
     * group reconfiguration protocol
     */
    // true - trigger reconfiguration, false - do not trigger reconfiguration
    bool checkConfig();
    void handleGRQuery(GRQuery* gr);
    string buildGRSnapshot();
    void handleGRNAck(GRNAck* grnack);
    void handleGRQueryReply(GRQueryReply* grr);
    void checkGRSnapshots();
    json computeGRSnapshot();
    void handleGREnd(GREnd* gre);

    /*
     * event handling protocol
     */
    void receiveEvent(Event* event);
    void initConsensus();
    void startPropose(ConsensusQuery* cq);
    void collectProposal(Proposal* proposal);
    void handleConsensusNAck(ConsensusNAck* nack);
    void checkProposals();
    map<int, string> decide();
    void handleDecision(Decision* decision);
    void handleConsensusAck(ConsensusAck* ack);
    // for uniform agreement on the end of leader election
    void checkConsensusAcks();
    // apply the first event in Qd by encoding it and then adding it with the second value
    void applyEvent();
    void applyUpdate(UpdateApply* updateApply);

    /*
     * garbage collection protocols
     */
    void gossip();
    void handleGossip(Gossip* gossip);
    void clearQr();

    // return the sent message for faking the send-to-self process
//    virtual void broadcast(cMessage* message, set<string> hosts);
    /**
     * Send message to the UDP module
     */
//    virtual void sendMsg(cMessage* msg);
//    virtual void sendReliable(cMessage* msg);
    /*
     * Return the intersect of survivals and neighbors
     */
    set<string> survivedMembers();
    IPvXAddress loadIPAddress();

public:
    /*
     * 0 - Event Handling (EH); 1 - Group Reconfiguration (GR); 2 - Coordinator Change (CC);
     * 3 - dormant state (DORMANT, before joining a functional group)
     */
    enum Mode {
        EH, GR, CC, DORMANT
    };
    Mode mode;

    ConsensusBasedCtrl();
    virtual ~ConsensusBasedCtrl();

protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage *msg);
    // syncClock is currently not used in this test set
    virtual void syncClock();
};

#endif
