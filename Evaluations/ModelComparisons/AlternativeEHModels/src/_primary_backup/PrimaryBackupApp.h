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

#ifndef __PRIMARY_BACKUP_NODEAPP_H_
#define __PRIMARY_BACKUP_NODEAPP_H_

#include "../common/HostBase.h"
#include "../common/sdbm.h"
#include "../messages/PrimaryBackupEventApply_m.h"
#include "../messages/PrimaryBackupUpdateApply_m.h"
#include "../messages/StateApply_m.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

using namespace std;
using namespace omnetpp;
using namespace boost;
using namespace boost::tuples;

/**
 * TODO - Generated class
 */
class PrimaryBackupApp: public HostBase {
private:
    // applied event queue, tuple (event index, event content)
    map<int, string> Qa;
    // the corresponding keys of the applied events in Qa
    map<int, boost::tuple<int, unsigned long, int> > Qa_key;
    int lastApplied;
    // states in tuple (state #, state)
    map<int, unsigned long> Qs;
    unsigned long initial_state;
    int STAGE_NUM;

protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage *msg);
    virtual void applyEvent(PrimaryBackupEventApply* event);
    virtual void applyState(StateApply* stateApply);
public:
    PrimaryBackupApp();
    virtual ~PrimaryBackupApp();
    void initState(unsigned long state);
};

#endif
