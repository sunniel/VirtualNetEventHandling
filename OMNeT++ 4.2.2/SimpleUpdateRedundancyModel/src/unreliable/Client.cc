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
    timeoutEvents = 0;
    seq = 1;
    timeoutEvents = 0;
}

Client::~Client() {
    dests.clear();
    cancelAndDelete(clientInit);
    cancelAndDelete(eventCycle);

    // clear eventTimeouts
    for (map<int, Timeout*>::iterator it = eventTimeouts.begin();
            it != eventTimeouts.end(); ++it) {
        Timeout* timer = it->second;
        if (timer != NULL) {
            cancelAndDelete(timer);
        }
    }
}

void Client::initialize() {
    GatebasedModule::initialize();

    eventRoundTrip = registerSignal("delay");

    // initialize node ID
    id = sdbm::encode(fullName);
    EV << "ID of " << fullName << " is: " << id << endl;

    eventCycle = new cMessage("EVENT_CYCLE");
    timeoutCycle = par("eventTimeout");
    clientInit = new cMessage("client_init");
    simtime_t startup = par("startup");
    scheduleAt(simTime() + startup, clientInit);

    WATCH_MAP(dests);
    WATCH(coord);
    WATCH_MAP(eventTS);
    WATCH_MAP(updateTS);
    WATCH_MAP(delays);
    WATCH(timeoutEvents);
}

void Client::dispatchHandler(cMessage *msg) {
    if (msg->isName("EVENT_TIMEOUT")) {
        Timeout* to = check_and_cast<Timeout*>(msg);
        if (ev.isGUI()) {
            string msg = "Server timeout!";
            bubble(msg.c_str());
        }
        int seqNum = to->getSeq();
        if (eventTimeouts.find(seqNum) != eventTimeouts.end()) {
            cancelAndDelete(to);
            eventTimeouts.erase(seqNum);
        }

        // for result analysis
        timeoutEvents++;

    } else if (msg->isName("RESP_COORD")) {
        CoordResponse* coordResp = check_and_cast<CoordResponse*>(msg);
        coord = coordResp->getCoordFullName();
        if (!coord.empty()) {
            // load coordinator gate ID
            if (hasGate(coord.c_str())) {
                coordId = gateHalf(coord.c_str(), cGate::OUTPUT)->getId();
                // prepare for the next event
                simtime_t next = par("eventCycle");
                scheduleAt(simTime() + next, eventCycle);
            }
        }

        delete coordResp;
    } else if (msg->isName("EVENT_CYCLE")) {
        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));
        if (purger->getState(coord) == ModuleState::Failed) {
            cMessage* requestCoord = new cMessage("REQ_COORD");
            sendIdeal(requestCoord, rendezvous, rendezvousId,
                    DISPLAY_STR_CH_TEMP);
        } else {
            Event* em = new Event("Event");
            em->setSenderId(id);
            em->setSeq(seq);
            stringstream ss;
            ss << "User operation " << seq << " from client " << getIndex();
            em->setContent(ss.str().c_str());
            em->setSourceName(fullName);
            em->setDestName(coord.c_str());
            sendMsg(em, coord, coordId);

            // timeout update reception
            Timeout* to = new Timeout("EVENT_TIMEOUT");
            to->setSeq(seq);
            scheduleAt(simTime() + timeoutCycle, to);
            eventTimeouts[seq] = to;

            eventTS[seq] = simTime();
            seq++;

            // prepare for the next event
            simtime_t next = par("eventCycle");
            scheduleAt(simTime() + next, eventCycle);
        }
    } else if (msg->isName("Update")) {
        Update* update = check_and_cast<Update*>(msg);
        int seqNum = update->getSeq();

        // stop timeout for this event
        if (eventTimeouts.find(seqNum) != eventTimeouts.end()) {
            Timeout* to = eventTimeouts[seqNum];
            cancelAndDelete(to);
            eventTimeouts.erase(seqNum);
        }

        delete msg;

        // record interaction delay for analysis
        simtime_t begin = eventTS[seqNum];
        simtime_t end = simTime();
        updateTS[seqNum] = end;
        double delay = (end - begin).dbl();
        delays[seqNum] = delay;
        // emit a signal for result recording
        emit(eventRoundTrip, delay);
//        eventTS.erase(seqNum);
    } else if (msg->isName("client_init")) {
        // add self to purger
        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));
        purger->addHost(fullName);

        cModule *rendModule = getParentModule()->getSubmodule("rendezvous");
        rendezvous = rendModule->getFullName();
        rendezvousId = createGates(rendModule);

        cMessage* requestCoord = new cMessage("REQ_COORD");
        sendReliable(requestCoord, rendezvous, rendezvousId,
                DISPLAY_STR_CH_TEMP);
    }
}

void Client::finish() {
    simtime_t current = simTime();

    EV << "number of timeout events: " << timeoutEvents << endl;
    int untimeouted = 0;
    // find all unexpired events
    for (map<int, Timeout*>::iterator it = eventTimeouts.begin();
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
