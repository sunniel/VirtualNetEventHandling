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

#ifndef REND_H_
#define REND_H_

#include "IPAddress.h"
#include "IPvXAddress.h"
#include "SimpleNodeEntry.h"
#include "SimpleInfo.h"
#include "UnderlayConfiguratorAccess.h"
#include "TransportAddress.h"
#include "UDPControlInfo_m.h"
#include "HostBase.h"
#include "sdbm.h"
#include "HBTimeout_m.h"
#include "HBResponse_m.h"
#include "HBProbe_m.h"
#include "ConnectReply_m.h"

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

class Rendezvous: public HostBase {
private:
    // tuple (host full name, out gate Id to the host)
//    map<string, int> group;
    set<IPvXAddress> survivals;
    map<IPvXAddress, string> routingTable;
    // It is better to use pointer in map if the referenced object contains a pointer member.
    // The pointer should be released later.
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, TimeoutScheduler*> timeouts;
    simtime_t cycle;
    // host creation counter
    int hostCreated;
    int groupSize;
    int STAGE_NUM;
    UnderlayConfigurator* configurator;
public:
    Rendezvous();
    virtual ~Rendezvous();

protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage* msg);
    virtual void handleClientConnect(cMessage* msg);
    virtual void handleHeartbeatResp(HBResponse* hbr);
    virtual void handleTimeout(HBTimeout* timeout);

    virtual void addNodes(int num, bool isInit);
    virtual void disposeTimeouts();
};

#endif /* REND_H_ */
