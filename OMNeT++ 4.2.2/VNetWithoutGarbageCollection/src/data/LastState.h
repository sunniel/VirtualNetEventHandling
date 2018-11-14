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

#ifndef LASTSTATE_H_
#define LASTSTATE_H_

#include <string>
#include <vector>
#include <map>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

using namespace std;
using namespace boost::tuples;

class LastState{
public:
    int epoch;
    int groupId;
    vector<string> groupMembers;
    map<int, string> DL;
    int escId;
    long esc;

    // tuple (round#, client ID, sequence number, event content))
    map<tuple<int, unsigned long, int>, string> Qd;
    // tuple (round#, sender ID, sequence number, event content)
    map<tuple<int, unsigned long, int>, string> Qp;
    map<int, string> Qs;

    LastState();
    virtual ~LastState();
};

#endif /* LASTSTATE_H_ */
