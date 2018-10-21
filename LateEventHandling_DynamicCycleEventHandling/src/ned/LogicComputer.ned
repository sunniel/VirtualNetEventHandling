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

module LogicComputer
{
    parameters:
        @display("i=LogicComputer/1ogic_computer_square_vl");
		int groupSize = default(4);
        int groupSizeThreshold = default(2);
        int groupSizeStart = default(0); 
    submodules:
        node[groupSizeStart]: Node;
        rendezvous: Rendezvous {
            @display("p=550,58");
        }
        purger: Purger {
            @display("p=50,50");
        }
    connections allowunconnected:
}
