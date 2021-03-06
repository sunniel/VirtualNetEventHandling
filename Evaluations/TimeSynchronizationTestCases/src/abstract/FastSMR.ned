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

package abstract;

import abstract.*;
import global.*;
import hosts.*;

network FastSMRModel
{
    parameters:
        @display("bgb=800,600;bgi=maps/Sierra_map1,s");
        @statistic[InteractionLatency](source=latency; title="Interaction Latency"; record=stats);
        @statistic[DeliveryQueueLength](source=QdLength; title="Delivery queue length"; record=stats);
        int client_num = default(1);
        double pktLossRate;
        double Copt @unit(mps);
        double BW @unit(bps);
        double MTU @unit(b);
        double jitter_mu;
        double jitter_sigma;
        string churnGeneratorModule;
        string logical_computer;
        double Dmin @unit(s);
        volatile double jitter @unit(s);
    submodules:
        globalObserver: GlobalObserver {
            @display("p=50,50");
        }
        underlayConfigurator: UnderlayConfigurator {
            @display("p=50,150");
        }
        coordinator: Coordinator {
            @display("p=50,250");
        }
        churnGenerator: UniformChurnGenerator {
            @display("p=50,350");
        }
    //        churnGenerator: WeibullChurnGenerator {
    //            @display("p=50,350");
    //        }
    //        churnGenerator: ManualChurnGenerator {
    //            @display("p=50,350");
    //        }
    connections allowunconnected:
}
