//
// Copyright (C) 2007 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file GlobalParameters.cc
 * @author IngmarBaumgart
 */

#include <omnetpp.h>

#include "GlobalParameters.h"

Define_Module(GlobalParameters);

void GlobalParameters::initialize()
{
//    printStateToStdOut = par("printStateToStdOut");

//    if (!ev.isDisabled()) {
//        ev.getOStream().setf(std::ios::fixed, std::ios::floatfield);
//        ev.getOStream().precision(3);
//    }
    maxDist = par("max_distance");
    WATCH_MAP(distances);
}

double GlobalParameters::getDistance(IPvXAddress host1, IPvXAddress host2){
    double distance = 0;
    tuple<IPvXAddress, IPvXAddress> key(host1, host2);
    tuple<IPvXAddress, IPvXAddress> mkey(host2, host1);
    if(distances.count(key) > 0){
        distance = distances[key];
    }
    else if(distances.count(key) > 0){
        distance = distances[mkey];
    }
    else{
        distance = uniform(0, maxDist);
        distances[key] = distance;
    }
    return distance;
}

void GlobalParameters::remoteHost(IPvXAddress host){
    vector<tuple<IPvXAddress, IPvXAddress> > toRemove;
    for( map<tuple<IPvXAddress, IPvXAddress>, double>::iterator it = distances.begin(); it!= distances.end(); it++){
        tuple<IPvXAddress, IPvXAddress> key = it->first;
        if(get<0>(key) == host || get<1>(key) == host){
            toRemove.push_back(key);
        }
    }
    for(size_t i = 0; i < toRemove.size(); i++){
        distances.erase(toRemove[i]);
    }
}
