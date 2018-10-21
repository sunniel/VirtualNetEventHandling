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

#include "GatebasedModule.h"

GatebasedModule::GatebasedModule() {
    // TODO Auto-generated constructor stub

}

GatebasedModule::~GatebasedModule() {
    // TODO Auto-generated destructor stub
}

void GatebasedModule::initialize() {
    fullName = this->getFullName();
}

void GatebasedModule::multicast(cMessage* message, map<string, int>& hosts) {
    for (map<string, int>::iterator it = hosts.begin(); it != hosts.end();
            ++it) {
        string moduleName = it->first;
        int gateId = it->second;
        transmit(message->dup(), moduleName, gateId);
    }
}

// The function is used by the objects with connections always on
void GatebasedModule::transmit(cMessage* msg, string arrivalHostName,
        int gateid) {
    string sender = fullName;
    cModule* arrivalModule = getModuleByName(arrivalHostName);

    // count message by purger
    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->addMsg(sender);
    if (arrivalModule != NULL) {
        purger->addMsg(arrivalHostName);
    }

    if (arrivalModule != NULL && this->hasGate(arrivalHostName.c_str())
            && this->gate(gateid)->isConnected()) {
        cSimpleModule::send(msg, gateid);
    } else {
        purger->removeMsg(sender);
        if (arrivalModule != NULL) {
            purger->removeMsg(arrivalHostName);
        }
        delete msg;
        return;
    }
}

void GatebasedModule::sendReliable(cMessage* msg, string arrivalHostName,
        int gateid) {
    sendReliable(msg, arrivalHostName, gateid, DISPLAY_STR_CH_HIDDEN);
}

void GatebasedModule::sendReliable(cMessage* msg, string arrivalHostName,
        int gateid, const char* channelDisplayString) {
    string sender = fullName;

    cModule* arrivalModule = getModuleByName(arrivalHostName);

    // count message by purger
    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->addMsg(sender);
    if (arrivalModule != NULL) {
        purger->addMsg(arrivalHostName);
    }

    if (this->hasGate(arrivalHostName.c_str())) {
        cGate* out = this->gateHalf(arrivalHostName.c_str(), cGate::OUTPUT);
        if (!out->isConnected()) {
            // set up a connection
            cModule* remoteModule = getModuleByName(arrivalHostName);
            bool canConnect = createReliableConnection(remoteModule,
                    channelDisplayString);
            if (!canConnect) {
                purger->removeMsg(sender);
                if (arrivalModule != NULL) {
                    purger->removeMsg(arrivalHostName);
                }
                delete msg;
                return;
            }
        }
        cSimpleModule::send(msg, gateid);
    } else {
        purger->removeMsg(sender);
        if (arrivalModule != NULL) {
            purger->removeMsg(arrivalHostName);
        }
        delete msg;
        return;
    }
}

void GatebasedModule::sendIdeal(cMessage* msg, string arrivalHostName,
        int gateid) {
    sendIdeal(msg, arrivalHostName, gateid, DISPLAY_STR_CH_HIDDEN);
}

void GatebasedModule::sendIdeal(cMessage* msg, string arrivalHostName,
        int gateid, const char* channelDisplayString) {
    string sender = fullName;

    cModule* arrivalModule = getModuleByName(arrivalHostName);

    // count message by purger
    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->addMsg(sender);
    if (arrivalModule != NULL) {
        purger->addMsg(arrivalHostName);
    }

    if (this->hasGate(arrivalHostName.c_str())) {
        cGate* out = this->gateHalf(arrivalHostName.c_str(), cGate::OUTPUT);
        if (!out->isConnected()) {
            // set up a connection
            cModule* remoteModule = getModuleByName(arrivalHostName);
            bool canConnect = createIdealConnection(remoteModule,
                    channelDisplayString);
            if (!canConnect) {
                purger->removeMsg(sender);
                if (arrivalModule != NULL) {
                    purger->removeMsg(arrivalHostName);
                }
                delete msg;
                return;
            }
        }
        cSimpleModule::send(msg, gateid);
    } else {
        purger->removeMsg(sender);
        if (arrivalModule != NULL) {
            purger->removeMsg(arrivalHostName);
        }
        delete msg;
        return;
    }
}

void GatebasedModule::sendMsg(cMessage* msg, string arrivalHostName,
        int gateid) {
    string sender = fullName;

    cModule* arrivalModule = getModuleByName(arrivalHostName);

    // count message by purger
    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->addMsg(sender);
    if (arrivalModule != NULL) {
        purger->addMsg(arrivalHostName);
    }

    if (this->hasGate(arrivalHostName.c_str())) {
        cGate* out = this->gateHalf(arrivalHostName.c_str(), cGate::OUTPUT);
        if (!out->isConnected()) {
            // set up a connection
            cModule* remoteModule = getModuleByName(arrivalHostName);
            bool canConnect = createConnection(remoteModule);
            if (!canConnect) {
                purger->removeMsg(sender);
                if (arrivalModule != NULL) {
                    purger->removeMsg(arrivalHostName);
                }
                delete msg;
                return;
            }
        }
        cSimpleModule::send(msg, gateid);
    } else {
        purger->removeMsg(sender);
        if (arrivalModule != NULL) {
            purger->removeMsg(arrivalHostName);
        }
        delete msg;
        return;
    }
}

void GatebasedModule::sendMsg(cMessage* msg, string arrivalHostName,
        const char *gatename, int gateindex) {
    string sender = fullName;

    cModule* arrivalModule = getModuleByName(arrivalHostName);

    // count message by purger
    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->addMsg(sender);
    if (arrivalModule != NULL) {
        purger->addMsg(arrivalHostName);
    }

    if (this->hasGate(arrivalHostName.c_str())) {
        cGate* out = this->gateHalf(arrivalHostName.c_str(), cGate::OUTPUT);
        if (!out->isConnected()) {
            // set up a connection
            cModule* remoteModule = getModuleByName(arrivalHostName);
            bool canConnect = createConnection(remoteModule);
            if (!canConnect) {
                purger->removeMsg(sender);
                if (arrivalModule != NULL) {
                    purger->removeMsg(arrivalHostName);
                }
                delete msg;
                return;
            }
        }
        cSimpleModule::send(msg, gatename, gateindex);
    } else {
        purger->removeMsg(sender);
        if (arrivalModule != NULL) {
            purger->removeMsg(arrivalHostName);
        }
        delete msg;
        return;
    }
}

void GatebasedModule::sendMsg(cMessage* msg, string arrivalHostName,
        cGate *outputgate) {
    string sender = fullName;

    cModule* arrivalModule = getModuleByName(arrivalHostName);

    // count message by purger
    Purger* purger = check_and_cast<Purger*>(
            getParentModule()->getSubmodule("purger"));
    purger->addMsg(sender);
    if (arrivalModule != NULL) {
        purger->addMsg(arrivalHostName);
    }

    if (this->hasGate(arrivalHostName.c_str())) {
        cGate* out = this->gateHalf(arrivalHostName.c_str(), cGate::OUTPUT);
        if (!out->isConnected()) {
            // set up a connection
            cModule* remoteModule = getModuleByName(arrivalHostName);
            bool canConnect = createConnection(remoteModule);
            if (!canConnect) {
                purger->removeMsg(sender);
                if (arrivalModule != NULL) {
                    purger->removeMsg(arrivalHostName);
                }
                delete msg;
                return;
            }
        }
        cSimpleModule::send(msg, outputgate);
    } else {
        purger->removeMsg(sender);
        if (arrivalModule != NULL) {
            purger->removeMsg(arrivalHostName);
        }
        delete msg;
        return;
    }
}

void GatebasedModule::handleMessage(cMessage* msg) {

    string senderName = msg->getSenderModule()->getFullName();
    string hostName = msg->getArrivalModule()->getFullName();

    if (this->hasGate(senderName.c_str())
            && this->gateHalf(senderName.c_str(), cGate::INPUT)->isConnected()
            && (!util::strStartWith(senderName, "host")
                    || !util::strStartWith(hostName, "host"))) {
//        EV << hostName << " remove connect with " << senderName << endl;
        this->gateHalf(senderName.c_str(), cGate::INPUT)->getPreviousGate()->disconnect();
    }

    // non-self message
    if (strcmp(hostName.c_str(), senderName.c_str()) != 0) {
//        EV << "In the base class, full name of the host: " << hostName << endl;
//        EV << "In the base class, full name of the sender: " << senderName
//                  << endl;

        Purger* purger = check_and_cast<Purger*>(
                getParentModule()->getSubmodule("purger"));

        purger->removeMsg(hostName);
        purger->removeMsg(senderName);
    } else {
//        EV << fullName << " does not have arrival module" << endl;
    }

    dispatchHandler(msg);
}

bool GatebasedModule::createConnection(cModule* module) {
    if (module != NULL) {
        string moduleFullName = module->getFullName();
        cGate* localOutGate = this->gateHalf(moduleFullName.c_str(),
                cGate::OUTPUT);
        cGate* remoteInGate = module->gateHalf(fullName, cGate::INPUT);
        if (!localOutGate->isConnected()) {
            cChannelType *channelType = cChannelType::get(CH_TYPE_FAIR_LOSS);
            cChannel* flChannel = channelType->create("channel");
            localOutGate->connectTo(remoteInGate, flChannel);
            flChannel->getDisplayString().parse(DISPLAY_STR_CH_TEMP);
        }
        return true;
    } else {
        return false;
    }
}

bool GatebasedModule::createReliableConnection(cModule* module,
        const char* channelDisplayString) {
    if (module != NULL) {
        string moduleFullName = module->getFullName();
        cGate* localOutGate = this->gateHalf(moduleFullName.c_str(),
                cGate::OUTPUT);
        cGate* remoteInGate = module->gateHalf(fullName, cGate::INPUT);
        cChannelType *channelType = cChannelType::get(CH_TYPE_FAIR_LOSS);
        if (!localOutGate->isConnected()) {
            cChannel* flChannel = channelType->create("channel");
            localOutGate->connectTo(remoteInGate, flChannel);
            flChannel->par("dropRate") = 0.0;
            flChannel->getDisplayString().parse(channelDisplayString);
        }
        return true;
    } else {
        return false;
    }
}

bool GatebasedModule::createIdealConnection(cModule* module,
        const char* channelDisplayString) {
    if (module != NULL) {
        string moduleFullName = module->getFullName();
        cGate* localOutGate = this->gateHalf(moduleFullName.c_str(),
                cGate::OUTPUT);
        cGate* remoteInGate = module->gateHalf(fullName, cGate::INPUT);
        if (!localOutGate->isConnected()) {
            cIdealChannel* flChannel = cIdealChannel::create("channel");
            localOutGate->connectTo(remoteInGate, flChannel);
            flChannel->getDisplayString().parse(channelDisplayString);
        }
        return true;
    } else {
        return false;
    }
}

int GatebasedModule::createGates(cModule* module) {
    string moduleFullName = module->getFullName();
    if (!this->hasGate(moduleFullName.c_str())) {
        this->addGate(moduleFullName.c_str(), cGate::INOUT, false);
    }
    if (!module->hasGate(fullName)) {
        module->addGate(fullName, cGate::INOUT, false);
    }
    return this->gateHalf(module->getFullName(), cGate::OUTPUT)->getId();
}

cModule* GatebasedModule::getModuleByName(string moduleName) {
    string localName = util::getHostName(moduleName);
    int index = util::getHostIndex(moduleName);
    return getParentModule()->getSubmodule(localName.c_str(), index);
}
