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

#ifndef __FASTSMRSIMULATION_NODEAPP_H_
#define __FASTSMRSIMULATION_NODEAPP_H_

#include "SocketRTScheduler.h"
#include "GatebasedModule.h"
#include "sdbm.h"
#include "EventApply_m.h"
#include "UpdateApply_m.h"
#include "Ready.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

using namespace std;
using namespace boost::tuples;

/**
 * TODO - Generated class
 */
class NodeApp: public GatebasedModule {
    struct position {
        double x;
        double y;
    };
private:
    // applied event queue, tuple (event index, event content)
    map<int, string> Qa;
    int lastApplied;
    // states in tuple (state #, state)
    map<int, unsigned long> Qs;
    position p;

    cMessage* hostInit;
protected:
    virtual void initialize();
    virtual void dispatchHandler(cMessage *msg);
    virtual void init();
    virtual void applyEvent(EventApply* event);
    virtual position getPosition();
    virtual void setPosition(double x, double y);
public:
    NodeApp();
    virtual ~NodeApp();
};

#endif
