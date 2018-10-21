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

#include "Purger.h"

Purger::Purger() {
    // TODO Auto-generated constructor stub
}

Purger::~Purger() {
    hostStates.clear();
    hostMsg.clear();
    hostState.clear();
    if(notification != NULL){
        cancelAndDelete(notification);
    }
}

void Purger::initialize() {
    purgeCycle = par("purgeCycle");
    notification = new cMessage(msg::NOTIFICATION);
    simtime_t startup = par("startup");
    scheduleAt(simTime() + purgeCycle + startup, notification);

    WATCH_MAP(hostMsg);
    WATCH_MAP(hostState);
}

void Purger::handleMessage(cMessage *msg) {
    vector<string> toRemove;

    for (map<string, ModuleState>::iterator it = hostStates.begin();
            it != hostStates.end(); it++) {
        string hostName = it->first;
        string name = util::getHostName(hostName);
        int index = util::getHostIndex(hostName);
        ModuleState ms = it->second;

        if (ms.state == ModuleState::Failed) {
            EV << "From purger, host: " << hostName << " is treated as failed."
                      << endl;

            if (ms.msgNum != 0) {
                EV << "However, " << ms.msgNum
                          << " message has not been delivered" << endl;
            } else {
                // delete all gates and connections to the host
                for (map<string, ModuleState>::iterator it2 =
                        hostStates.begin(); it2 != hostStates.end(); it2++) {
                    string fullName = it2->first;
                    string moduleName = util::getHostName(fullName);
                    int moduleIndex = util::getHostIndex(fullName);
                    cModule* module = getParentModule()->getSubmodule(
                            moduleName.c_str(), moduleIndex);
                    if (module != NULL && module->hasGate(hostName.c_str())) {
                        cGate* outGate = module->gateHalf(hostName.c_str(),
                                cGate::OUTPUT);
                        if (outGate->isConnected()) {
                            outGate->disconnect();
                            EV << "out gate from: " << fullName << endl;
                        }
                        cGate* inGate = module->gateHalf(hostName.c_str(),
                                cGate::INPUT);
                        if (inGate->isConnected()) {
                            inGate->getPreviousGate()->disconnect();
                            EV << "in gate from: " << fullName << endl;
                        }
                        module->deleteGate(hostName.c_str());
                    }
                }

                // delete host
                cModule* host = getParentModule()->getSubmodule(name.c_str(),
                        index);
                host->callFinish();
                host->deleteModule();

                EV << "From purger, delete host: " << hostName << endl;
//                if (ev.isGUI()) {
//                    string msg = "delete " + hostName;
//                    bubble(msg.c_str());
//                }

                // remove the host from the list
                toRemove.push_back(hostName);
            }
        }
    }

    for (size_t i = 0; i < toRemove.size(); i++) {
        hostStates.erase(toRemove[i]);
        hostMsg.erase(toRemove[i]);
        hostState.erase(toRemove[i]);
    }
    toRemove.clear();

    scheduleAt(simTime() + purgeCycle, msg);
}

//bool Purger::isModuleBusy(string hostName){
//    int busy = 0;
//
//    for (map<string, ModuleState>::iterator it2 =
//            hostStates.begin(); it2 != hostStates.end(); it2++) {
//        string fullName = it2->first;
//        string moduleName = getHostName(fullName);
//        int moduleIndex = getHostIndex(fullName);
//        cModule* module = getParentModule()->getSubmodule(
//                moduleName.c_str(), moduleIndex);
//        if (module != NULL && module->hasGate(hostName.c_str())) {
//            if(module->gateHalf(hostName.c_str(), cGate::OUTPUT)->getChannel()->isBusy()){
//                busy++;
//            }
//            if(module->gateHalf(hostName.c_str(), cGate::INPUT)->getPreviousGate()->getChannel()->isBusy()){
//                busy++;
//            }
//        }
//    }
//    EV << "Busy of module : " << hostName << " is: " << busy << endl;
//    return (busy != 0);
//}
