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
// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    adapted for using on INET 3.6.3, and also included some changes such as using ICMPv6 messages
//                    for transmitting ROL control messages, interface table , and lifesycle modules
//

//EXTRA BEGIN
//package org.mixim.modules.netw;
//import org.mixim.base.modules.BaseNetwLayer;
//simple RPL extends BaseNetwLayer

package rpl.src.routing.rpl;
simple RPLRouting
//EXTRA END
{
    parameters:
        //EXTRA BEGIN
        @class(rpl::RPLRouting);
        @display("i=block/network2");
        string routingTableModule;
        string interfaceTableModule;
        string networkProtocolModule = default("^.networkLayer.ipv6");
        string sinkAddress = default("fe80::8aa:ff:fe00:1");
        double RPLStartTime @unit(s) = default(0.03s);
        //EXTRA END
        
        int DIOheaderLength @unit(byte) = default(73 Byte);
        int DISheaderLength @unit(byte) = default(54 Byte);
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
        double DISStartDelay = default(0);
        double DISIntMin @unit(s) = default(0.03s);
        int DISRedun= default(1);
        int DISIntDoubl= default(0);
        
        
        double GlobalRepairTimer @unit(s) = default(10000 s);
        int MaxNofParents= default(5);
        int NumberofIterations = default(20);
        string FilePath;

 //EXTRA BEGIN       
        //@display("i=block/fork");
        //@class(RPL);
        gates:
            input icmpv6In;
            output icmpv6Out;   

//EXTRA END
}
