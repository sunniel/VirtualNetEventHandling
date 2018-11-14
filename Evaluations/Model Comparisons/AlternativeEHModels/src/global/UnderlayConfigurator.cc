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

#include "UnderlayConfigurator.h"

Define_Module(UnderlayConfigurator);

UnderlayConfigurator::UnderlayConfigurator() {
    lcCreated = 0;
    clientCreated = 0;
}

UnderlayConfigurator::~UnderlayConfigurator() {
    disposeFailures();
}

int UnderlayConfigurator::numInitStages() const {
    return 1;
}

void UnderlayConfigurator::initialize(int stage) {
    nextFreeAddress = 0x1000001;
    globalNodeList = GlobalNodeListAccess().get();
    parameterList = GlobalParametersAccess().get();

    WATCH_MAP(death_schedule);

    // initialize clients
    cMessage* clientInit = new cMessage(msg::CLIENT_INIT);
    scheduleAt(0, clientInit);
}

void UnderlayConfigurator::handleMessage(cMessage* msg) {
    if (msg->isName(msg::FAILURE)) {
        Failure* failure = check_and_cast<Failure*>(msg);
        string hostAddress = failure->getHostAddress();
        EV << "[" << simTime() << "s] " << hostAddress << " is failed" << endl;
        IPvXAddress ipAddress(hostAddress.c_str());
        removeNode(ipAddress);
    } else if (msg->isName(msg::CLIENT_INIT)) {
        handleClientInit(msg);
    }
}

void UnderlayConfigurator::handleClientInit(cMessage* msg) {
    // initialize clients
    int clientNum = getSimulation()->getModuleByPath("Model")->par(
            "client_num");
    const char* clientType = par("clientType");
    const char* clientName = par("clientName");
    for (int i = 0; i < clientNum; i++) {
        // create a new client
        cModuleType *moduleType = cModuleType::get(clientType);
        cModule* parent = getParentModule();
        // create (possibly compound) module and build its submodules (if any)
        cModule* client = moduleType->create(clientName, parent,
                clientCreated + 1, clientCreated);
        clientCreated++;
        // set up parameters, if any
        client->finalizeParameters();
        client->buildInside();
        // create activation message
        client->scheduleStart(simTime());
        client->callInitialize(0);
        client->callInitialize(1);
        Coordinator* coordinator = CoordinatorAccess().get();
        ClientInfo info = coordinator->addClient(client->getFullName());
        // initialize the client location
        client->getDisplayString().setTagArg("p", 0, info.getX());
        client->getDisplayString().setTagArg("p", 1, info.getY());
    }
    delete msg;
}

TransportAddress* UnderlayConfigurator::createLogicComputer() {
    const char* hostType = par("lcType");
    const char* hostName = par("lcName");

    // create a new node
    cModuleType *moduleType = cModuleType::get(hostType);
    cModule* parent = getParentModule();
    // create (possibly compound) module and build its submodules (if any)
    cModule *LC = moduleType->create(hostName, parent, lcCreated + 1,
            lcCreated);
    lcCreated++;
    // set up parameters, if any
    LC->finalizeParameters();
    LC->buildInside();
    // create activation message
    LC->scheduleStart(simTime());
    LC->callInitialize(0);

    // create address for the Rendezvous node
    IPvXAddress addr = IPAddress(nextFreeAddress++);
    cModule* rendezvous = LC->getSubmodule("rendezvous");
    HostBase* rendezvousCtrl = check_and_cast<HostBase*>(
            rendezvous->getSubmodule("ctrl"));
    rendezvousCtrl->setIPAddress(addr);
    LC->callInitialize(1);

    // place the logical computer
    long x = CoordinatorAccess().get()->random_LC_X();
    long y = CoordinatorAccess().get()->random_LC_Y();
    LC->getDisplayString().setTagArg("p", 0, x);
    LC->getDisplayString().setTagArg("p", 1, y);

    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(rendezvous);
    SimpleInfo* info = new SimpleInfo(rendezvous->getId());
    info->setEntry(entry);
    //add host to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    TransportAddress *address = new TransportAddress(addr);
    return address;
}

TransportAddress* UnderlayConfigurator::registerEndpoint(cModule* host) {
    IPvXAddress addr = IPAddress(nextFreeAddress++);
    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(host);
    SimpleInfo* info = new SimpleInfo(host->getId());
    info->setEntry(entry);
    //add node to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    TransportAddress *address = new TransportAddress(addr);
    return address;
}

TransportAddress* UnderlayConfigurator::createNode(cModule* parent,
        int nodeCreated) {
    Enter_Method_Silent("Create a new node");

    const char* hostType = par("nodeType");
    const char* hostName = par("nodeName");

    // create a new node
    cModuleType *moduleType = cModuleType::get(hostType);
    // create (possibly compound) module and build its submodules (if any)
    cModule *host = moduleType->create(hostName, parent, nodeCreated + 1,
    nodeCreated);
    string nodeName = host->getFullName();
    EV << "[" << simTime() << "s] create host: " << nodeName << endl;

    // set up parameters, if any
    host->finalizeParameters();
    host->buildInside();
    // create activation message
    host->scheduleStart(simTime());
    host->callInitialize(0);

    IPvXAddress addr = IPAddress(nextFreeAddress++);
    IPv4InterfaceData* ipData = new IPv4InterfaceData();
    ipData->setIPAddress(addr.get4());
    InterfaceEntry* ie = new InterfaceEntry();
    string IPInterface = host->par("IPInterface");
    ie->setName(IPInterface.c_str());
    ie->setIPv4Data(ipData);
    InterfaceTable* ift = InterfaceTableAccess().get(host);
    ift->addInterface(ie);
    host->callInitialize(1);
    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(host);
    SimpleInfo* info = new SimpleInfo(host->getId());
    info->setEntry(entry);
    //add node to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    // time for node failure
    Failure* failure = new Failure(msg::FAILURE);
    string addrStr = addr.get4().str();
    failure->setHostAddress(addrStr.c_str());
    failures[addrStr] = failure;
    simtime_t lifespan = ChurnGeneratorAccess().get()->getSessionLength();
    simtime_t deathTime = simTime() + lifespan;
    scheduleAt(deathTime, failure);
    death_schedule[addrStr] = deathTime;
    TransportAddress *address = new TransportAddress(addr);
    return address;
}

void UnderlayConfigurator::removeNode(IPvXAddress& nodeAddr) {
    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            nodeAddr));
    if (info != nullptr) {
        SimpleNodeEntry* destEntry = info->getEntry();
        cModule* node = destEntry->getUdpIPv4Gate()->getOwnerModule();
        node->callFinish();
        node->deleteModule();
        globalNodeList->killPeer(nodeAddr);
        parameterList->remoteHost(nodeAddr);
        string addr = nodeAddr.get4().str();
        Failure* failure = failures[addr];
        cancelAndDelete(failure);
        failures.erase(nodeAddr.get4().str());
    }
}

void UnderlayConfigurator::removeLogicComputer(IPvXAddress& rendezvousAddr) {
    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            rendezvousAddr));
    SimpleNodeEntry* destEntry = info->getEntry();
    cModule* LC =
            destEntry->getUdpIPv4Gate()->getOwnerModule()->getParentModule()->getParentModule();
    LC->callFinish();
    LC->deleteModule();
    globalNodeList->killPeer(rendezvousAddr);
    parameterList->remoteHost(rendezvousAddr);
}

void UnderlayConfigurator::revokeNode(IPvXAddress& nodeAddr) {
    Enter_Method_Silent("Revoke a node");

    string hostAddress = nodeAddr.get4().str();
    Failure* failure = failures[hostAddress];
    if (failure != NULL) {
        cancelEvent(failure);
    }
    scheduleAt(simTime(), failure);
}

void UnderlayConfigurator::disposeFailures() {
    try {
        for (map<string, Failure*>::iterator it = failures.begin();
                it != failures.end(); ++it) {
            Failure* failure = it->second;
            if (failure != NULL) {
                cancelAndDelete(failure);
            }
        }
        failures.clear();
    } catch (exception& e) {
        EV << e.what() << endl;
    }
}
