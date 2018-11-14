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

#include "Client.h"

Define_Module(Client);

Client::Client() {
    STAGE_NUM = stage::CLIENT_INIT;

    loadTimeout = new cMessage(msg::LOAD_TIMEOUT);
    neighborDiscovery = new cMessage(msg::NEIGHBOR_DISCOVERY);
}

Client::~Client() {
    if (loadTimeout != NULL) {
        cancelAndDelete(loadTimeout);
    }
    if (neighborDiscovery != NULL) {
        cancelAndDelete(neighborDiscovery);
    }

    disposeTimeouts();
}

void Client::disposeTimeouts() {
    try {
        for (map<string, ClientCycleTimeout*>::iterator it = timeouts.begin();
                it != timeouts.end(); ++it) {
            ClientCycleTimeout* timer = it->second;
            if (timer != NULL) {
                cancelAndDelete(timer);
            }
        }
        timeouts.clear();
    } catch (exception& e) {
        EV << e.what() << endl;
    }
}

int Client::numInitStages() const {
    return STAGE_NUM;
}

void Client::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();

        // initialize node ID
        id = sdbm::encode(fullName);

        // retrieve parameters and initialize timers
        loadCycle = par("loadCycle");
        connectCycle = par("initJoinTime");
        discovery = par("discoveryCycle");

        // initialize variable observers
        WATCH(cycle);
        WATCH_MAP(starts);
        WATCH_MAP(trace);
        WATCH_MAP(seq);
        WATCH(id);
        WATCH_MULTIMAP(replicaMaps);
        WATCH_MAP(rendezvousMaps);

        // initialize neighbor discovery
        scheduleAt(simTime() + discovery, neighborDiscovery);
    }

    if (stage == 1) {
        TransportAddress* addr =
                UnderlayConfiguratorAccess().get()->registerEndpoint(
                        getParentModule());
        ipAddress = addr->getIp();
        // start checking connection
        flag = par("connect");
        scheduleAt(simTime() + loadCycle, loadTimeout);

        interactionLatency = registerSignal("latency");
    }
}

void Client::dispatchHandler(cMessage *msg) {

    if (msg->isName(msg::JOIN)) {
        handleJoin(msg);
    } else if (msg->isName(msg::EVENT_CYCLE)) {
        handleCycleEvent(msg);
    } else if (msg->isName(msg::UPDATE)) {
        handleUpdate(msg);
    } else if (msg->isName(msg::CONFIG_UPDATE)) {
        this->handleConfigUpdate(msg);
    } else if (msg->isName(msg::LOAD_TIMEOUT)) {
        handleLoad();
    } else if (msg->isName(msg::CONNECT_TIMEOUT)) {
        handleConnectTimeout(msg);
    } else if (msg->isName(msg::REPLY_CONNECT)
            || msg->isName(msg::REPLY_CONNECT_NEIGHBOR)) {
        handleConnectReply(msg);
    } else if (msg->isName(msg::NEIGHBOR_DISCOVERY)) {
        handleNeighborDiscovery(msg);
    }
}

void Client::handleNeighborDiscovery(cMessage* msg) {

    Coordinator* coordinator = CoordinatorAccess().get();
    ClientInfo& info = coordinator->getClientInfo(fullName);
    set<ClientInfo> neighbors = coordinator->getNeighbors(fullName, info.getX(),
            info.getY());
    for (set<ClientInfo>::iterator it = neighbors.begin();
            it != neighbors.end(); it++) {
        ClientInfo neighbor = *it;

        if (!neighbor.getRendezvous().isUnspecified()) {
            string name = neighbor.getName();
            if (rendezvousMaps.count(name) == 0) {
                IPvXAddress neighborRendz = neighbor.getRendezvous();

                // connect to the neighbor logical computer
                Connect* connect = new Connect(msg::REQ_CONNECT_NEIGHBOR);
                UDPControlInfo* udpControlInfo = new UDPControlInfo();
                udpControlInfo->setSrcAddr(ipAddress);
                udpControlInfo->setDestAddr(neighborRendz);
                connect->setControlInfo(udpControlInfo);
                sendIdeal(connect);
            }
        }
    }

    scheduleAt(simTime() + discovery, neighborDiscovery);
}

void Client::handleJoin(cMessage* msg) {
    Join* join = check_and_cast<Join*>(msg);
    string LCName = join->getLCName();
    if (starts.count(LCName) > 0 && starts[LCName] < 0) {
        cycle = join->getCycleLength();
        if (join->getJoinTime() < simTime()) {
            // calculate the time of the next cycle from the given start for first event scheduling
            double current = simTime().dbl();
            double joinTime = join->getJoinTime().dbl();
            double cycles = floor((current - joinTime) / cycle.dbl()) + 1.0;
            // start is the length of interval from the current time
            starts[LCName] = cycles * cycle.dbl() + joinTime;
            EV << "[debug] current time is: " << current << endl;
            EV << "[debug] joinTime time is: " << joinTime << endl;
            EV << "[debug] cycle time is: " << cycle << endl;
            EV << "[debug] cycles time is: " << cycles << endl;
            EV << "[debug] start time is: " << starts[LCName] << endl;
            // calculate the sequence number
            seq[LCName] += 1 + static_cast<int>(cycles);
        } else {
            // start is the length of interval from the current time
            starts[LCName] = join->getJoinTime();
            seq[LCName]++;
            EV << "[debug] start time is: " << starts[LCName] << endl;
            EV << "[debug] join sequence is: " << seq[LCName] << endl;
        }
        ClientCycleTimeout* cycleTimeout = new ClientCycleTimeout(
                msg::EVENT_CYCLE);
        cycleTimeout->setLCName(LCName.c_str());
        timeouts[LCName] = cycleTimeout;
        scheduleAt(starts[LCName], cycleTimeout);
    }

    delete join;
}

void Client::handleCycleEvent(cMessage* msg) {
    ClientCycleTimeout* cycleTimeout = check_and_cast<ClientCycleTimeout*>(msg);
    string LCName = cycleTimeout->getLCName();

    Event* em;
    string event;
    em = new Event(msg::EVENT);
    em->setSenderId(id);
    stringstream ss;
    ss << "User operation " << seq[LCName] << " from " << fullName;
    event = ss.str();
    em->setContent(event.c_str());
    // sourceName and destName must be set for module purge
    em->setSourceName(fullName);
    em->setSeq(seq[LCName]);
    // message size: event size + 1 byte for event sequence number
    em->setByteLength(event.size() + 1);

    // broadcast the message to all live nodes
    set<IPvXAddress> replicas;
    pair<multimap<string, IPvXAddress>::iterator,
            multimap<string, IPvXAddress>::iterator> ret =
            replicaMaps.equal_range(LCName);
    for (multimap<string, IPvXAddress>::iterator iter = ret.first;
            iter != ret.second; iter++) {
        replicas.insert(iter->second);
    }
    multicast(em, replicas);
    // record event sending time
    boost::tuple<unsigned long, string, int> event_key(id, LCName, seq[LCName]);
    sentEvents[event_key] = simTime();
    // for event delivery rate calculation
    GlobalStatisticsAccess().get()->addSentEvent(fullName, LCName, seq[LCName], simTime());

    // Set the timer for sending the next event
    bool deviate = par("deviate").boolValue();
    if (deviate) {
        // for changing the event arrival delay
        if (seq[LCName] == 3) {
            scheduleAt(simTime() + 2.1 * cycle, cycleTimeout);
        } else {
            scheduleAt(simTime() + cycle, cycleTimeout);
        }
    } else {
        scheduleAt(simTime() + cycle, cycleTimeout);
    }

    // increase event sequence number
    seq[LCName]++;
}

void Client::handleUpdate(cMessage* msg) {

    Update* update = check_and_cast<Update*>(msg);
    unsigned long senderId = update->getSenderId();
    int sequence = update->getSequence();
    string LCName = update->getLCName();

    boost::tuple<unsigned long, int> event_key(senderId, sequence);
    if (trace.count(event_key) == 0) {
        trace[event_key] = update->getContent();

        // for interaction latency calculation
        boost::tuple<unsigned long, string, int> update_key(senderId, LCName,
                sequence);
        if (sentEvents.count(update_key) > 0) {
            simtime_t sentTime = sentEvents[update_key];
            simtime_t latency = simTime() - sentTime;
            emit(interactionLatency, latency);
            // for event delivery rate calculation
            GlobalStatisticsAccess().get()->addReceivedEvent(fullName, LCName,
                    sequence);
        }
    }

    delete update;
}

void Client::handleConfigUpdate(cMessage* msg) {

    ConfigUpdate* update = check_and_cast<ConfigUpdate*>(msg);
    string LCName = update->getLCName();
    string configList = update->getConfigs();
    set<string> replicas;
    util::convertStrToSet(configList, replicas);
    replicaMaps.erase(LCName);
    for (auto elem : replicas) {
        string replica = elem;
        IPvXAddress ipAddr(replica.c_str());
        util::multimap_insert(replicaMaps, { LCName, ipAddr });
    }

    delete update;
}

void Client::handleLoad() {

    // start checking connection
    flag = par("connect");
    if (!flag) {
        scheduleAt(simTime() + loadCycle, loadTimeout);
        EV << "I am a client~" << endl;
    } else {
        // check whether events are only sent to a specific logical computer or not
        string target = getSimulation()->getModuleByPath("FastSMRModel")->par(
                "logical_computer").stdstringValue();
        if (target.empty()
                || (!target.empty() && target.compare(fullName) == 0)) {
            TransportAddress* rendAddr =
                    UnderlayConfiguratorAccess().get()->createLogicComputer();
            rendezvous = rendAddr->getIp();
            CoordinatorAccess().get()->setRendezvous(fullName, rendezvous);

            Connect* connect = new Connect(msg::REQ_CONNECT);
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setSrcAddr(ipAddress);
            udpControlInfo->setDestAddr(rendezvous);
            connect->setControlInfo(udpControlInfo);
            connect->setLCName((string(fullName) + "-LC").c_str());
            sendIdeal(connect);
        }
    }
}

void Client::handleConnectReply(cMessage* msg) {
    ConnectReply* reply = check_and_cast<ConnectReply*>(msg);
    IPvXAddress srcAddr = check_and_cast<UDPControlInfo *>(
            reply->getControlInfo())->getSrcAddr();

    string LCName = reply->getLCName();
    string hostAddrs = reply->getHostAddrs();

    // to stop the attempt of connection once the connection has already been set up
    if (starts.count(LCName) == 0 || starts[LCName] < 0) {
        if (!hostAddrs.empty()) {
            vector<string> toAdd;
            util::splitString(hostAddrs, ";", toAdd);
            for (size_t i = 0; i < toAdd.size(); i++) {
                string addrStr = toAdd[i];
                IPvXAddress addr(addrStr.c_str());
                util::multimap_insert(replicaMaps, { LCName, addr });
            }
            rendezvousMaps.insert( { LCName, srcAddr });
            starts[LCName] = -1;
            seq[LCName] = 0;
        }

        ConnectTimeout* connectTimeout = new ConnectTimeout(
                msg::CONNECT_TIMEOUT);
        connectTimeout->setLCName(LCName.c_str());
        cancelEvent(connectTimeout);
        scheduleAt(simTime() + connectCycle, connectTimeout);
    }

    delete reply;
}

void Client::handleConnectTimeout(cMessage* msg) {
    ConnectTimeout* connectTimeout = check_and_cast<ConnectTimeout*>(msg);
    string LCName = connectTimeout->getLCName();
    // init join
    pair<multimap<string, IPvXAddress>::iterator,
            multimap<string, IPvXAddress>::iterator> ret =
            replicaMaps.equal_range(LCName);
    for (multimap<string, IPvXAddress>::iterator iter = ret.first;
            iter != ret.second; iter++) {
        InitJoin* initJoin = new InitJoin(msg::INIT_JOIN);
        initJoin->setSenderId(id);
        initJoin->setSourceName(fullName);
        initJoin->setSeq(seq[LCName]);
        initJoin->setJoinTime(simTime());
        initJoin->setLCName(LCName.c_str());
        IPvXAddress ip = iter->second;
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(ip);
        udpControlInfo->setSrcAddr(ipAddress);
        initJoin->setControlInfo(udpControlInfo);
        sendFixedDelay(initJoin);
    }

    delete connectTimeout;
}

void Client::finish() {

}
