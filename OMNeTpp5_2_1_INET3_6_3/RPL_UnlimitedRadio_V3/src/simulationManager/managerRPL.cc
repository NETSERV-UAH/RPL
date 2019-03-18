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

#include <algorithm>
#include "src/simulationManager/ManagerRPL.h"

#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/ipv6/IPv6InterfaceData.h"


namespace rpl {
using namespace inet;

Define_Module(ManagerRPL);

void ManagerRPL::initialize(int stage)
{
    EV << "->ManagerRPL::initialize()" << endl;
    if(stage == INITSTAGE_NETWORK_LAYER_3) // for preparing IP addresses of interface table
    {
        extractTopology();
    }
    EV << "<-ManagerRPL::initialize()" << endl;
}

void ManagerRPL::extractTopology()
{
    EV << "->ManagerRPL::extractTopology()" << endl;

    // extract topology
    cTopology topo("topo");
    topo.extractByProperty("networkNode"); // topo.extractByProperty("node");
    //cModule *mod = topo.getNode(0)->getModule();

    // fill in NodeInfoVector (isHost and names) and HostInfoVector (IP and MAC addresses)
    nodeInfo.resize(topo.getNumNodes());
    unsigned int nWSN = 0;

    for (unsigned int i=0; i<topo.getNumNodes(); i++)
    {
        EV << "    Node #" << i << ":" <<endl;
        cModule *mod = topo.getNode(i)->getModule();
        nodeInfo[i].nedTypeName = std::string(mod->getNedTypeName()); //returns the ned type name
        nodeInfo[i].fullName = std::string(mod->getFullName()); //getFullName() or getName() returns the name assigned in the topology (such as host1, switch1, etc...)
        EV << "      Ned type: " << nodeInfo[i].nedTypeName << "; Name: " << nodeInfo[i].fullName <<endl;

        //isWSN
        if (nodeInfo[i].nedTypeName.find("RPLRouter") != std::string::npos) //such as: RPLRouter, ...
        {
            nodeInfo[i].isWSN = true;
            //Add element to WSNInfo vector
            WSNInfo newWSN;
            //IInterfaceTable *ift = IPvXAddressResolver().interfaceTableOf(mod);
            IInterfaceTable *ift = L3AddressResolver().interfaceTableOf(mod);

            int nInterfaces = ift->getNumInterfaces();
            if(nInterfaces > 2) //If host has more than 2 interfaces...
                error("The host has more than 2 interfaces (one connected and loopback) and that's still not implemented!");
            for (unsigned int k=0; k<nInterfaces; k++)
            {
                InterfaceEntry *ie = ift->getInterface(k);
                //We add only the info about the entry which is not the loopback
                if (!ie->isLoopback())
                {
                    newWSN.fullName = nodeInfo[i].fullName;
                    newWSN.ipAddress = ie->ipv6Data()->getLinkLocalAddress();
                    newWSN.macAddress = ie->getMacAddress();
                    //If all WSN host have not RPL, an error will occur in next line
                    //newWSN.pRPLRouting = check_and_cast<RPLRouting *>(mod->getSubmodule("rpl"));
                    newWSN.moduleIndex = mod->getIndex();
                    EV << "        " << newWSN.fullName << "-> IP: " << newWSN.ipAddress << "; MAC: " << newWSN.macAddress << "; Module Index: " << newWSN.moduleIndex << "; Vector index: " << i <<endl;
                    nWSN++;
                }
            }

            wSNInfo.push_back(newWSN);
        }
    }

    unsigned int n = wSNInfo.size();
    EV << "  and " << n << "(active)/" << nWSN << "(total) of those nodes are WSN node" << endl;

    EV << "<-ManagerRPL::extractTopology()" << endl;
}


void ManagerRPL::handleMessage(cMessage *msg)
{
    EV << "->ManagerRPL::handleMessage()" << endl;

    throw cRuntimeError("This module doesn't handle message!");

    EV << "<-ManagerRPL::handleMessage()" << endl;
}

int ManagerRPL::getIndexFromLLAddress(IPv6Address address)
{

    Enter_Method("ManagerRPL::getIndexFromAddress()");
    EV << "Vector size is " << wSNInfo.size() << endl;

    for (int i=0; i<wSNInfo.size(); i++){
        if (wSNInfo.at(i).ipAddress == address)
            return i;
    }

    //return -1;
    throw cRuntimeError("ManagerRPL::getIndexFromAddress(): Address not found!");

}

MACAddress ManagerRPL::getMacAddressFromIPAddress(IPv6Address address)
{

    Enter_Method("ManagerRPL::getMacAddressFromIPAddress()");

    for (int i=0; i<wSNInfo.size(); i++){
        if (wSNInfo.at(i).ipAddress == address)
            return wSNInfo.at(i).macAddress;
    }

    //return -1;
    throw cRuntimeError("ManagerRPL::getMacAddressFromIPAddress(): Address not found!");


}


IPv6Address ManagerRPL::getAddressFromIndex(int index)
{

    Enter_Method("ManagerRPL::getAddressFromIndex()");


    //if ((index >= 0) && (index < wSNInfo.size()))
        return wSNInfo.at(index).ipAddress;

    //return IPv6Address::UNSPECIFIED_ADDRESS;

}

std::string ManagerRPL::getNameFromAddress(IPv6Address address)
{

    Enter_Method("ManagerRPL::getNameFromAddress()");

    for (int i=0; i<wSNInfo.size(); i++){
        if (wSNInfo.at(i).ipAddress == address)
            return wSNInfo.at(i).fullName;
    }

    return "Wrong Name";

}

std::string ManagerRPL::getNameFromIndex(int index)
{

    Enter_Method("ManagerRPL::getNameFromIndex()");


    //if ((i > 0) && (i < wSNInfo.size()))
        return wSNInfo.at(index).fullName;

    //return "Wrong Index";

}

void ManagerRPL::finish()
{
    EV << "->ManagerRPL::finish()" << endl;

    std::string topoAddressInfo;
    FILE *destfp;
    if((destfp=fopen("RPLNodeAddresses.txt","w"))!=nullptr)
    {
        for (int i=0; i<wSNInfo.size(); i++){
            topoAddressInfo = "\t" + wSNInfo.at(i).fullName + "\t" + wSNInfo.at(i).ipAddress.str() + wSNInfo.at(i).macAddress.str() + "\n";
            fprintf(destfp, "%c%S", i, topoAddressInfo.c_str());
        }
    }
    fclose(destfp);

    EV << "<-ManagerRPL::finish()" << endl;
}


} // nemespace rpl
