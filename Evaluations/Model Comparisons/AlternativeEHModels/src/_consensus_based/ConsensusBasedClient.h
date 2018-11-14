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

#ifndef __CONSENSUS_BASED_CLIENT_H_
#define __CONSENSUS_BASED_CLIENT_H_

#include <functional>
#include <map>
#include "../common/HostBase.h"
#include "../common/sdbm.h"
#include "../common/Util.h"
#include "../common/MiscWatch.h"
#include "../global/UnderlayConfiguratorAccess.h"
#include "../global/CoordinatorAccess.h"
#include "../global/GlobalStatisticsAccess.h"
#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"
#include "../objects/TransportAddress.h"
#include "../objects/ClientInfo.h"
#include "../messages/Join_m.h"
#include "../messages/InitJoin_m.h"
#include "../messages/Event_m.h"
#include "../messages/Update_m.h"
#include "../messages/Timeout_m.h"
#include "../messages/Connect_m.h"
#include "../messages/ConnectReply_m.h"
#include "../messages/ConnectTimeout_m.h"
#include "../messages/ClientCycleTimeout_m.h"
#include "../messages/ConfigUpdate_m.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

using namespace std;
using namespace omnetpp;
using namespace boost;
using namespace boost::tuples;

/**
 * TODO - Generated class
 */
class ConsensusBasedClient: public HostBase {
private:
    unsigned long id;
    // event sequences to different logical computers, in (logical_computer_name, event_seq)
    map<string, int> seq;
    // tuple (sender Id, update index);
    map<unsigned long, int> senders;
    // tuple (event sender Id, event sequence, update content)
    map<boost::tuple<unsigned long, int>, string> trace;
    bool flag;
//    bool init;
    int STAGE_NUM;

    // remote addresses
    IPvXAddress rendezvous;
    // replicas of the neighbor users' logical computer, in (logical_computer_name, replica_address)
    multimap<string, IPvXAddress> replicaMaps;
    // rendezvous of the neighbor users' logical computer, in (logical_computer_name, replica_address)
    map<string, IPvXAddress> rendezvousMaps;

    // time related variables
    // current local clock, calculated by adding the volatile offset value
    simtime_t clock;
    // (logical computer (name), start time)
    map<string, simtime_t> starts;
    simtime_t discovery;
    simtime_t connectCycle;
    simtime_t loadCycle;
    simtime_t eventCycle;

    map<string, ClientCycleTimeout*> timeouts;
    cMessage* loadTimeout;
    cMessage* neighborDiscovery;

    /*
     * for result recording
     */
    simsignal_t interactionLatency;
    // record the time of sent events, in the form of ((sender_ID, logical_computer_name, event_sequence number), event sent time)
    map<boost::tuple<unsigned long, string, int>, simtime_t> sentEvents;

    void handleJoin(cMessage* msg);
    void handleCycleEvent(cMessage* msg);
    void handleUpdate(cMessage* msg);
    void handleConfigUpdate(cMessage* msg);
    void handleLoad();
    void handleConnectReply(cMessage* msg);
    void handleConnectTimeout(cMessage* msg);
    void handleNeighborDiscovery(cMessage* msg);
    void disposeTimeouts();
protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage *msg);
    virtual void finish();
public:
    ConsensusBasedClient();
    virtual ~ConsensusBasedClient();
    IPvXAddress getRendezvous() {
        return rendezvous;
    }
};

#endif /* __CONSENSUS_BASED_CLIENT_H_ */
