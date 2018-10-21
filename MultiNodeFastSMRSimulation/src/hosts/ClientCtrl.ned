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

package hosts;

//
// TODO auto-generated module
//
simple ClientCtrl
{
    parameters:
        @class(Client);
//		@signal[delay](type="double");
//        @statistic[interactDelay](title="Interaction Delay"; source="delay"; record=stats; interpolationmode=none);
        @display("i=block/user");
        //  Minimal time unit can only be second
        double initJoinTime @unit(s);
//        volatile double offset @unit(s);
        int quorumSize;
        double eventTimeout @unit(s);
        double connectCycle @unit(s);
	gates:
        inout udplink;
        inout tcplink;      
}
