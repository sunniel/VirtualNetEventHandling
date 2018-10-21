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

#include "FairLossChannel.h"

using namespace std;

FairLossChannel::~FairLossChannel() {
    // TODO Auto-generated destructor stub
}

void FairLossChannel::processMessage(cMessage * msg, simtime_t t,
        result_t & result) {
    double threshold = par("dropRate").doubleValue();
    simtime_t delay = par("delay");
    //simtime_t maxDelay = par("maxDelay");
    if (uniform(0, 1) > threshold) {
        EV << "Transmission delay: " << delay << " sec" << endl;
      //  EV << "Maximal delay: " << maxDelay << " sec" << endl;
        // set propagation delay (i.e., message transmission delay)
        //if(delay > maxDelay){
            //delay = maxDelay;
        //}
        result.delay = delay;
    } else {
        // drop the message
        if (dynamic_cast<Event*>(msg)) {
            Event* event = (Event*) (msg);
            EV << "Event [" << event->getFullName() << " ("
                      << event->getSenderId() << ":" << event->getSeq() << ") "
                      << "] from " << event->getSourceName() << " to "
                      << event->getDestName() << " has been dropped" << endl;

            // purge dropped message
            Purger* purger = check_and_cast<Purger*>(
                    getParentModule()->getSubmodule("purger"));
            string senderName = event->getSourceName();
            string arrivalName = event->getDestName();
            // sourceName and destName must be set for module purge
            purger->removeMsg(senderName);
            purger->removeMsg(arrivalName);

            // remove connection
            cGate* senderGate = event->getSenderGate();
            EV << "is connected: " << senderGate->isConnected()<< endl;
            EV << "is sender started with host: " << !util::strStartWith(senderName, "host") << endl;
            EV << "is arrival started with host: " << !util::strStartWith(arrivalName, "host") << endl;
            if (senderGate->isConnected()
                    && (!util::strStartWith(senderName, "host")
                            || !util::strStartWith(arrivalName, "host"))) {
                senderGate->disconnect();
            }
        } else if (dynamic_cast<Update*>(msg)) {
            Update* update = (Update*) (msg);
            EV << "Update [" << update->getFullName() << " ("
                      << update->getSenderId() << ":" << update->getSeq()
                      << ") " << "] from " << update->getSourceName() << " to "
                      << update->getDestName() << " has been dropped" << endl;

            // purge dropped message
            Purger* purger = check_and_cast<Purger*>(
                    getParentModule()->getSubmodule("purger"));
            string senderName = update->getSourceName();
            string arrivalName = update->getDestName();
            // sourceName and destName must be set for module purge
            purger->removeMsg(senderName);
            purger->removeMsg(arrivalName);

            // remove connection
            cGate* senderGate = update->getSenderGate();
            EV << "is connected: " << senderGate->isConnected()<< endl;
            EV << "is sender started with host: " << !util::strStartWith(senderName, "host") << endl;
            EV << "is arrival started with host: " << !util::strStartWith(arrivalName, "host") << endl;
            if (senderGate->isConnected()
                    && (!util::strStartWith(senderName, "host")
                            || !util::strStartWith(arrivalName, "host"))) {
                senderGate->disconnect();
            }
        }
        // set message to be dropped
        result.discard = true;
    }
}

