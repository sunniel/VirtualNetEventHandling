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

#include "BasicSMRClient.h"

Define_Module(BasicSMRClient);

BasicSMRClient::BasicSMRClient() {
    seq = 1;
    timeoutEvents = 0;
}

BasicSMRClient::~BasicSMRClient() {
    if (clientInit != NULL) {
        cancelAndDelete(clientInit);
    }
    if (eventCycle != NULL) {
        cancelAndDelete(eventCycle);
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

void BasicSMRClient::initialize() {
    GatebasedModule::initialize();

    eventRoundTrip = registerSignal("delay");

    // initialize node ID
    id = sdbm::encode(fullName);
    EV << "ID of " << fullName << " is: " << id << endl;

    eventCycle = new cMessage(msg::EVENT_CYCLE);
    timeoutCycle = par("eventTimeout");
    clientInit = new cMessage(msg::CLIENT_INIT);
    simtime_t startup = par("startup");
    scheduleAt(simTime() + startup, clientInit);

    WATCH_MAP(trace);
    WATCH_MAP(eventTS);
    WATCH_MAP(updateTS);
    WATCH_MAP(delays);
    WATCH(timeoutEvents);
}

void BasicSMRClient::dispatchHandler(cMessage *msg) {
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

    } else if (msg->isName(msg::EVENT_CYCLE)) {
        Event* em = new Event(msg::EVENT);
        em->setSenderId(id);
        em->setSeq(seq);
        stringstream ss;
        ss << "User operation " << seq << " from " << fullName;
        string content = ss.str();
        em->setContent(content.c_str());
        // sourceName and destName must be set for module purge
        em->setSourceName(fullName);

        // broadcast the message to all live nodes
        string hostType = par("hostType");
        for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
                iter++) {
            cModule* module = iter();
            if (strcmp(module->getModuleType()->getName(), hostType.c_str())
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
        delete em;

        // timeout update reception
        Timeout* to = new Timeout(msg::EVENT_TIMEOUT);
        to->setEvent(content.c_str());
        scheduleAt(simTime() + timeoutCycle, to);
        eventTimeouts[content] = to;

        // buffer sent records for result analysis
        eventTS[content] = simTime();
        seq++;

        // Set the timer for sending the next event
        simtime_t next = par("eventCycle");
        scheduleAt(simTime() + next, eventCycle);
    } else if (msg->isName(msg::UPDATE)) {
        Update* update = check_and_cast<Update*>(msg);
        // record received updates
        string content = update->getContent();
        map<int, string> updates;
        util::convertStrToMap(content, updates);
        trace.insert(updates.begin(), updates.end());
        // TODO may not valid to purge history update
        if (trace.size() > 100) {
            size_t numToPurge = trace.size() - 100;
            map<int, string>::iterator itEnd = trace.begin();
            std::advance(itEnd, numToPurge);
            trace.erase(trace.begin(), itEnd);
        }

        for (map<int, string>::iterator it = updates.begin();
                it != updates.end(); it++) {
            string event = it->second;

            // stop timeout for this event
            if (eventTimeouts.find(event) != eventTimeouts.end()) {
                Timeout* to = eventTimeouts[event];
                cancelAndDelete(to);
                eventTimeouts.erase(event);
                EV << "remove event timeout" << endl;
            }

            // record interaction delay for analysis
            if (eventTS.find(event) != eventTS.end()) {
                simtime_t begin = eventTS[event];
                simtime_t end = simTime();
                updateTS[event] = end;
                double delay = (end - begin).dbl();
                delays[event] = delay;
                // emit a signal for result recording
                emit(eventRoundTrip, delay);
                //        eventTS.erase(seqNum);
            }
        }

        delete update;
    } else if (msg->isName(msg::CLIENT_INIT)) {
        // add self to purger
        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));
        purger->addHost(fullName);

        // evalute the prepare for the next event
        simtime_t next = par("eventCycle");
        scheduleAt(simTime() + next, eventCycle);
    }
}

void BasicSMRClient::finish() {
    simtime_t current = simTime();

    EV << "number of timeout events: " << timeoutEvents << endl;
    int untimeouted = 0;
    // find all unexpired events
    for (map<string, Timeout*>::iterator it = eventTimeouts.begin();
            it != eventTimeouts.end(); ++it) {
        Timeout* timer = it->second;
        simtime_t timeout = timer->getTimeout();
        if (timeout - current > timeoutCycle) {
            untimeouted++;
        }
    }
    double eventNum = static_cast<double>(eventTS.size());
    // deliver_rate = timely_delivered / total number of events
    double deliveryRate = (eventNum - timeoutEvents - untimeouted) / eventNum;
    // add deliver_rate to analysis result
    recordScalar("Delivery rate", deliveryRate);
}
