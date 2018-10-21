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
    seq = 0;
    start = -1;
    timeoutEvents = 0;
    connect = false;
//    init = false;
    cycleTimeout = NULL;
    connectTimeout = NULL;
    rtEvent = NULL;
    supportTelnet = false;
    STAGE_NUM = stage::CLIENT_INIT;

    cycleTimeout = new cMessage(msg::EVENT_CYCLE);
    connectTimeout = new cMessage(msg::CONNECT_TIMEOUT);
}

Client::~Client() {
    if (cycleTimeout != NULL) {
        cancelAndDelete(cycleTimeout);
    }
    if (connectTimeout != NULL) {
        cancelAndDelete(connectTimeout);
    }
    if (rtEvent != NULL) {
        cancelAndDelete(rtEvent);
    }

    // clear eventTimeouts
//    for (map<string, Timeout*>::iterator it = eventTimeouts.begin();
//            it != eventTimeouts.end(); ++it) {
//        Timeout* timer = it->second;
//        if (timer != NULL) {
//            cancelAndDelete(timer);
//        }
//    }
}

int Client::numInitStages() const {
    return STAGE_NUM;
}

void Client::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();

//        eventRoundTrip = registerSignal("delay");

// initialize node ID
        id = sdbm::encode(fullName);
        EV << "ID of " << fullName << " is: " << id << endl;

        // retrieve parameters and initialize timers
        quorum = par("quorumSize");
        connectCycle = par("connectCycle");
        timeoutCycle = par("eventTimeout");

// initialize hardware socket
        rtScheduler =
                dynamic_cast<SimpleSocketRTScheduler *>(simulation.getScheduler());
        if (rtScheduler) {
            rtEvent = new cMessage(msg::RT_EVENT);
            if (!rtScheduler->initialized()) {
                rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, 4000,
                        &numRecvBytes);
            }
            double xLocation = dblrand() * 800;
            getDisplayString().setTagArg("p", 0, xLocation);
            getDisplayString().setTagArg("p", 1, 20L);
        }

        // initialize variable observers
        WATCH(cycle);
        WATCH(start);
        WATCH(connect);
        WATCH_SET(hosts);
//        WATCH(init);
        WATCH_MAP(trace_buffer);
        WATCH_MAP(trace);
        WATCH(seq);
        WATCH(id);
//        WATCH_MAP(eventTS);
//        WATCH_MAP(updateTS);
//        WATCH(queue);
//        WATCH_MAP(delays);
//        WATCH(timeoutEvents);
    }

    if (stage == 1) {
//        // add self to purger
//        Purger* purger = getPurger();
//        purger->addHost(fullName);

        TransportAddress* addr =
                UnderlayConfiguratorAccess().get()->registerEndpoint(
                        getParentModule());
        ipAddress = addr->getIp();
        // start checking connection
        if (!connect) {
            scheduleAt(simTime() + connectCycle, connectTimeout);
        }
    }
}

void Client::dispatchHandler(cMessage *msg) {

    if (msg->isName(msg::EVENT_TIMEOUT)) {
        Timeout* to = check_and_cast<Timeout*>(msg);
        if (ev.isGUI()) {
            string msg = "Server timeout!";
            bubble(msg.c_str());
        }
        string content = to->getEvent();
        if (eventTimeouts.find(content) != eventTimeouts.end()) {
            cancelAndDelete(to);
            eventTimeouts.erase(content);
        }

        // for result analysis
        timeoutEvents++;

    } else if (msg->isName(msg::JOIN)) {
        handleJoin(msg);
    } else if (msg->isName(msg::EVENT_CYCLE)) {
        handleCycleEvent();
    } else if (msg->isName(msg::UPDATE)) {
        handleUpdate(msg);
    } else if (msg->isName(msg::CONNECT_TIMEOUT)) {
        handleConnect();
    } else if (msg->isName(msg::REPLY_CONNECT)) {
        handleConnectReply(msg);
    } else if (msg == rtEvent) {
        handleSocketEvent();
    }
}

void Client::handleJoin(cMessage* msg) {
    if (start < 0) {
        Join* join = check_and_cast<Join*>(msg);
        senders[join->getSenderId()] = -1;
//            start = join->getJoinTime();
        cycle = join->getCycleLength();
        if (join->getJoinTime() < simTime()) {
            // calculate the time of the next cycle from the given start for first event scheduling
            double current = simTime().dbl();
            double joinTime = join->getJoinTime().dbl();
            double cycles = floor((current - joinTime) / cycle.dbl()) + 1.0;
            // start is the length of interval from the current time
            start = cycles * cycle.dbl() + joinTime;
            EV << "[debug] current time is: " << current << endl;
            EV << "[debug] joinTime time is: " << joinTime << endl;
            EV << "[debug] cycle time is: " << cycle << endl;
            EV << "[debug] cycles time is: " << cycles << endl;
            EV << "[debug] start time is: " << start << endl;
            // calculate the sequence number
            seq += 1 + static_cast<int>(cycles);
        } else {
            // start is the length of interval from the current time
            start = join->getJoinTime();
            seq++;
            EV << "[debug] start time is: " << start << endl;
            EV << "[debug] join sequence is: " << seq << endl;
        }
        scheduleAt(start, cycleTimeout);
    }

    delete msg;
}

void Client::handleCycleEvent() {

    Event* em;
    string event;
    if (queue.empty()) {
        em = new Event(msg::EVENT);
        em->setSenderId(id);
        stringstream ss;
        ss << "User operation " << seq << " from " << fullName;
        event = ss.str();
        em->setContent(event.c_str());
        // sourceName and destName must be set for module purge
        em->setSourceName(fullName);
    } else {
        em = check_and_cast<Event*>(queue.pop());
        event = em->getContent();
    }
    em->setSeq(seq);

    // broadcast the message to all live nodes
    multicast(em, hosts);

    // timeout update reception
//        Timeout* to = new Timeout(msg::EVENT_TIMEOUT);
//        to->setEvent(event.c_str());
//        scheduleAt(simTime() + timeoutCycle, to);
//        eventTimeouts[event] = to;

// Set the timer for sending the next event
//        scheduleAt(simTime() + cycle, cycleTimeout);
    // TODO for test purpose
    if (seq == 3) {
        scheduleAt(simTime() + 2.1 * cycle, cycleTimeout);
    } else {
        scheduleAt(simTime() + cycle, cycleTimeout);
//            scheduleAt(start + seq * cycle, cycleTimeout);
    }

    // for test purpose, record event trace
//        trace[seq] = event;

// buffer sent records for result analysis
//    eventTS[event] = simTime();

    // increase event sequence number
    seq++;
}

void Client::handleUpdate(cMessage* msg) {

    Update* update = check_and_cast<Update*>(msg);
    unsigned long senderId = update->getSenderId();
//        int seq = update->getSeq();
    int index = update->getIndex();
    if (senders[senderId] < index) {
        string content = update->getContent();
        tuple<unsigned long, int> key(senderId, index);
        if (trace_buffer.count(key) > 0) {
            tuple<string, int> value = trace_buffer[key];
            trace_buffer[key] = tuple<string, int>(get<0>(value),
                    get<1>(value) + 1);
        } else {
            tuple<string, int> value(content, 1);
            trace_buffer[key] = value;
        }
        int repetition = get<1>(trace_buffer[key]);
        if (repetition >= quorum) {
            trace[key] = content;
            trace_buffer.erase(key);
            senders[senderId] = index;
        }
    }

//        for (map<int, string>::iterator it = updates.begin();
//                it != updates.end(); it++) {
//            string event = it->second;
//
//            // stop timeout for this event
//            if (eventTimeouts.find(event) != eventTimeouts.end()) {
//                Timeout* to = eventTimeouts[event];
//                cancelAndDelete(to);
//                eventTimeouts.erase(event);
//                EV << "remove event timeout" << endl;
//            }
//
//            // record interaction delay for analysis
//            int index = it->first;
//            if (eventTS.find(event) != eventTS.end()
//                    && trace.find(index) != trace.end()) {
//                simtime_t begin = eventTS[event];
//                simtime_t end = simTime();
//                updateTS[event] = end;
//                double delay = (end - begin).dbl();
//                delays[event] = delay;
//                // emit a signal for result recording
////                emit(eventRoundTrip, delay);
//                //        eventTS.erase(seqNum);
//            }
//        }

    // TODO may not valid to purge history update
//        if (trace.size() > 100) {
//            size_t numToPurge = trace.size() - 100;
//            map<int, string>::iterator itEnd = trace.begin();
//            std::advance(itEnd, numToPurge);
//            trace.erase(trace.begin(), itEnd);
//        }
    delete update;
}

void Client::handleConnect() {

    // start checking connection
    if (!connect) {
        scheduleAt(simTime() + connectCycle, connectTimeout);
    } else {
        TransportAddress* rendAddr =
                UnderlayConfiguratorAccess().get()->createLogicComputer();
        rendezvous = rendAddr->getIp();

        cMessage* connect = new cMessage(msg::REQ_CONNECT);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setSrcAddr(ipAddress);
        udpControlInfo->setDestAddr(rendezvous);
        connect->setControlInfo(udpControlInfo);
        sendIdeal(connect);
    }
}

void Client::handleConnectReply(cMessage* msg) {
    ConnectReply* reply = check_and_cast<ConnectReply*>(msg);
    string hostAddrs = reply->getHostAddrs();

    vector<string> toAdd;
    util::splitString(hostAddrs, ";", toAdd);
    for (size_t i = 0; i < toAdd.size(); i++) {
        string addrStr = toAdd[i];
        IPvXAddress addr(addrStr.c_str());
        hosts.insert(addr);
    }

    // init join
    for (set<IPvXAddress>::iterator iter = hosts.begin(); iter != hosts.end();
            iter++) {
        InitJoin* initJoin = new InitJoin(msg::INIT_JOIN);
        initJoin->setSenderId(id);
        initJoin->setSourceName(fullName);
        initJoin->setSeq(seq);
        initJoin->setJoinTime(simTime());
        IPvXAddress ip = *iter;
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(ip);
        udpControlInfo->setSrcAddr(ipAddress);
        initJoin->setControlInfo(udpControlInfo);
        sendReliable(initJoin);
    }

    delete reply;
}

void Client::handleSocketEvent() {
    // get data from buffer
    string text = string(recvBuffer, numRecvBytes);
    numRecvBytes = 0;

    // assemble and send Telnet packet
    Event* e = new Event(msg::EVENT);
    e->setSourceName(fullName);
    e->setSenderId(id);
    e->setContent(text.c_str());
    queue.insert(e);

    rtScheduler->sendBytes(text.c_str(), strlen(text.c_str()));
}

void Client::finish() {
//    simtime_t current = simTime();
//
//    EV << "number of timeout events: " << timeoutEvents << endl;
//    int untimeouted = 0;
//    // find all unexpired events
//    for (map<string, Timeout*>::iterator it = eventTimeouts.begin();
//            it != eventTimeouts.end(); ++it) {
//        Timeout* timer = it->second;
//        simtime_t timeout = timer->getTimeout();
//        if (timeout - current > timeoutCycle) {
//            untimeouted++;
//        }
//    }
//    double eventNum = static_cast<double>(eventTS.size());
//    // deliver_rate = timely_delivered / total number of events
//    double deliveryRate = (eventNum - timeoutEvents - untimeouted) / eventNum;
//    // add deliver_rate to analysis result
//    recordScalar("Delivery rate", deliveryRate);
}