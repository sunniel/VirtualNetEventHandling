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

#include "FastSMRRend.h"

Define_Module(FastSMRRend);

FastSMRRend::FastSMRRend() {
    hostCreated = 0;
    init = NULL;
}

FastSMRRend::~FastSMRRend() {
    group.clear();
    disposeTimeouts();
    if(rendezvousInit != NULL){
        cancelAndDelete(rendezvousInit);
    }
    if(init != NULL){
        cancelAndDelete(init);
    }
}

void FastSMRRend::initialize() {
    GatebasedModule::initialize();

    cycle = par("cycle");
    simtime_t startup = par("startup");
    rendezvousInit = new cMessage(msg::RENDEZVOUS_INIT);
    scheduleAt(simTime() + startup, rendezvousInit);
    init = new cMessage(msg::GROUP_INIT);
    simtime_t groupInit = par("groupInit");
    scheduleAt(simTime() + groupInit + startup, init);

    WATCH(hostCreated);
    WATCH_MAP(group);
}

void FastSMRRend::dispatchHandler(cMessage *msg) {

    if (msg->isName(msg::HEARTBEAT_RESPONSE)) {
        HBResponse* hbr = check_and_cast<HBResponse*>(msg);
        handleHeartbeatResp(hbr);
//            printTimeouts();

    } else if (msg->isName(msg::HEARTBEAT_TIMEOUT)) {
//        printTimeouts();
        HBTimeout* timeout = check_and_cast<HBTimeout*>(msg);
        handleTimeout(timeout);

        // add new hosts if the current number of hosts is lower than the threshold
        cModule* parent = this->getParentModule();
        int groupSize = parent->par("groupSize");
        int threshold = parent->par("groupSizeThreshold");
        int size = group.size();
        if (size < threshold) {
            this->addNodes(groupSize - size, false);
        }
    } else if (msg->isName(msg::GROUP_INIT)){
        // add new hosts at the beginning
        cModule* parent = this->getParentModule();
        int groupSize = parent->par("groupSize");
        int threshold = parent->par("groupSizeThreshold");
//        int groupSize = getAncestorPar("groupSize");
//        int threshold = getAncestorPar("groupSizeThreshold");
        int size = group.size();
        if (size < threshold) {
            addNodes(groupSize - size, true);
        }
    }
    else if (msg->isName(msg::HEARTBEAT_INIT)) {
        // If default group size is not zero, call this function to handle hb_init
//        handleHeartbeatInit(msg);
    } else if (msg->isName(msg::RENDEZVOUS_INIT)) {
        // If default group size is not zero, call this function to initialize Survivals list
        initGroup();

        // add self to purger
        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));
        purger->addHost(fullName);
    }
}

void FastSMRRend::initGroup() {
    EV << fullName << " initialize group member: " << endl;

    // connect all hosts to Rendezvous
    for (cModule::SubmoduleIterator iter(getParentModule()); !iter.end();
            iter++) {
        cModule* module = iter();
        if (strcmp(module->getModuleType()->getName(), "BasicSMRNode") == 0) {

            string hostName = module->getFullName();

            // create gates and add module into survivals list
            group[hostName] = createGates(module);

            // count host creation
            hostCreated++;
        }
    }
}

void FastSMRRend::handleHeartbeatResp(HBResponse* hbr) {

    string host = hbr->getSourceName();

//    if (ev.isGUI()) {
//        string msg = "Received HBR from " + host;
//        bubble(msg.c_str());
//    }

    if (timeouts.find(host) != timeouts.end()) {
        TimeoutScheduler* timer = timeouts[host];
        timer->timeout = simTime() + cycle;

        HBProbe *heartbeat = new HBProbe(msg::HEARTBEAT);
        stringstream sstream;
        for (map<string, int>::iterator it = group.begin(); it != group.end();
                ++it) {
            string moduleName = it->first;
            sstream << moduleName << ";";
        }
        string members = sstream.str();
        heartbeat->setSurvivals(members.c_str());

        int gateId = group[host];
        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(host.c_str());
        heartbeat->setTimestamp(simTime());
        heartbeat->setIsInit(false);
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

void FastSMRRend::handleTimeout(HBTimeout* timeout) {
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
    cancelAndDelete(timer->event);

    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->changeState(host, ModuleState::Failed);
}

void FastSMRRend::handleHeartbeatInit(cMessage* msg) {
    EV << "[" << simTime() << "s] " << "Start heartbeat" << endl;
    HBProbe *heartbeat = new HBProbe(msg::HEARTBEAT);
    stringstream sstream;
    for (map<string, int>::iterator it = group.begin(); it != group.end();
            ++it) {
        string moduleName = it->first;
        sstream << moduleName << ";";
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
        heartbeat->setIsInit(false);
        sendReliable(heartbeat->dup(), moduleName, gateId);

        // set heartbeat timeout
        TimeoutScheduler* te = new TimeoutScheduler();
        te->timeout = simTime() + cycle;
        HBTimeout* to = new HBTimeout(msg::HEARTBEAT_TIMEOUT);
        to->setHostName(moduleName.c_str());
        to->setTimeout(te->timeout);
        te->event = to;
        timeouts[moduleName] = te;
        scheduleAt(te->timeout, te->event);
    }

    // delete the original copy
    delete heartbeat;
}

void FastSMRRend::addNodes(int num, bool isInit) {
    EV << "[" << simTime() << "s] create " << num << " new hosts" << endl;
    vector<string> newHosts;
    cModule* parent = getParentModule();

    for (int i = 0; i < num; i++) {
        // create a new node
        cModuleType *moduleType = cModuleType::get("fastSMR.FastSMRNode");
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
    }

    // initialize heartbeat
    HBProbe *heartbeat = new HBProbe(msg::HEARTBEAT);
    stringstream sstream;
    for (map<string, int>::iterator it = group.begin(); it != group.end();
            ++it) {
        string moduleName = it->first;
        sstream << moduleName << ";";
    }
    string members = sstream.str();
    heartbeat->setSurvivals(members.c_str());

    for (size_t i = 0; i < newHosts.size(); i++) {
        string hostName = newHosts[i];

        heartbeat->setSourceName(fullName);
        heartbeat->setDestName(hostName.c_str());
        heartbeat->setTimestamp(simTime());
        heartbeat->setIsInit(isInit);
        sendReliable(heartbeat->dup(), hostName, group[hostName]);

        // set heartbeat timeout
        TimeoutScheduler* te = new TimeoutScheduler();
        te->timeout = simTime() + cycle;
        HBTimeout* to = new HBTimeout(msg::HEARTBEAT_TIMEOUT);
        to->setHostName(hostName.c_str());
        to->setTimeout(te->timeout);
        te->event = to;
        EV << "[" << simTime() << "s] " << hostName << " join the group"
                  << endl;
        timeouts[hostName] = te;
        scheduleAt(te->timeout, te->event);
    }

    newHosts.clear();

    // delete the original copy
    delete heartbeat;
}

void FastSMRRend::disposeTimeouts() {

    try {
        for (map<string, TimeoutScheduler*>::iterator it = timeouts.begin();
                it != timeouts.end(); ++it) {
            TimeoutScheduler* timer = it->second;
            if(timer != NULL){
                if(timer->event != NULL){
                    cancelAndDelete(timer->event);
                }
                delete timer;
            }
        }
        timeouts.clear();
    } catch (exception& e) {
        EV << e.what() << endl;
    }
}
