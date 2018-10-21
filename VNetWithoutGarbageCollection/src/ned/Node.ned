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

module Node
{
    parameters:
        @display("i=block/server");
        volatile double lifespan @unit(s);
        double startup @unit(s);
        // 0 - On, 1 - Off
        int state = default(0);
    gates:
        inout gate @directIn;
    submodules:
        app: NodeApp {
            parameters:
                @display("p=82,82");
        }
        ctrl: NodeCtrl {
            parameters:
                @display("p=176,82");
        }
    connections:
        ctrl.uplink$o --> app.link$i;
        ctrl.uplink$i <-- app.link$o;
        gate$i --> ctrl.downlink$i;
        gate$o <-- ctrl.downlink$o;
}
