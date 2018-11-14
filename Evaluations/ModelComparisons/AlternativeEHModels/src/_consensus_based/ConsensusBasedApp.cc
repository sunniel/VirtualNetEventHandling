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

#include "ConsensusBasedApp.h"

Define_Module(ConsensusBasedApp);

ConsensusBasedApp::ConsensusBasedApp() {
    lastApplied = -1;
    initial_state = 0;
    STAGE_NUM = stage::NODE_APP_INIT;
}

ConsensusBasedApp::~ConsensusBasedApp() {
}

int ConsensusBasedApp::numInitStages() const {
    return STAGE_NUM;
}

void ConsensusBasedApp::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();

        WATCH(lastApplied);
        WATCH(initial_state);
        WATCH_MAP(Qa_key);
        WATCH_MAP(Qa);
        WATCH_MAP(Qs);
    }
    if (stage == 1) {
        // do nothing here
    }
}

void ConsensusBasedApp::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::EVENT_APPLY)) {
        ConsensusBasedEventApply* eventApply = check_and_cast<ConsensusBasedEventApply*>(msg);
        applyEvent(eventApply);
    }
}

void ConsensusBasedApp::applyEvent(ConsensusBasedEventApply* eventApply) {
    // deliver event to the application layer
    string content = eventApply->getEvent();
    int index = eventApply->getEventIndex();
    Qa[index] = content;
    boost::tuple<unsigned long, int> key(eventApply->getClientId(),
            eventApply->getSequence());
    Qa_key[index] = key;

    // handle events at application layer
    map<int, string>::iterator it2 = Qa.begin();
    if (lastApplied != -1) {
        it2 = Qa.find(lastApplied);
        it2++;
    }
    for (; it2 != Qa.end(); it2++) {
        int index2 = it2->first;
        lastApplied = index2;
        string event = it2->second;
        boost::tuple<unsigned long, int> eventKey = Qa_key[index2];

        string newState;
        if (Qs.empty()) {
            unsigned long input = sdbm::encode(event.c_str());
            newState = util::longToStr(initial_state) + util::longToStr(input);
        } else {
            map<int, unsigned long>::reverse_iterator last = Qs.rbegin();
            unsigned long lastState = last->second;
            EV << "[Debug] lastState = " << lastState << endl;
            unsigned long input = sdbm::encode(event.c_str());
            EV << "[Debug] input = " << input << endl;
            newState = util::longToStr(lastState) + util::longToStr(input);
            EV << "[Debug] newState = " << newState << endl;
        }
        Qs[index2] = sdbm::encode(newState.c_str());
        unsigned long state = Qs[index2];

        // assign update to the queues in Qsend
        UpdateApply* update = new UpdateApply(msg::UPDATE_APPLY);
        update->setEvent(event.c_str());
        // here, round is used as the event index
        update->setRound(index2);
        update->setClientId(get<0>(eventKey));
        update->setSequence(get<1>(eventKey));
        update->setState(state);
        send(update, gateHalf("link", cGate::OUTPUT));
    }
    delete eventApply;
}

void ConsensusBasedApp::initState(unsigned long state) {
    this->initial_state = state;
    Qs.clear();
    Qa.clear();
    Qa_key.clear();
    lastApplied = -1;
}
