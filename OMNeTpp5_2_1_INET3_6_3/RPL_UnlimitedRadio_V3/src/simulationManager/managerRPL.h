//
// Copyright (C) 05/2013 Elisa Rojas
//      Implements the base class for UDPFlowGenerator and TCPFlowGenerator, which share functionality
/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    INET 3.6.3 adaptation, also adapted for using in the wARP-PATH protocol
*/
/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    INET 3.6.3 adaptation, also adapted for using in the IoTorii(WSN) protocol
*/
/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    (3) UPC, Castelldefels, Spain.
 *                    INET 3.6.3 adaptation, also adapted for using in the RPL protocol as a simulation manager module
*/
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

#ifndef RPL_SRC_SIMULATIONMANAGER_MANAGERRPL_H_
#define RPL_SRC_SIMULATIONMANAGER_MANAGERRPL_H_

#include <omnetpp.h>
#include "inet/networklayer/contract/ipv6/IPv6Address.h"
#include "inet/linklayer/common/MACAddress.h"
#include <string>
//#include "src/routing/rpl/RPLRouting.h"


namespace rpl {
using namespace inet;

class RPLRouting;

class ManagerRPL : public cSimpleModule
{
    //friend class RPLRouting;

   protected:

      //To save info about all the nodes in the network
      struct NodeInfo {
          NodeInfo() {isWSN=false;}
          std::string fullName;
          std::string nedTypeName;
          bool isWSN;
      };
      typedef std::vector<NodeInfo> NodeInfoVector;

      //To save info only about the WSN nodes in the network
      struct WSNInfo {
          std::string fullName;
          IPv6Address ipAddress;
          MACAddress macAddress;
          //RPLRouting *pRPLRouting; //cModule *pRPLRouting; //Module in the host that represent the RPL routing
          int moduleIndex;
      };
      typedef std::vector<WSNInfo> WSNInfoVector;

      NodeInfoVector nodeInfo; //Vector that contains the topology, it will be of size topo.nodes[]
      WSNInfoVector wSNInfo; //Vector that contains only the adhoc hosts in the topology and their IP and MAC addresses

   protected:
      virtual int numInitStages() const  {return NUM_INIT_STAGES;} //All stages can be used for initialize(int stage)
      virtual void initialize(int stage) override;
      virtual void extractTopology();
      virtual void handleMessage(cMessage *msg) override;

   public:
      /**@brief return the index corresponding to the address */
      int getIndexFromLLAddress(IPv6Address address);

      /**@brief return the address corresponding to the index */
      IPv6Address getAddressFromIndex(int index);

      /**@brief return the MAC address corresponding to the IP address */
      MACAddress getMacAddressFromIPAddress(IPv6Address address);


      /**@brief return the name corresponding to the address */
      std::string getNameFromAddress(IPv6Address address);

      /**@brief return the name corresponding to the index */
      std::string getNameFromIndex(int index);
protected:

      virtual void finish() override;
};

} //namespace rpl

#endif /* RPL_SRC_SIMULATIONMANAGER_MANAGERRPL_H_ */
