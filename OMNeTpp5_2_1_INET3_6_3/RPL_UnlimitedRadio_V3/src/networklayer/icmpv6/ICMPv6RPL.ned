//
// Copyright (C) 2005 Andras Varga
//
// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    (3) UPC, Castelldefels, Spain.
//                    INET 3.6.3, adapted for using the RPL routing protocol
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, see <http://www.gnu.org/licenses/>.
//



package rpl.src.networklayer.icmpv6;

//
// ICMPv6 implementation.
//
// To send an ICMPv6 Echo Request, send any message to port pingIn with ~IPv6ControlInfo
// attached. The message will be used as ping payload (encapsulated into an ~ICMPv6Message).
// When an ICMPv6 Echo Reply is received, the payload message will be sent to pingOut.
//
// @see ~IPv6, ~IPv6RoutingTable, ~IPv6NeighbourDiscovery
//#FIXME gate descriptions needed...
//
simple ICMPv6RPL
{
    parameters:
        //EXTRA BEGIN
        @class(rpl::ICMPv6RPL);
        //DAO Parameters
        double DEFAULT_DAO_DELAY @unit(s) = 1 s;
        double DelayDAO @unit(s) = default(DEFAULT_DAO_DELAY);
        volatile double randomDelayDAO @unit(s) = uniform(0.5, 1.5) * DelayDAO; //randomDelayDAO = x~[0,1) * DelayDAO + delay/2 or x~[0.5,1.5) * DelayDAO      
        double ROUTE_INFINITE_LIFETIME @unit(s) = 10000 s;
        double defaultLifeTime @unit(s) = default(ROUTE_INFINITE_LIFETIME); // This feature was only implemented for downward routes by DAO messages. Upward routes uses ROUTE_INFINITE_LIFETIME.
        volatile double randomDefaultLifeTime @unit(s) = uniform(0.5, 0.75) * defaultLifeTime; //randomDefaultLifeTime = x~[0.5,0.75) * lifeTime
        //double ZERO_LIFETIME @unit(s) = 0 s;  // This feature is used for No-Path DAO.
        int DAOheaderLength @unit(byte) = default(0 Byte); //EXTRA
         
 		//DIS Parameters       
        int DISheaderLength @unit(byte) = default(54 Byte);
        
        
        string routingTableModule;  //The path to the RoutingTable module
        //EXTRA END
        string interfaceTableModule;   // The path to the InterfaceTable module
        @display("i=block/control");
    gates:
        input ipv6In;
        output ipv6Out;
        input pingIn[]; // ping requests from app
        output pingOut[];
        output errorOut;  // for errors
        //EXTRA
        input RPLIn;
        output RPLOut;
        
}


