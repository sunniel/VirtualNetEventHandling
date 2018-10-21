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

#ifndef PURGER_H_
#define PURGER_H_

#include <map>
#include <iterator>
#include <vector>
#include <omnetpp.h>
#include "Constants.h"
#include "Util.h"

using namespace std;

//
// This module works as a global oracle module to monitor the state of modules and events.
// A module will be removed from a network only if it has failed and all its sent events has
// been dispatched. After removing the module, all gates linking to the module will also be removed.
//
class ModuleState {
public:
    enum State {
        Live, Failed
    };

    string hostName;
    int msgNum;
    State state;

    ModuleState() {
        msgNum = 0;
    }
    ModuleState(string hostName, State state) {
        this->hostName = hostName;
        this->state = state;
        msgNum = 0;
    }
    virtual ~ModuleState() {
        ;
    }
};

class Purger: public cSimpleModule {
private:
    simtime_t purgeCycle;
    // tuple (host name, module state).
    // This map cannot be assigned to WATCH_MAP, which will cause compilation error.
    map<string, ModuleState> hostStates;
    map<string, int> hostMsg;
    map<string, ModuleState::State> hostState;
    cMessage* notification;
public:
    Purger();
    virtual ~Purger();

    void addHost(string hostName) {
        ModuleState ms(hostName, ModuleState::Live);
        hostStates[hostName] = ms;
        hostMsg[hostName] = 0;
        hostState[hostName] = ModuleState::Live;
    }

    void addMsg(string hostName) {
        ModuleState* ms = &hostStates[hostName];
        ms->msgNum++;
        hostMsg[hostName]++;

//        EV << "In purger, add " << hostName << " message to " << ms->msgNum
//                  << endl;
    }

    void removeMsg(string hostName) {
        ModuleState* ms = &hostStates[hostName];
        if (ms->msgNum > 0) {
            ms->msgNum--;
            hostMsg[hostName]--;

//            EV << "In purger, remove " << hostName << " message to "
//                      << ms->msgNum << endl;
        }
    }

    void changeState(string hostName, ModuleState::State state) {
//        ModuleState* ms = &hostStates[hostName];
//        ms->state = state;
        hostStates[hostName].state = state;
        hostState[hostName] = state;

        // change host display state
        if (ev.isGUI()) {
            string name = util::getHostName(hostName);
            int index = util::getHostIndex(hostName);
            cModule* host = getParentModule()->getSubmodule(name.c_str(),
                    index);
            host->getDisplayString().parse(DISPLAY_STR_MOD_HIDDEN);
            for (map<string, ModuleState>::iterator it2 = hostStates.begin();
                    it2 != hostStates.end(); it2++) {
                string moduleName = it2->first;
                name = util::getHostName(moduleName);
                index = util::getHostIndex(moduleName);
                cModule* module = getParentModule()->getSubmodule(name.c_str(),
                        index);
                if (module != NULL && module->hasGate(hostName.c_str())) {
                    cGate* outGate = module->gateHalf(hostName.c_str(),
                            cGate::OUTPUT);
                    if (outGate->isConnected()) {
                        outGate->getDisplayString().parse(
                                DISPLAY_STR_CH_HIDDEN);
                    }
                    cGate* inGate = module->gateHalf(hostName.c_str(),
                            cGate::INPUT);
                    if (inGate->isConnected()) {
                        inGate->getPreviousGate()->getDisplayString().parse(
                                DISPLAY_STR_CH_HIDDEN);
                    }
                }
            }
        }
    }

    ModuleState::State getState(string hostName) {
        ModuleState* ms = &hostStates[hostName];
        return ms->state;
    }

    bool contains(string hostName) {
        if (hostStates.find(hostName) == hostStates.end()) {
            return false;
        }
        return true;
    }

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    // TODO explore this function later
//    virtual bool isModuleBusy(string hostName);
};

Define_Module(Purger);

#endif /* PURGER_H_ */
