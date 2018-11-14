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

package unreliable;

import common.*;

simple UnreliableHost
{
    parameters:
        @display("i=block/server");
        volatile double lifespan @unit(s);
        double startup @unit(s);
        // 0 - On, 1 - Off
        int state = default(0);
//        double setupTime @unit(s);
}

simple Client
{
    parameters:
        @signal[delay](type="double");
        @statistic[interactDelay](title="Interaction Delay"; source="delay"; record=stats; interpolationmode=none);
        
        @display("i=block/user");
        //  Minimal time unit can only be second
        double eventCycle @unit(s);
        double eventTimeout @unit(s);
        double startup @unit(s);
}

simple Rendezvous
{
    parameters:
        @display("i=block/broadcast");
        double cycle @unit(s);
        double startup @unit(s);
}

//channel FairLossChannel
//{
//    parameters:
//        volatile double delay @unit(s);
//        double dropRate = default(0.5);
//        bool disabled = default(false);
//}

network UnreliableUpdateRedundancyModel
{
    parameters:
        @display("bgb=617,401");
        int groupSize = default(4);
        int groupSizeThreshold = default(2);
        int groupSizeStart;
    types:
//        channel Channel extends FairLossChannel
//        {
//            parameters:
//                dropRate = 0;
//        }
    submodules:
        client[2]: Client {
        }
        host[groupSizeStart]: UnreliableHost {
        }
        rendezvous: Rendezvous {
            @display("p=550,58");
        }
        purger: Purger {
            @display("p=50,50");
        }
    connections allowunconnected:
}
