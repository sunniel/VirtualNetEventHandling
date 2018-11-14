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

#ifndef READY_H_
#define READY_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class Ready{
public:
    int epoch;
    int groupId;
    vector<string> groupMembers;
    map<int, string> DL;
    int escId;
    long esc;

    map<int, map<unsigned long, string> > Qd;
    map<int, map<unsigned long, string> > Qp;
    map<int, string> Qs;

    Ready();
    virtual ~Ready();
};

#endif /* READY_H_ */
