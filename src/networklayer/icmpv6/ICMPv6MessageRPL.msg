//
// Copyright (C) 2001 CTIE, Monash University
// Copyright (C) 2005 Andras Varga
// Copyright (C) 2005 Wei Yang, Ng
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

cplusplus {{
#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ipv6/IPv6Address.h"
#define ICMPv6_HEADER_BYTES  8

}}


    
namespace rpl;

cplusplus {{
using namespace inet;
class inet::IPv6Address;
}}

//class noncobject ::IPv6Address;
class noncobject inet::IPv6Address;


//
//# TODO consolidate this file's contents with ICMPMessage.msg
//

enum ICMPv6Type
{

    ICMPv6_UNSPECIFIED = 0;
    ICMPv6_DESTINATION_UNREACHABLE = 1;
    ICMPv6_PACKET_TOO_BIG = 2;
    ICMPv6_TIME_EXCEEDED = 3;
    ICMPv6_PARAMETER_PROBLEM = 4;
    ICMPv6_ECHO_REQUEST = 128;
    ICMPv6_ECHO_REPLY = 129;
    ICMPv6_MLD_QUERY = 130;
    ICMPv6_MLD_REPORT = 131;
    ICMPv6_MLD_DONE = 132;
    ICMPv6_ROUTER_SOL = 133;
    ICMPv6_ROUTER_AD = 134;
    ICMPv6_NEIGHBOUR_SOL = 135;
    ICMPv6_NEIGHBOUR_AD = 136;
    ICMPv6_REDIRECT = 137;
    ICMPv6_MLDv2_REPORT = 143;
    ICMPv6_EXPERIMENTAL_MOBILITY = 150;  //Zarrar Yousaf 02.08.07 (FMIPv6 Implementation)
    
    //EXTRA
    ICMPv6_RPL_CONTROL_MESSAGE = 155; //RFC 6550, section 6, page 30
}


//cplusplus {{
//typedef int ICMPv6Code;
//}}


//EXTRA
//values of code field
enum ICMPv6_RPL_CONTROL_MSG
{
    DIS = 0x00;
    DIO = 0x01;
}

//EXTRA
//DIS options
enum RPL_DIS_OPTIONS
{
    PAD1 = 0x00;
    PADN = 0x01;
    Solicited_Information = 0x07;
}

//
// ICMPv6 "codes" for type ICMPv6_DESTINATION_UNREACHABLE
//
enum ICMPv6DEST_UN
{

    NO_ROUTE_TO_DEST = 0;
    COMM_WITH_DEST_PROHIBITED = 1;
    //2 - NOT ASSIGNED
    ADDRESS_UNREACHABLE = 3;
    PORT_UNREACHABLE = 4;
}


//
// ICMPv6 "codes" for type ICMPv6_TIME_EXCEEDED
//
enum ICMPv6_TIME_EX
{

    ND_HOP_LIMIT_EXCEEDED = 0;
    ND_FRAGMENT_REASSEMBLY_TIME = 1;
}

//
// ICMPv6 "codes" for type ICMPv6_PARAMETER_PROBLEM
//
enum ICMPv6_PARAMETER_PROB
{

    ERROREOUS_HDR_FIELD = 0;
    UNRECOGNIZED_NEXT_HDR_TYPE = 1;
    UNRECOGNIZED_IPV6_OPTION = 2;
}


//
// Represents an ICMPv6 packet.
//
// Notes:
//    1. number of octets excluding the error datagram that is usually appended
//       in optInfo, i.e. the Type|CODE|CHECKSUM|UNUSED/POINTER/MTU/OTHER
//       as defined in RFC2463
//    2. Any ICMP type with MSB set, i.e. >=128 is an Informational ICMP message
//
packet ICMPv6Message
{
    int type @enum(ICMPv6Type);
        //int code; //TODO: this should be specific to different ICMP types.
}

//
// Notes:
//   1. As defined in RFC2463: Section 3
//
packet ICMPv6DestUnreachableMsg extends ICMPv6Message
{
    int code @enum(ICMPv6DEST_UN);
}

packet ICMPv6PacketTooBigMsg extends ICMPv6Message
{
    int code; //Set to 0 by sender and ignored by receiver.
    int MTU; //MTU of next-hop link
}

packet ICMPv6TimeExceededMsg extends ICMPv6Message
{
    int code @enum(ICMPv6_TIME_EX);
}

packet ICMPv6ParamProblemMsg extends ICMPv6Message
{
    int code @enum(ICMPv6_PARAMETER_PROB);
}

//
// ICMPv6 Echo Request packet (RFC2463: Section 4).
// Data is attached through encapsulation (see ICMPv6.cc)
//
packet ICMPv6EchoRequestMsg extends ICMPv6Message
{
    int code; //set to 0.
    int identifier; // identifier to aid in matching Echo replies. May be Zero
    int seqNumber; // sequence number to aid in matching Echo replies. May be Zero
        //Data is attached through encapsulation. See ICMPv6.cc
}

//
// ICMPv6 Echo Reply packet. Data is attached through encapsulation (see ICMPv6.cc)
//
packet ICMPv6EchoReplyMsg extends ICMPv6Message
{
    int code; //set to 0.
    int identifier; // identifier to aid in matching Echo replies. May be Zero
    int seqNumber; // sequence number to aid in matching Echo replies. May be Zero
}

//EXTRA 
//DIS control message for RPL
packet ICMPv6DISMsg extends ICMPv6Message
{
    int code @enum(ICMPv6_RPL_CONTROL_MSG); // RFC 6550, section 6: set to 0x00
    // TODO: checksum 
    int flags; // RFC 6550, section 6.2.1: set to 0
    int reserved; // RFC 6550, section 6.2.1 set to 0
    
        int RPLInstanceID;          // The ID of the RPL instance
    int VersionNumber;          // DODAG version number
    int V;                      // Node's rank
    int I;                      // Type of the DODAG, Grounded or Flooding
    int D;                      // Destination Advertisement Trigger Sequence Number       
    int Flag;                   // The size of Imin in Trcikle algorithm
    IPv6Address DODAGID;   // IPv6 address set by DODAG root  
    
    int options @enum(RPL_DIS_OPTIONS); //RFC 6550, section 6.2.1
}

//EXTRA 
//DIO control message for RPL
packet ICMPv6DIOMsg extends ICMPv6Message
{
    int code @enum(ICMPv6_RPL_CONTROL_MSG); // RFC 6550, section 6: set to 0x01
    // TODO: checksum 
    //int rplInstanceId = 0; // RFC 6550, section 6.3.1
    int versionNumber = 0; // RFC 6550, section 6.3.1
    int rank = 0;
    int grounded;
    //0
    //MOP
    //Prf
    //int flags = 0; // RFC 6550, section 6.3.1
    //int reserved = 0; // RFC 6550, section 6.3.1
    int DTSN;                 // Destination Advertisement Trigger Sequence Number       
    double IMin;              // The size of Imin in Trcikle algorithm
    int NofDoub;              // Number of doubling in Trcikle algorithm
    int k;                    // Redundancy constant in Trcikle algorithm
    IPv6Address DODAGID;              // IPv6 address set by DODAG root 
     
 
    
    int options @enum(RPL_DIS_OPTIONS); //RFC 6550, section 6.3.1
}