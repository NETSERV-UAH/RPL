//
// Copyright (C) 2012 Opensim Ltd
// Author: Levente Meszaros
//
// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    OMNeT++ 5.2.1 & INET 3.6.3
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package rpl.src.networklayer.contract;

//
// This module interface provides an abstraction for the upper interface of
// different network layers.
//
moduleinterface INetworkLayerUpperRPL
{
    parameters:
        @display("i=block/fork");
    gates:
        input transportIn[] @labels(ITransportPacket/down);
        output transportOut[] @labels(ITransportPacket/up);
        input pingIn[]@labels(PingPayload/down);
        output pingOut[] @labels(PingPayload/up);
}

//
// This module interface provides an abstraction for the lower interface of
// different network layers.
//
moduleinterface INetworkLayerLowerRPL
{
    parameters:
        @display("i=block/fork");
    gates:
        input ifIn[] @labels(INetworkDatagram/up);
        output ifOut[] @labels(INetworkDatagram/down);
}

//
// This module interface provides an abstraction for the upper and lower
// interfaces of different network layers.
//
moduleinterface INetworkLayerRPL extends INetworkLayerUpperRPL, INetworkLayerLowerRPL
{
    //EXTRA
    gates:
        input RPLIn @labels(RPLmessage/up);
        output RPLOut @labels(RPLmessage/down);
}
