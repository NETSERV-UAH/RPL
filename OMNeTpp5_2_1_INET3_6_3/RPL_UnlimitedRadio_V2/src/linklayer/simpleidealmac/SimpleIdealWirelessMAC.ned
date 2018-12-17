//*
// * this module was inspired by the CSMA module in the INET framework.
// *
// *
// * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
// *                    (1) GIST, University of Alcala, Spain.
// *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
// *                    Develpoed on OMNeT++ 5.2.1 & INET 3.6.3 for using in the IoTorii protocol
//

//*
// * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
// *                    (1) GIST, University of Alcala, Spain.
// *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
// *                    (3) UPC, Castelldefels, Spain.
//*                    Develpoed on OMNeT++ 5.2.1 & INET 3.6.3 by using their proposed interfaces to adapt for using in the RPL and other wireless protocols
//
//***************************************************************************
//* file:        CSMA.ned
//*
//* author:      Jerome Rousselot, Marc Loebbers
//*
//* copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
//*                  (C) 2004 Telecommunication Networks Group (TKN) at
//*              Technische Universitaet Berlin, Germany.
//*
//*              This program is free software; you can redistribute it
//*              and/or modify it under the terms of the GNU General Public
//*              License as published by the Free Software Foundation; either
//*              version 2 of the License, or (at your option) any later
//*              version.
//*              For further information see file COPYING
//*              in the top level directory
//*
//* Funding: This work was partially financed by the European Commission under the
//* Framework 6 IST Project "Wirelessly Accessible Sensor Populations"
//* (WASP) under contract IST-034963.
//***************************************************************************
//* part of:    Modifications to the MF-2 framework by CSEM
//**************************************************************************/


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

package rpl.src.linklayer.simpleidealmac;

simple SimpleIdealWirelessMAC //extends MACProtocolBase like IMACProtocol  //EXTRA
{
    parameters:
       string interfaceTableModule; //EXTRA this line is added becaus "extends MACProtocolBase like IMACProtocol" is disabled

       string address = default("auto"); // MAC address as hex string (12 hex digits), or
                                          // "auto". "auto" values will be replaced by
                                          // a generated MAC address in init stage 0.

        string radioModule = default("^.radio");   // The path to the Radio module  //FIXME remove default value

        // length of MAC header
        int headerLength @unit(bit) = default(72 bit);
        int mtu @unit("B") = default(127 B - 9 B);
        // bit rate
        double bitrate @unit(bps) = default(250000 bps);

        @class(rpl::SimpleIdealWirelessMAC);                                          // "auto". "auto" values will be replaced by
        
        @signal[packetSentToUpper](type=cPacket); //because of lack of "LayeredProtocolBase"
        @signal[packetReceivedFromUpper](type=cPacket); //because of lack of "LayeredProtocolBase"
        @signal[packetFromUpperDropped](type=cPacket); //because of lack of "LayeredProtocolBase"
        @signal[packetSentToLower](type=cPacket); //because of lack of "LayeredProtocolBase"
        @signal[packetReceivedFromLower](type=cPacket); //because of lack of "LayeredProtocolBase"
        @signal[packetFromLowerDropped](type=cPacket); //because of lack of "LayeredProtocolBase"
        
 //EXTRA BEGIN       
    gates:
        input upperLayerIn @labels(ILinkLayerFrame/down);
        output upperLayerOut @labels(ILinkLayerFrame/up);
        input lowerLayerIn @labels(ILinkLayerFrame/up);
        output lowerLayerOut @labels(ILinkLayerFrame/down);        
//EXTRA END          
}
