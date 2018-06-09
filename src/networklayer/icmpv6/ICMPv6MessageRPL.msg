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
#include "inet/networklayer/icmpv6/ICMPv6Message_m.h"
#include "inet/networklayer/contract/ipv6/IPv6Address.h"
#define ICMPv6_HEADER_BYTES  8
}}

namespace inet;
//class noncobject ::IPv6Address;
class noncobject IPv6Address;

message ICMPv6Message;

enum ICMPv6TypeRPL
{

    ICMPv6_RPL_CONTROL_MESSAGE = 155; //RFC 6550, section 6, page 30
}


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

//EXTRA 
//DIS control message for RPL
//message ICMPv6DISMsg extends ICMPv6Message
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
//message ICMPv6DIOMsg extends ICMPv6Message
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