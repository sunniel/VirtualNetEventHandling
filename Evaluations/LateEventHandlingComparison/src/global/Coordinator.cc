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

#include "Coordinator.h"

Define_Module(Coordinator);

Coordinator::Coordinator() {
    STAGE_NUM = 1;
}

Coordinator::~Coordinator() {
    // TODO Auto-generated destructor stub
}

int Coordinator::numInitStages() const {
    return STAGE_NUM;
}

void Coordinator::initialize(int stage) {
    locXMax = par("locXMax");
    locYMax = par("locYMax");
    locXMin = par("locXMin");
    locYMin = par("locYMin");

    LCXMax = par("LCXMax");
    LCYMax = par("LCYMax");
    LCXMin = par("LCXMin");
    LCYMin = par("LCYMin");

    radius = par("percept_radius");

    WATCH_MAP(clients);
}

void Coordinator::handleMessage(cMessage *msg) {
}

ClientInfo& Coordinator::addClient(string clientName) {
    long x = (long) uniform((double) locXMin, (double) locXMax);
    long y = (long) uniform((double) locYMin, (double) locYMax);

    ClientInfo info(clientName, x, y);
    clients[clientName] = info;
    return clients[clientName];
}

void Coordinator::setRendezvous(string clientName, IPvXAddress rendezvous) {
    if (clients.count(clientName) == 0) {
        addClient(clientName);
    }
    clients[clientName].setRendezvous(rendezvous);
}

ClientInfo& Coordinator::getClientInfo(string clientName) {
    if (clients.count(clientName) == 0) {
        addClient(clientName);
    }
    return clients[clientName];
}

set<ClientInfo> Coordinator::getNeighbors(string clientName, long x, long y) {
    set<ClientInfo> neighbors;
    for (map<string, ClientInfo>::iterator it = clients.begin();
            it != clients.end(); it++) {
        ClientInfo client = it->second;
        long clientX = client.getX();
        long clientY = client.getY();
        // exclude the caller itself
        if(clientName.compare(client.getName()) != 0){
            double distance = sqrt(
                    pow((double) (x - clientX), 2)
                            + pow((double) (y - clientY), 2));
            if (distance <= (double) radius) {
                neighbors.insert(client);
            }
        }
    }
    return neighbors;
}

long Coordinator::random_LC_X() {
    return LCXMin;
}

long Coordinator::random_LC_Y() {
    return (long) uniform((double) LCYMin, (double) LCYMax);
}

