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

#include "FastSMRClient.h"

Define_Module(FastSMRClient);

FastSMRClient::FastSMRClient() {
    seq = 1;
    start = -1;
    timeoutEvents = 0;
}

FastSMRClient::~FastSMRClient() {
    if (clientInit != NULL) {
        cancelAndDelete(clientInit);
    }
    if (cycleTimeout != NULL) {
        cancelAndDelete(cycleTimeout);
    }
    if (initJoinTimeout != NULL) {
        cancelAndDelete(initJoinTimeout);
    }

    // clear eventTimeouts
    for (map<string, Timeout*>::iterator it = eventTimeouts.begin();
            it != eventTimeouts.end(); ++it) {
        Timeout* timer = it->second;
        if (timer != NULL) {
            cancelAndDelete(timer);
        }
    }
}

void FastSMRClient::initialize() {
    GatebasedModule::initialize();

    eventRoundTrip = registerSignal("delay");

    // initialize node ID
    id = sdbm::encode(fullName);
    EV << "ID of " << fullName << " is: " << id << endl;

    quorum = par("quorumSize");
    cycleTimeout = new cMessage(msg::EVENT_CYCLE);
    timeoutCycle = par("eventTimeout");
    clientInit = new cMessage(msg::CLIENT_INIT);
    simtime_t startup = par("startup");
    scheduleAt(simTime() + startup, clientInit);
    simtime_t initJoinTime = par("initJoinTime");
    initJoinTimeout = new cMessage(msg::INIT_JOIN_TIMEOUT);
    scheduleAt(initJoinTime, initJoinTimeout);

    WATCH(cycle);
    WATCH(start);
    WATCH_MAP(trace);
//    WATCH_MAP(trace_buffer_w);
    WATCH(seq);
    WATCH(id);
    WATCH_MAP(eventTS);
    WATCH_MAP(updateTS);
    WATCH_MAP(delays);
    WATCH(timeoutEvents);
}

void FastSMRClient::dispatchHandler(cMessage *msg) {
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
        if (start < 0) {
            Join* join = check_and_cast<Join*>(msg);
//            start = join->getJoinTime();
            cycle = join->getCycleLength();
            if (join->getJoinTime() < simTime()) {
                // calculate the time of the next cycle from the given start for first event scheduling
                double current = simTime().dbl();
                double joinTime = join->getJoinTime().dbl();
                double cycles = floor((current - joinTime) / cycle.dbl()) + 1.0;
                // start is the length of interval from the current time
                start = cycles * cycle.dbl() + joinTime - current;
                EV << "[debug] current time is: " << current << endl;
                EV << "[debug] joinTime time is: " << joinTime << endl;
                EV << "[debug] cycle time is: " << cycle << endl;
                EV << "[debug] cycles time is: " << cycles << endl;
                EV << "[debug] start time is: " << start << endl;
                // calculate the sequence number
                seq += static_cast<int>(cycles);
            } else {
                // start is the length of interval from the current time
                start = join->getJoinTime() - simTime();
            }
            scheduleAt(simTime() + start, cycleTimeout);
        }

        delete msg;
    } else if (msg->isName(msg::EVENT_CYCLE)) {
        Event* em = new Event(msg::EVENT);
        em->setSenderId(id);
        string event;
        if(seq <= 2000){
            em->setSeq(seq);
            stringstream ss;
            ss << "User operation " << seq << " from " << fullName;
            event = ss.str();
            em->setContent(event.c_str());
        }
        else{
            em->setSeq(seq);
            stringstream ss;
            ss << "no more events from " << fullName;
            event = ss.str();
            em->setContent(event.c_str());
        }

        // sourceName and destName must be set for module purge
        em->setSourceName(fullName);

        // broadcast the message to all live nodes
        for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
                iter++) {
            cModule* module = iter();
            if (strcmp(module->getModuleType()->getName(), "FastSMRNode")
                    == 0) {

                string hostName = module->getFullName();
                Purger* purger = check_and_cast<Purger*>(
                        getParentModule()->getSubmodule("purger"));
                if (purger->getState(hostName) == ModuleState::Live) {
                    int gateId = createGates(module);
                    em->setDestName(hostName.c_str());
                    sendMsg(em->dup(), hostName, gateId);
                }
            }
        }
        // timeout update reception
//        Timeout* to = new Timeout(msg::EVENT_TIMEOUT);
//        to->setEvent(event.c_str());
//        scheduleAt(simTime() + timeoutCycle, to);
//        eventTimeouts[event] = to;

        // Set the timer for sending the next event
        simtime_t offset = par("offset");
        simtime_t timer = (cycle + offset) >= 0 ?  (cycle + offset) : 0;
        scheduleAt(simTime() + timer, cycleTimeout);
//        scheduleAt(simTime() + cycle, cycleTimeout);

        // for test purpose, record event trace
//        trace[seq] = event;

        // buffer sent records for result analysis
        if(seq <= 2000){
            eventTS[event] = simTime();
        }

        // increase event sequence number
        seq++;
        delete em;
    } else if (msg->isName(msg::UPDATE)) {
        Update* update = check_and_cast<Update*>(msg);
        string content = update->getContent();
        map<int, string> updates;
        util::convertStrToMap(content, updates);
        for (map<int, string>::iterator it = updates.begin();
                it != updates.end(); it++) {
            int index = it->first;
            string content = it->second;

            if (trace.count(index) == 0) {
                trace[index] = content;
                if(eventTS.count(content) > 0){
                    simtime_t end = simTime();
                    updateTS[content] = end;
                }
            }
        }

//        decodeTraceBuffer();

        for (map<int, string>::iterator it = updates.begin();
                it != updates.end(); it++) {
            string event = it->second;

            // stop timeout for this event
//            if (eventTimeouts.find(event) != eventTimeouts.end()) {
//                Timeout* to = eventTimeouts[event];
//                cancelAndDelete(to);
//                eventTimeouts.erase(event);
//                EV << "remove event timeout" << endl;
//            }

            // record interaction delay for analysis
            int index = it->first;
            if (eventTS.find(event) != eventTS.end()
                    && trace.find(index) != trace.end()) {
                simtime_t begin = eventTS[event];
                simtime_t end = simTime();
//                if(index <= 2000){
//                    updateTS[event] = end;
//                }
                double delay = (end - begin).dbl();
                delays[event] = delay;
                // emit a signal for result recording
                emit(eventRoundTrip, delay);
                //        eventTS.erase(seqNum);
            }
        }

        // TODO may not valid to purge history update
//        if (trace.size() > 100) {
//            size_t numToPurge = trace.size() - 100;
//            map<int, string>::iterator itEnd = trace.begin();
//            std::advance(itEnd, numToPurge);
//            trace.erase(trace.begin(), itEnd);
//        }
        delete update;
    } else if (msg->isName(msg::INIT_JOIN_TIMEOUT)) {
        // init join
        InitJoin* initJoin = new InitJoin(msg::INIT_JOIN);
        initJoin->setSenderId(id);
        initJoin->setSourceName(fullName);
        initJoin->setSeq(seq);
        initJoin->setJoinTime(simTime());
        for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
                iter++) {
            cModule* module = iter();
            if (strcmp(module->getModuleType()->getName(), "FastSMRNode")
                    == 0) {
                this->createGates(module);
                string hostName = module->getFullName();
                Purger* purger = check_and_cast<Purger*>(
                        getParentModule()->getSubmodule("purger"));
                if (purger->getState(hostName) == ModuleState::Live) {
                    int gateId = createGates(module);
                    initJoin->setDestName(hostName.c_str());
                    sendReliable(initJoin->dup(), hostName, gateId);
                }
            }
        }
        delete initJoin;
    } else if (msg->isName(msg::CLIENT_INIT)) {
        // add self to purger
        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));
        purger->addHost(fullName);
    }
}

//void FastSMRClient::decodeTraceBuffer() {
//    trace_buffer_w.clear();
//    for (map<int, map<string, int> >::iterator it = trace_buffer.begin();
//            it != trace_buffer.end(); it++) {
//        unsigned long senderId = it->first;
//        string event = util::convertMapToStr(it->second);
//        trace_buffer_w[senderId] = event;
//    }
//}

void FastSMRClient::finish() {
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
    double eventNum = static_cast<double>(eventTS.size());
    // deliver_rate = timely_delivered / total number of events
//    double deliveryRate = (eventNum - timeoutEvents - untimeouted) / eventNum;

    double deliveryNum = static_cast<double>(updateTS.size());
    double deliveryRate = deliveryNum / eventNum;

    // add deliver_rate to analysis result
    recordScalar("Delivery rate", deliveryRate);
}
