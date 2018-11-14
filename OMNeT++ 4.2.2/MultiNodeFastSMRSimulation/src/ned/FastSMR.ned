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
import node.*;
import global.*;
import hosts.*;

network FastSMRModel
{
    parameters:
        //        @display("bgb=800,600;bgi=game/Sierra_map1,s");
        int clientSize = default(1);
        int LCSize = default(0);
        //        volatile double delay @unit(s);
        //        double dropRate = default(0.5);
        //        double maxDelay @unit(s);
        double pktLossRate;
        double Copt @unit(mps);
        double BW @unit(bps);
        double MTU @unit(b);
        double jitter_mu;
        double jitter_sigma;
        string churnGeneratorModule;
    submodules:
        globalObserver: GlobalObserver {
            @display("p=50,50");
        }
        underlayConfigurator: UnderlayConfigurator {
            @display("p=50,150");
        }
        regionCoordinator: RegionCoordinator {
            @display("p=50,250");
        }
        churnGenerator: WeibullChurnGenerator {
            @display("p=50,350");
        }
        client[clientSize]: Client;
        logicComputer[0]: LogicComputer;
    //        {
    //            parameters:
    //                @display("p=411,550");
    //        }
    connections allowunconnected:
}
