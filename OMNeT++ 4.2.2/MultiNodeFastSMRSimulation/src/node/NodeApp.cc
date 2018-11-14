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

#include "NodeApp.h"

Define_Module(NodeApp);

NodeApp::NodeApp() {
    lastApplied = -1;
    STAGE_NUM = stage::NODE_APP_INIT;
}

NodeApp::~NodeApp() {
}

int NodeApp::numInitStages() const {
    return STAGE_NUM;
}

void NodeApp::initialize(int stage) {
    if (stage == 0) {
        p.x = par("xorigin");
        p.y = par("yorigin");

        WATCH(lastApplied);
        WATCH_MAP(Qa);
        WATCH_MAP(Qs);
    }
    if (stage == 1) {
        SimpleSocketRTScheduler* rtScheduler =
                dynamic_cast<SimpleSocketRTScheduler *>(simulation.getScheduler());
        if (rtScheduler) {
//            setPosition(p.x, p.y);
        }
    }
}

void NodeApp::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::EVENT_APPLY)) {
        EventApply* eventApply = check_and_cast<EventApply*>(msg);
        applyEvent(eventApply);
    }
}

void NodeApp::applyEvent(EventApply* eventApply) {
    // deliver event to the application layer
    string content = eventApply->getEvent();
    int index = 0;
    if (!Qa.empty()) {
        index = Qa.rbegin()->first;
    }
    index++;
    Qa[index] = content;

    // handle events at application layer
    map<int, string>::iterator it2 = Qa.begin();
    if (lastApplied != -1) {
        it2 = Qa.find(lastApplied);
        it2++;
    }
    for (; it2 != Qa.end(); it2++) {
        int index = it2->first;
        lastApplied = index;
        string event = it2->second;

        // apply avatar movement
        if (event.compare(operation::UP) == 0 || event.compare("w") == 0) {
            p.y -= 10;
            setPosition(p.x, p.y);
        } else if (event.compare(operation::LEFT) == 0
                || event.compare("a") == 0) {
            p.x -= 10;
            setPosition(p.x, p.y);
        } else if (event.compare(operation::DOWN) == 0
                || event.compare("s") == 0) {
            p.y += 10;
            setPosition(p.x, p.y);
        } else if (event.compare(operation::RIGHT) == 0
                || event.compare("d") == 0) {
            p.x += 10;
            setPosition(p.x, p.y);
        }

        if (Qs.empty()) {
            Qs[index] = sdbm::encode(event.c_str());
        } else {
            map<int, unsigned long>::reverse_iterator last = Qs.rbegin();
            unsigned long lastState = last->second;
            EV << "[Debug] lastState = " << lastState << endl;
            unsigned long input = sdbm::encode(event.c_str());
            EV << "[Debug] input = " << input << endl;
            string newState = util::longToStr(lastState)
                    + util::longToStr(input);
            EV << "[Debug] newState = " << newState << endl;
            Qs[index] = sdbm::encode(newState.c_str());
        }

        // assign update to the queues in Qsend
        UpdateApply* update = new UpdateApply(msg::UPDATE_APPLY);
        update->setContent(event.c_str());
        send(update, gateHalf("link", cGate::OUTPUT));
    }
    delete eventApply;
}

NodeApp::position NodeApp::getPosition() {
    // modify the logic computer position for visualization purpose
    cModule* parent = getParentModule()->getParentModule();
    position pos;
    string xstr = parent->getDisplayString().getTagArg("p", 0);
    string ystr = parent->getDisplayString().getTagArg("p", 1);
    pos.x = util::strToDouble(xstr);
    pos.y = util::strToDouble(ystr);
    return pos;
}

void NodeApp::setPosition(double x, double y) {
    // modify the logic computer position for visualization purpose
    cModule* parent = getParentModule()->getParentModule();
    position pos = getPosition();
    if (pos.x != x || pos.y != y) {
        parent->getDisplayString().setTagArg("p", 0, x);
        parent->getDisplayString().setTagArg("p", 1, y);
    }
}
