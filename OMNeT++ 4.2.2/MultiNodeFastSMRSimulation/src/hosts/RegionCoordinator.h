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

#ifndef REGIONCOORDINATOR_H_
#define REGIONCOORDINATOR_H_

#include "HostBase.h"
#include "IPAddress.h"
#include "IPvXAddress.h"
#include "SimpleNodeEntry.h"
#include "SimpleInfo.h"
#include "UnderlayConfiguratorAccess.h"

class RegionCoordinator: public HostBase {
public:
    RegionCoordinator();
    virtual ~RegionCoordinator();
    string getPublicAddress();
protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage* msg);
private:
    int STAGE_NUM;
};

#endif /* REGIONCOORDINATOR_H_ */
