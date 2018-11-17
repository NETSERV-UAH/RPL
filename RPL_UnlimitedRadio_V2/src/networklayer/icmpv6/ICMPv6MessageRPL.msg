//
// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    (3) UPC, Castelldefels, Spain.
//                    INET 3.6.3, adapted for using the RPL routing protocol
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
//     0x00: DODAG Information Solicitation (Section 6.2)
    DIS = 0x00;

//     0x01: DODAG Information Object (Section 6.3)
    DIO = 0x01;
    
//     0x02: Destination Advertisement Object (Section 6.4)
    DAO = 0x02;
//     0x03: Destination Advertisement Object Acknowledgment (Section 6.5)

//     0x80: Secure DODAG Information Solicitation (Section 6.2.2)

//     0x81: Secure DODAG Information Object (Section 6.3.2)

//     0x82: Secure Destination Advertisement Object (Section 6.4.2)

//     0x83: Secure Destination Advertisement Object Acknowledgment (Section 6.5.2)

//     0x8A: Consistency Check (Section 6.6)
    
}

//EXTRA
//DIS options RFC 6550 Section 6.2.3
//DIO options RFC 6550 Section 6.3.3
//DAO options  RFC 6550 Section 6.4.3
enum RPL_OPTIONS
{
    PAD1 = 0x00; //DIS, DIO, and DAO
    PADN = 0x01; //DIS, DIO, and DAO
    
    Solicited_Information = 0x07;  //DIS
    
    DAG_Metric_Container = 0x02;   //DIO
    Routing_Information = 0x03;    //DIO
    DODAG_Configuration = 0x04;    //DIO
    Prefix_Information = 0x08;     //DIO
    
    RPL_Target = 0x05;             //DAO
    Transit_Information = 0x06;    //DAO
    RPL_Target_Descriptor = 0x09;  //DAO
    
    
}

//EXTRA
//6.2.1.  Format of the DIS Base Object

//        0                   1                   2
//        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       |     Flags     |   Reserved    |   Option(s)...
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 
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
    
    int options @enum(RPL_OPTIONS); //RFC 6550, section 6.2.1
}



//EXTRA 
//6.3.1 Format of the DIO Base Object

//        0                   1                   2                   3
//        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       | RPLInstanceID |Version Number |             Rank              |
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       |G|0| MOP | Prf |     DTSN      |     Flags     |   Reserved    |
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       |                                                               |
//       +                                                               +
//       |                                                               |
//       +                            DODAGID                            +
//       |                                                               |
//       +                                                               +
//       |                                                               |
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       |   Option(s)...
//       +-+-+-+-+-+-+-+-+

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
    int dtsn;                 // Destination Advertisement Trigger Sequence Number       
    double IMin;              // The size of Imin in Trcikle algorithm
    int NofDoub;              // Number of doubling in Trcikle algorithm
    int k;                    // Redundancy constant in Trcikle algorithm
    IPv6Address DODAGID;              // IPv6 address set by DODAG root 
     
 
    
    int options @enum(RPL_OPTIONS); //RFC 6550, section 6.3.1
}



//6.4.1 Format of the DAO Base Object

//        0                   1                   2                   3
//        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       | RPLInstanceID |K|D|   Flags   |   Reserved    | DAOSequence   |
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       |                                                               |
//       +                                                               +
//       |                                                               |
//       +                            DODAGID*                           +
//       |                                                               |
//       +                                                               +
//       |                                                               |
//       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//       |   Option(s)...
//       +-+-+-+-+-+-+-+-+


//EXTRA 
//The DAO control message for RPL, Section 6.4.1
//message ICMPv6DAOMsg extends ICMPv6Message
packet ICMPv6DAOMsg extends ICMPv6Message
{
    int code @enum(ICMPv6_RPL_CONTROL_MSG); // RFC 6550, section 6: set to 0x01
    // TODO: checksum 
    //int rplInstanceId = 0; // RPLInstanceID: 8-bit field indicating the topology instance associated with the DODAG, as learned from the DIO.
    //int k;                // The 'K' flag indicates that the recipient is expected to send a DAO-ACK back.
    int d;  // The 'D' flag indicates that the DODAGID field is present.  This flag MUST be set when a local RPLInstanceID is used.
    //int flags = 0; // Flags: The 6 bits remaining unused in the Flags field are reserved for flags.  The field MUST be initialized to zero by the sender and MUST be ignored by the receiver.
    
    //int reserved = 0;  // Reserved: 8-bit unused field.  The field MUST be initialized to zero by the sender and MUST be ignored by the receiver.
    //int daoSequence;  //  DAOSequence: Incremented at each unique DAO message from a node and echoed in the DAO-ACK message.
    IPv6Address DODAGID;  //DODAGID (optional): 128-bit unsigned integer set by a DODAG root that uniquely identifies a DODAG.  This field is only present when the 'D' flag is set.  This field is typically only present when a local RPLInstanceID is in use, in order to identify the DODAGID that is associated with the RPLInstanceID.  When a global RPLInstanceID is in use, this field need not be present.
      
    int options @enum(RPL_OPTIONS); //RFC 6550, section 6.3.1
}