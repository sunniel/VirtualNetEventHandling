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

package _primary_backup;

import transport.*;
import app.*;
import hosts.*;

module PrimaryBackupClient extends Client
{
    submodules:
        ctrl: PrimaryBackupClientCtrl {
            parameters:
                @display("p=176,80");
        }
    connections:
        udp.uplink$o --> ctrl.udplink$i;
        udp.uplink$i <-- ctrl.udplink$o;
        tcp.uplink$o --> ctrl.tcplink$i;
        tcp.uplink$i <-- ctrl.tcplink$o;
}
