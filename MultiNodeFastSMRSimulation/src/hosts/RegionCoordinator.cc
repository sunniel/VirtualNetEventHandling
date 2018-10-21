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

#include "RegionCoordinator.h"

Define_Module(RegionCoordinator);

RegionCoordinator::RegionCoordinator() {
    STAGE_NUM = stage::RENDEZVOUS_INIT;
}

RegionCoordinator::~RegionCoordinator() {
    // TODO Auto-generated destructor stub
}

int RegionCoordinator::numInitStages() const {
    return STAGE_NUM;
}

void RegionCoordinator::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
    }
    if (stage == 1) {
        TransportAddress* addr =
                UnderlayConfiguratorAccess().get()->registerEndpoint(
                        getParentModule());
        ipAddress = addr->getIp();
    }
}

string RegionCoordinator::getPublicAddress() {
    return ipAddress.get4().str();
}

void RegionCoordinator::dispatchHandler(cMessage *msg) {
}
