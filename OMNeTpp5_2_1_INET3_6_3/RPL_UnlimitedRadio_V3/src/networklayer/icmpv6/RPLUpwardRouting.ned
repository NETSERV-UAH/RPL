//***************************************************************************
// * file:        RPLRouting.ned
// *
// * author:      Hamidreza Kermajani
// *
// * copyright:   (C) 2013 UPC, Castelldefels, Spain.
// *
// * description: Implementation of the IPv6 Routing Protocol for Low power
// *              and Lossy Networks (RPL).
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// *
// *
// *
// ***************************************************************************
// * last modification: 09/29/2013
// **************************************************************************/

//
// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    (3) UPC, Castelldefels, Spain.
//                    Adapted for using on INET 3.6.3, and also included some changes such as using ICMPv6 messages
//                    for transmitting ROL control messages, interface table , and lifesycle modules
//
// To read more information about the Kermajani's article, you can use [1].
//
//                    [1] Kermajani, Hamidreza, and Carles Gomez. "On the network convergence process
//                    in RPL over IEEE 802.15. 4 multihop networks: Improvement and trade-offs."
//                    Sensors 14.7 (2014): 11993-12022.þ


//EXTRA BEGIN
//package org.mixim.modules.netw;
//import org.mixim.base.modules.BaseNetwLayer;
//simple RPL extends BaseNetwLayer

package rpl.src.networklayer.icmpv6;
simple RPLUpwardRouting
//EXTRA END
{
    parameters:
        //EXTRA BEGIN
        @class(rpl::RPLUpwardRouting);
        @display("i=block/network2");

        double RPLStartTime @unit(s) = default(0.03s);
        string routingTableModule;
        string interfaceTableModule;
        string networkProtocolModule = default("^.networkLayer.ipv6");
        string sinkAddress = default("fe80::8aa:ff:fe00:1");
        int modeOfOperation = default(2);
        //EXTRA END
        
        int DIOheaderLength @unit(byte) = default(73 Byte);
        // debug switch
        //EXTRA BEGIN
        //bool debug = default(false);
        //bool trace = default(false);
     	//bool useSimTracer = default(false);
        // RSSI threshold for route selection
        //double rssiThreshold @unit(dBm) = default(-50 dBm);
     	//EXTRA END

        double DIOIntMin @unit(s) = default(0.008 s);
        int DIORedun= default(10);
        int DIOIntDoubl= default(20);

        bool DISEnable = default(false);
        bool refreshDAORoutes = default(true);
              

 //EXTRA BEGIN       
        //@display("i=block/fork");
        //@class(RPL);
        gates:
            input icmpv6In;
            output icmpv6Out;   

//EXTRA END
}
