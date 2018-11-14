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

package ned;

import ned.*;

network FastSMRModel
{
    parameters:
        @display("bgb=800,600;bgi=game/Sierra_map1,s");
        int clientSize = default(1);
        volatile double delay @unit(s);
        double dropRate = default(0.1);
        double maxDelay @unit(s);
    submodules:
        client[clientSize]: Client;
        logicComputer[1]: LogicComputer; 
//        {
//            parameters:
//                @display("p=411,45");
//        }
    connections allowunconnected:
}
