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

#include "Rendezvous.h"

Rendezvous::Rendezvous() {
    hostCreated = 0;
}

Rendezvous::~Rendezvous() {
    group.clear();
    disposeTimeouts();
    cancelAndDelete(rendezvousInit);
}

void Rendezvous::initialize() {
    GatebasedModule::initialize();

    cycle = par("cycle");
    simtime_t startup = par("startup");
    rendezvousInit = new cMessage("rendezvous_init");
    scheduleAt(simTime() + startup, rendezvousInit);
    // TODO may move init to class member and destroy it in destructor
    cMessage* init = new cMessage("hb_init");
    scheduleAt(simTime() + cycle + startup, init);

    WATCH(hostCreated);
    WATCH_MAP(group);
    WATCH_MAP(seniorities);
}

void Rendezvous::dispatchHandler(cMessage *msg) {
    if (msg->isName("hb_resp")) {
        HeartbeatResponse* hbr = check_and_cast<HeartbeatResponse*>(msg);
        handleHeartbeatResp(hbr);
//            printTimeouts();

    } else if (msg->isName("hb_timeout")) {
//        printTimeouts();
        Timeout* timeout = check_and_cast<Timeout*>(msg);
        handleTimeout(timeout);

        // add new hosts if the current number of hosts is lower than the threshold
        cModule* parent = this->getParentModule();
        int groupSize = parent->par("groupSize");
        int threshold = parent->par("groupSizeThreshold");
        int size = group.size();
        if (size < threshold) {
            this->addNodes(groupSize - size);
        }
    } else if (msg->isName("hb_init")) {
        // handling hb_init
        handleHeartbeatInit(msg);

        // add new hosts at the beginning
        cModule* parent = this->getParentModule();
        int groupSize = parent->par("groupSize");
        int threshold = parent->par("groupSizeThreshold");
        int size = group.size();
        if (size < threshold) {
            this->addNodes(groupSize - size);
        }
    } else if (msg->isName("rendezvous_init")) {
        initGroup();

        // add self to purger
        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));
        purger->addHost(fullName);
    } else if (msg->isName("REQ_COORD")) {
        string senderName = msg->getSenderModule()->getFullName();
        string coord = coordinator();
        if (hasGate(senderName.c_str())) {
            CoordResponse* coordResp = new CoordResponse("RESP_COORD");
            coordResp->setCoordFullName(coord.c_str());
            sendIdeal(coordResp, senderName,
                    this->gateHalf(senderName.c_str(), cGate::OUTPUT)->getId(),
                    DISPLAY_STR_CH_TEMP);
        }

        delete msg;
    }
}

void Rendezvous::initGroup() {

    EV << fullName << " initialize group member: " << endl;

    // connect all hosts to Rendezvous
    for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
            iter++) {
        cModule* module = iter();
        if (strcmp(module->getModuleType()->getName(), "UnreliableHost") == 0) {

            string hostName = module->getFullName();

            // create gates and add module into survivals list
            group[hostName] = createGates(module);

            // initialize seniority
            seniorities[hostName] = 0;

            // count host creation
            hostCreated++;
        }
    }
}

void Rendezvous::handleHeartbeatResp(HeartbeatResponse* hbr) {
    string host = hbr->getSourceName();
    if (ev.isGUI()) {
        string msg = "Received HBR from " + host;
//        bubble(msg.c_str());
    }
    if (timeouts.count(host)) {
        TimeoutScheduler* timer = timeouts[host];
        timer->timeout = simTime() + cycle;

        Heartbeat *heartbeat = new Heartbeat("heartbeat");
        stringstream sstream;
        for (map<string, int>::iterator it = group.begin(); it != group.end();
                ++it) {
            string moduleName = it->first;
            int seniority = seniorities[moduleName];
            sstream << moduleName << ":" << seniority << ";";
        }
        string members = sstream.str();
        heartbeat->setSurvivals(members.c_str());

        int gateId = group[host];
        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(host.c_str());
        heartbeat->setTimestamp(simTime());
        sendReliable(heartbeat, host, gateId);
        // reset timer
        cancelEvent(timer->event);
        timer->event->setTimeout(timer->timeout);
        scheduleAt(timer->timeout, timer->event);
        // update the monitored timeout value
        timeouts[host] = timer;
    } else {
        EV << "[" << simTime() << "s] " << "Survivals no longer contains "
                  << host << endl;
    }
    // delete the received msg
    delete hbr;
}

void Rendezvous::handleTimeout(Timeout* timeout) {
    EV << "[" << simTime() << "s] " << "timeout: from "
              << timeout->getHostName() << " to " << fullName << endl;
    string host = timeout->getHostName();
    TimeoutScheduler* timer = timeouts[host];
    EV << "[" << simTime() << "s] " << "Rendezvous remove event host: "
              << timer->event->getHostName() << endl;

    if (ev.isGUI()) {
        string msg = "Remove " + host + " from group";
        bubble(msg.c_str());
    }

    group.erase(host);
    timeouts.erase(host);
    // remove seniority
    seniorities.erase(host);
    cancelAndDelete(timer->event);

    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->changeState(host, ModuleState::Failed);
}

void Rendezvous::handleHeartbeatInit(cMessage* msg) {
    EV << "[" << simTime() << "s] " << "Start heartbeat" << endl;
    Heartbeat *heartbeat = new Heartbeat("heartbeat");
    stringstream sstream;
    for (map<string, int>::iterator it = group.begin(); it != group.end();
            ++it) {
        string moduleName = it->first;
        int seniority = seniorities[moduleName];
        sstream << moduleName << ":" << seniority << ";";
    }
    string members = sstream.str();
    heartbeat->setSurvivals(members.c_str());

    for (map<string, int>::iterator it = group.begin(); it != group.end();
            ++it) {
        string moduleName = it->first;
        int gateId = it->second;
        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(moduleName.c_str());
        heartbeat->setTimestamp(simTime());
        sendReliable(heartbeat->dup(), moduleName, gateId);

        // set heartbeat timeout
        TimeoutScheduler* te = new TimeoutScheduler();
        te->timeout = simTime() + cycle;
        Timeout* to = new Timeout("hb_timeout");
        to->setHostName(moduleName.c_str());
        to->setTimeout(te->timeout);
        te->event = to;
//        EV << "[debug] module name: " << moduleName << " event timeout: "
//                  << te->timeout << "s" << endl;
        timeouts[moduleName] = te;
        scheduleAt(te->timeout, te->event);
    }

    // delete the original copy
    delete heartbeat;
    // TODO may not need to delete the pointer here, but in class destructor instead, if init is moved to class member
    delete msg;
}

void Rendezvous::addNodes(int num) {
    EV << "[" << simTime() << "s] create " << num << " new hosts" << endl;
    vector<string> newHosts;
    cModule* parent = getParentModule();

    // increase seniority of existing members
    for (map<string, int>::iterator it = seniorities.begin();
            it != seniorities.end(); it++) {
        it->second++;
    }

    for (int i = 0; i < num; i++) {
        // create a new node
        cModuleType *moduleType = cModuleType::get("unreliable.UnreliableHost");
        // create (possibly compound) module and build its submodules (if any)
        cModule *host = moduleType->create("host", parent, hostCreated + 1,
                hostCreated);
        string hostName = host->getFullName();
        EV << "[" << simTime() << "s] create host: " << hostName << endl;

        // set up parameters, if any
        host->finalizeParameters();

        host->buildInside();

        // create activation message
        host->scheduleStart(simTime());
        host->callInitialize();

        // set up gates and add module into survivals list
        group[hostName] = createGates(host);

        // count host creation
        hostCreated++;

        newHosts.push_back(hostName);

        // assign seniority to new members
        seniorities[hostName] = 0;
    }

    // initialize heartbeat
    Heartbeat *heartbeat = new Heartbeat("heartbeat");
    stringstream sstream;
    for (map<string, int>::iterator it = group.begin(); it != group.end();
            ++it) {
        string moduleName = it->first;
        int seniority = seniorities[moduleName];
        sstream << moduleName << ":" << seniority << ";";
    }
    string members = sstream.str();
    heartbeat->setSurvivals(members.c_str());

    for (size_t i = 0; i < newHosts.size(); i++) {
        string hostName = newHosts[i];

        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(hostName.c_str());
        heartbeat->setTimestamp(simTime());
        sendReliable(heartbeat->dup(), hostName, group[hostName]);

        // set heartbeat timeout
        TimeoutScheduler* te = new TimeoutScheduler();
        te->timeout = simTime() + cycle;
        Timeout* to = new Timeout("hb_timeout");
        to->setHostName(hostName.c_str());
        to->setTimeout(te->timeout);
        te->event = to;
        EV << "[" << simTime() << "s] " << hostName << " join the group"
                  << endl;
        timeouts[hostName] = te;
        scheduleAt(te->timeout, te->event);
    }
    // delete the original copy
    delete heartbeat;
}

string Rendezvous::coordinator() {
    unsigned long coordId = 0;
    // Note: If coordName is empty here. Otherwise, empty seniority will be mistakenly created by map::[] operation
    string coordName;
    for (map<string, int>::iterator it = group.begin(); it != group.end();
            it++) {
        string hostName = it->first;
        unsigned long id = sdbm::encode(it->first.c_str());
        if (coordName.empty()) {
            // To cater for the empty coordName case
            coordId = id;
            coordName = it->first;
        } else if (seniorities[hostName] > seniorities[coordName]
                || (seniorities[hostName] == seniorities[coordName]
                        && id > coordId)) {
            coordId = id;
            coordName = it->first;
        }
    }
    return coordName;
}

void Rendezvous::disposeTimeouts() {

    try {
        for (map<string, TimeoutScheduler*>::iterator it = timeouts.begin();
                it != timeouts.end(); ++it) {
            TimeoutScheduler* timer = it->second;
            cancelAndDelete(timer->event);
            delete timer;
        }
        timeouts.clear();
    } catch (exception& e) {
        EV << e.what() << endl;
    }
}

void Rendezvous::printTimeouts() {
    EV << "[debug] Print timouts" << endl;
    for (map<string, TimeoutScheduler*>::iterator it = timeouts.begin();
            it != timeouts.end(); ++it) {
        EV << "Host name: " << it->first << " timeout: " << it->second->timeout
                  << "s event: " << it->second->event->getHostName() << ":"
                  << it->second->event->getTimeout() << "s" << endl;
    }
}
