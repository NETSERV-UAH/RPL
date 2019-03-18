/***************************************************************************
 * file:        RPLRouting.cc
 *
 * author:      Hamidreza Kermajani
 *
 * copyright:   (C) 2013 UPC, Castelldefels, Spain.
 *
 * description: Implementation of the IPv6 Routing Protocol for Low power
 *              and Lossy Networks (RPL).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *
 ***************************************************************************
 * last modification: 09/29/2013
 **************************************************************************/

/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    (3) UPC, Castelldefels, Spain.
 *
 *                    Adapted for using on INET 3.6.3, and also included some changes such as using ICMPv6 messages
 *                    for transmitting RPL control messages, interface table , and lifesycle modules
 *
 *
 * To read more information about the Kermajani's article, you can use [1].
 *
 *                    [1] Kermajani, Hamidreza, and Carles Gomez. "On the network convergence process
 *                    in RPL over IEEE 802.15. 4 multihop networks: Improvement and trade-offs."
 *                    Sensors 14.7 (2014): 11993-12022.þ
*/

#include "inet/networklayer/ipv6/IPv6Route.h"
#include "inet/networklayer/common/IPSocket.h"
#include "inet/networklayer/contract/ipv6/IPv6ControlInfo.h"
#include "inet/networklayer/ipv6/IPv6InterfaceData.h"

#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"
#include "src/networklayer/icmpv6/RPLUpwardRouting.h"
#include "src/networklayer/icmpv6/ICMPv6RPL.h"


namespace rpl {
using namespace inet;

Define_Module(RPLUpwardRouting);

void RPLUpwardRouting::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        host = getContainingNode(this);
        neighbourDiscoveryRPL = check_and_cast<IPv6NeighbourDiscoveryRPL *>(this->getParentModule()->getSubmodule("neighbourDiscovery"));

        routingTable = getModuleFromPar<IRoutingTable>(par("routingTableModule"), this);

        interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        networkProtocol = getModuleFromPar<INetfilter>(par("networkProtocolModule"), this);
        pManagerRPL = check_and_cast<ManagerRPL *>(getSimulation()->getSystemModule()->getSubmodule("managerRPL"));
        statisticCollector = check_and_cast<StatisticCollector *>(getSimulation()->getSystemModule()->getSubmodule("statisticCollector"));
        icmpv6RPL = check_and_cast<ICMPv6RPL *>(this->getParentModule()->getSubmodule("icmpv6"));
        parentTableRPL = check_and_cast<ParentTableRPL *>(this->getParentModule()->getSubmodule("parentTableRPL"));

        icmpv6InGateId = findGate("icmpv6In");
        icmpv6OutGateId = findGate("icmpv6Out");

        sinkLinkLocalAddress = IPv6Address(par("sinkLinkLocalAddress"));
        sinkGlobalAddress = IPv6Address(par("sinkGlobalAddress"));
        EV << "sink address is " << sinkLinkLocalAddress << endl;

        int mOP = par ("modeOfOperation");
        mop = (RPLMOP) mOP;

        DIOheaderLength = par ("DIOheaderLength");

        DIOIntMin = par ("DIOIntMin");
        DIOIntDoubl = par ("DIOIntDoubl");
        DIORedun = par ("DIORedun");

        DISEnable = par ("DISEnable");
        refreshDAORoutes = par ("refreshDAORoutes");
        DelayDAO = icmpv6RPL->par("DelayDAO");
        defaultLifeTime = icmpv6RPL->par("defaultLifeTime");

        NodeStartTime=par( "RPLStartTime" );

    }
    else if (stage == INITSTAGE_NETWORK_LAYER_3){
        int nInterfaces = interfaceTable->getNumInterfaces();
        if(nInterfaces > 2) //If host has more than 2 interfaces...
            error("The host has more than 2 interfaces (one connected and loopback) and that's still not implemented!");
        for (int k=0; k < nInterfaces; k++)
        {
            InterfaceEntry *ieTemp = interfaceTable->getInterface(k);
            //We add only the info about the entry which is not the loopback
            if (!ieTemp->isLoopback())
            {
                ie = ieTemp;
                interfaceID = ie->getInterfaceId();
                myLLNetwAddr = ie->ipv6Data()->getLinkLocalAddress();
                myGlobalNetwAddr = ie->ipv6Data()->getGlobalAddress();
                if (myGlobalNetwAddr == IPv6Address::UNSPECIFIED_ADDRESS){
                    IPv6Address globalAddress("fd00::");
                    globalAddress.setSuffix(myLLNetwAddr, 64);
                    ie->ipv6Data()->assignAddress(myGlobalNetwAddr, false, 100000, 100000);
                    //myGlobalNetwAddr = ie->ipv6Data()->getGlobalAddress();
                    //myGlobalNetwAddr = ie->ipv6Data()->getPreferredAddress();
                    myGlobalNetwAddr = globalAddress;
                }
                EV << "interfaceID " << interfaceID << ": my link local address is: " << myLLNetwAddr << ", my global address is: " << myGlobalNetwAddr << endl;

            }
        }

        EV << "my Link Local address is " << myLLNetwAddr <<"; my index in the topology is " << pManagerRPL->getIndexFromLLAddress(myLLNetwAddr) << endl;

        if (myGlobalNetwAddr == IPv6Address::UNSPECIFIED_ADDRESS)
            throw cRuntimeError("RPLUpwardRouting::initialize: This node has not Global Address!");
        else if (myLLNetwAddr != IPv6Address::UNSPECIFIED_ADDRESS)
            statisticCollector->registNode(host, this, parentTableRPL, routingTable, myLLNetwAddr, myGlobalNetwAddr);
        else
            throw cRuntimeError("RPLUpwardRouting::initialize: This node has not Link Local Address!");


    }
     else if (stage == INITSTAGE_ROUTING_PROTOCOLS) {
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(host->getSubmodule("status"));
        isOperational = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;

        if (isOperational){
            dtsnInstance = 0;

            DIOTimer = NULL;

            isJoinedFirstVersion = false;
            VersionNember = -1;
            PrParent = IPv6Address::UNSPECIFIED_ADDRESS;
            DIOIMaxLength=DIOIntMin* int(pow (2.0,DIOIntDoubl));

            // Scheduling the sink node to the first DIO transmission!!
            //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER);

           if (myLLNetwAddr == sinkLinkLocalAddress) // If I am the sink node
            {
               statisticCollector->startStatistics(mop, myLLNetwAddr, simTime());
               isJoinedFirstVersion=true;
               VersionNember = 1;
               Version = VersionNember;
               Rank=1; // The value of rank for Sink
               DODAGID = myGlobalNetwAddr;  //DODAGID is a Global address
               Grounded = 1;

               DIO_CurIntsizeNext = DIOIntMin;
               DIO_StofCurIntNext = dodagSartTime;
               DIO_EndofCurIntNext = DIO_StofCurIntNext+DIO_CurIntsizeNext;

               scheduleNextDIOTransmission();
               char buf[100];
               sprintf(buf,"Root");
               host->getDisplayString().setTagArg("t", 0, buf);
          }else { //If I am not the sink node
              char buf[100];
              sprintf(buf,"Not joined!");
              host->getDisplayString().setTagArg("t", 0, buf);
              if (DISEnable)
              {
                  icmpv6RPL->SetDISParameters();
                  icmpv6RPL->scheduleNextDISTransmission();
              }
          }

        } //end operational
     } //end stage routing
}


void RPLUpwardRouting::TrickleReset()
{
    EV << "->RPLUpwardRouting::TrickleReset()" << endl;

    Enter_Method("TrickleReset()");
    if (DIOTimer->isScheduled()){     //EXTRA
        cancelEvent(DIOTimer);
    }
    //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
    DIO_CurIntsizeNext = DIOIntMin;
    DIO_StofCurIntNext = simTime();
    DIO_EndofCurIntNext = DIO_StofCurIntNext+DIO_CurIntsizeNext;
    EV << "Trickle is reset to Imin for next DIO, theoretical Imin : " << DIOIntMin << "practical Imin = Imin + simtime = : " << DIO_EndofCurIntNext << endl;

    EV << "<-RPLUpwardRouting::TrickleReset()" << endl;

}

void RPLUpwardRouting::scheduleNextDIOTransmission()
{
    EV << "->RPLUpwardRouting::scheduleNextDIOTransmission()" << endl;

    DIO_CurIntsizeNow = DIO_CurIntsizeNext;
    DIO_StofCurIntNow = DIO_StofCurIntNext;
    DIO_EndofCurIntNow = DIO_EndofCurIntNext;
    TimetoSendDIO = DIO_StofCurIntNow + (DIO_CurIntsizeNow/2) + uniform(0,DIO_CurIntsizeNow/2);
    EV << "Schedule next DIO : theoretical I = " << DIO_CurIntsizeNow << ", practical I = I + simtime = : " << DIO_EndofCurIntNow << endl;
    EV << "Time to send is random amount in the interval of [practical I/2 practicalI] uniformly :" << TimetoSendDIO << endl;


    if (DIOTimer)//EXTRA
        throw cRuntimeError("RPLUpwardRouting::scheduleNextDIOTransmission: DIO Timer must be nullptr.");
    else
        DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER);

    scheduleAt(TimetoSendDIO, DIOTimer);
    DIO_CurIntsizeNext *= 2;
    EV << "New theoretical I = I * 2 = " << DIO_CurIntsizeNext << endl;
    if (DIO_CurIntsizeNext>DIOIMaxLength)
    {
        EV << "New theoretical I (" << DIO_CurIntsizeNext << " is higher than Imax (" << DIOIMaxLength << "), so I is not changed." << endl;
        DIO_CurIntsizeNext = DIOIMaxLength;
    }

    DIO_StofCurIntNext = DIO_EndofCurIntNext;
    DIO_EndofCurIntNext = DIO_StofCurIntNext+DIO_CurIntsizeNext;
    DIO_c = 0;

    EV << "<-RPLUpwardRouting::scheduleNextDIOTransmission()" << endl;
}

void RPLUpwardRouting::DeleteDIOTimer()
{
    Enter_Method("DeleteDIOTimer()");
    if (DIOTimer){
        if(DIOTimer->isScheduled()){
            cancelAndDelete(DIOTimer);
            DIOTimer = nullptr;
        }
        else{
            delete DIOTimer;
            DIOTimer = nullptr;
        }
    }

}

/*
 * Global Repair is a centralized approach in the simulation.
 * This method is called by methods of StatisticCollector.
 */
void RPLUpwardRouting::setParametersBeforeGlobalRepair(simtime_t dodagSartTime){
    if (myLLNetwAddr == sinkLinkLocalAddress){  //Global Repair is run/started by the root node
        VersionNember++;
        Version = VersionNember;
        dtsnInstance++;
        Rank = 1;
        DODAGID = myGlobalNetwAddr;
        Grounded = 1;

        DIO_CurIntsizeNext = DIOIntMin;
        DIO_StofCurIntNext = dodagSartTime;
        DIO_EndofCurIntNext = DIO_StofCurIntNext+DIO_CurIntsizeNext;
    }else{
        isNodeJoined = false;
    }

}
void RPLUpwardRouting::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage())
        handleSelfMsg(msg);
    else
        handleIncommingMessage(msg);
}

void RPLUpwardRouting::handleSelfMsg(cMessage* msg)
{
    if (msg->getKind() == SEND_DIO_TIMER)
        handleDIOTimer(msg);
    else{
        EV << "Unknown self message is deleted." << endl;
        delete msg;
    }
}

void RPLUpwardRouting::handleDIOTimer(cMessage* msg)
{

    if((DIO_c < DIORedun) || (DIORedun == 0))
    {
        EV << "TEST00\n";

        // Broadcast DIO message
        ICMPv6DIOMsg* pkt = new ICMPv6DIOMsg("DIO", DIO);
        IPv6ControlInfo *controlInfo = new IPv6ControlInfo;
        controlInfo->setSrcAddr(myLLNetwAddr); //Link local address
        controlInfo->setDestAddr(IPv6Address::ALL_NODES_2); // IPv6Address::ALL_NODES_2 is ff02::2 (scope 2 (link-local)), FIXME: ff02::1a for rpl
        pkt->setByteLength(DIOheaderLength);
        pkt->setVersionNumber(VersionNember);
        pkt->setRank(Rank);
        pkt->setDODAGID(DODAGID);
        pkt->setGrounded(Grounded);
        pkt->setIMin(DIOIntMin);
        pkt->setNofDoub(DIOIntDoubl);
        pkt->setK(DIORedun);
        pkt->setDTSN(dtsnInstance);
        if (myLLNetwAddr == sinkLinkLocalAddress && refreshDAORoutes)
            dtsnInstance ++;
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        controlInfo->setProtocol(IP_PROT_IPv6_ICMP);
        pkt->setControlInfo(controlInfo);
        send(pkt, icmpv6OutGateId);

        numSentDIO++;
        char buf[100];
        if(myLLNetwAddr == sinkLinkLocalAddress)
             sprintf(buf,"DODAG ROOT\nVerNum = %d\nRank = %d\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, numSentDIO, numReceivedDIO, numSuppressedDIO);
        else
            sprintf(buf,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, parentTableRPL->getPrefParentIPAddress(VersionNember).getSuffix(96).str().c_str(), numSentDIO, numReceivedDIO, numSuppressedDIO);
        host->getDisplayString().setTagArg("t", 0, buf);
        cancelAndDelete(DIOTimer);
        DIOTimer = nullptr; //EXTRA
        //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
        scheduleNextDIOTransmission();
        return;
    }else if(DIO_c >= DIORedun){
        EV << "TEST11\n";

        //if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIO.Suppressed++;
        numSuppressedDIO++;
        char buf1[100];
        sprintf(buf1, "DIO transmission suppressed!");
        host->bubble(buf1);
        char buf2[100];
        sprintf(buf2,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, parentTableRPL->getPrefParentIPAddress(VersionNember).getSuffix(96).str().c_str(), numSentDIO, numReceivedDIO, numSuppressedDIO);
        host->getDisplayString().setTagArg("t", 0, buf2);
        cancelAndDelete(DIOTimer);
        DIOTimer = nullptr; //EXTRA
        //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
        scheduleNextDIOTransmission();
        return;
    }
}

void RPLUpwardRouting::handleIncommingMessage(cMessage* msg)
{
    EV << "->RPLUpwardRouting::handleIncommingMessage()" << endl;

    EV << "An ICMPv6 message is received from the ICMPv6 module, the message name is " << msg->getName() << ", message kind is " << msg->getKind() << endl;

    if(msg->getKind() == DIO)
    {
        handleIncommingDIOMessage(msg);
    } else
        delete msg;

    EV << "<-RPLUpwardRouting::handleIncommingMessage()" << endl;
}

void RPLUpwardRouting::handleIncommingDIOMessage(cMessage* msg)
{

    EV << "->RPLUpwardRouting::handleIncommingDIOMessage()" << endl;
    IPv6ControlInfo *ctrlInfo = nullptr;

    if(msg->getKind() == DIO)
    {
        ICMPv6DIOMsg* netwMsg = check_and_cast<ICMPv6DIOMsg*>(msg);
        ctrlInfo = check_and_cast<IPv6ControlInfo *>(netwMsg->removeControlInfo());
        EV << "Received message is ICMPv6 DIO message, DODAGID ID is " << netwMsg->getDODAGID() << ", src address is " << ctrlInfo->getSrcAddr() << endl;

        numReceivedDIO++;
       if(myLLNetwAddr == sinkLinkLocalAddress){ //I am the sink node, and DIO is deleted.
           if (!neighbourDiscoveryRPL->addNeighborFromRPLMessage(ctrlInfo)){
               throw cRuntimeError("RPLUpwardRouting::handleIncommingDIOMessage(): Neighbor can not be added!");
           }
            char buf2[100];
            sprintf(buf2,"DODAG ROOT\nVerNum = %d\nRank = %d\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, numSentDIO, numReceivedDIO, numSuppressedDIO);
            host->getDisplayString().setTagArg("t", 0, buf2);
            char buf3[50];
            sprintf(buf3,"DIO  deleted!");
            host->bubble(buf3);
            delete msg;
            return;
        }else {  //I am not the sink node.
           if(!isJoinedFirstVersion) {
               if (!neighbourDiscoveryRPL->addNeighborFromRPLMessage(ctrlInfo)){
                   throw cRuntimeError("RPLUpwardRouting::handleIncommingDIOMessage(): Neighbor can not be added!");
               }
               isJoinedFirstVersion = true; //for the first version
               isNodeJoined = true; //For each version
               statisticCollector->nodeJoinedUpward(myLLNetwAddr, netwMsg->getArrivalTime());
               VersionNember = netwMsg->getVersionNumber();

               DIO_CurIntsizeNext = DIOIntMin;
               DIO_StofCurIntNext = netwMsg->getArrivalTime();
               DIO_EndofCurIntNext = DIO_StofCurIntNext+DIO_CurIntsizeNext;

               Grounded = netwMsg->getGrounded();
               DIOIntDoubl = netwMsg->getNofDoub();
               DIOIntMin = netwMsg->getIMin();
               DIORedun = netwMsg->getK();
               DODAGID = netwMsg->getDODAGID();
               parentTableRPL->updateTable(ie, ctrlInfo->getSrcAddr(), netwMsg->getRank(), netwMsg->getDTSN(), netwMsg->getVersionNumber());
               statisticCollector->updateRank(myLLNetwAddr, parentTableRPL->getRank(VersionNember));

               char buf0[50];
               sprintf(buf0, "I joined DODAG%d via node %d !!", VersionNember,ctrlInfo->getSrcAddr());
               host->bubble(buf0);
               char buf1[100];
               sprintf(buf1,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, parentTableRPL->getPrefParentIPAddress(VersionNember).getSuffix(96).str().c_str(), numSentDIO, numReceivedDIO, numSuppressedDIO);
               host->getDisplayString().setTagArg("t", 0, buf1);
               icmpv6RPL->cancelAndDeleteDISTimer();
               scheduleNextDIOTransmission();
            }else if(netwMsg->getVersionNumber() > VersionNember) {
                if (!neighbourDiscoveryRPL->addNeighborFromRPLMessage(ctrlInfo)){
                    throw cRuntimeError("RPLUpwardRouting::handleIncommingDIOMessage(): Neighbor can not be added!");
                }
                //isJoinedFirstVersion = true;
                //IsNodeJoined[pManagerRPL->getIndexFromLLAddress(myLLNetwAddr)] = true;
                isNodeJoined = true; //isNodeJoined was being false when Global repair happened.
                DeleteDIOTimer();
                VersionNember = netwMsg->getVersionNumber();
                dtsnInstance ++; // To inform the down stream nodes. dtsnInstance is accommodated in the DIO msg, so the down stream nodes find out that they must send a new DAO because of the global repair ...
                statisticCollector->nodeJoinedUpward(myLLNetwAddr, netwMsg->getArrivalTime());

                DIOIntDoubl = netwMsg->getNofDoub();
                DIOIntMin = netwMsg->getIMin();
                DIORedun = netwMsg->getK();
                DODAGID = netwMsg->getDODAGID();
                DIO_CurIntsizeNext = DIOIntMin;
                DIO_StofCurIntNext = netwMsg->getArrivalTime();
                DIO_EndofCurIntNext = DIO_StofCurIntNext+DIO_CurIntsizeNext;
                Grounded = netwMsg->getGrounded();
                parentTableRPL->updateTable(ie, ctrlInfo->getSrcAddr(), netwMsg->getRank(), netwMsg->getDTSN(), netwMsg->getVersionNumber());
                statisticCollector->updateRank(myLLNetwAddr, parentTableRPL->getRank(VersionNember));

                char buf0[50];
                sprintf(buf0, "I joined DODAG %d via node %d !!", VersionNember, ctrlInfo->getSrcAddr());
                host->bubble(buf0);
                char buf1[100];
                sprintf(buf1,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, parentTableRPL->getPrefParentIPAddress(VersionNember).getSuffix(96).str().c_str(), numSentDIO, numReceivedDIO, numSuppressedDIO);
                host->getDisplayString().setTagArg("t", 0, buf1);
                icmpv6RPL->cancelAndDeleteDISTimer();
                scheduleNextDIOTransmission();
            }else if((netwMsg->getRank() <= Rank) && (netwMsg->getVersionNumber() == VersionNember)) {
                if (!neighbourDiscoveryRPL->addNeighborFromRPLMessage(ctrlInfo)){
                    throw cRuntimeError("RPLUpwardRouting::handleIncommingDIOMessage(): Neighbor can not be added!");
                }
                numReceivedDIO++;
                DIO_c++;
                DODAGID = netwMsg->getDODAGID();
                Grounded = netwMsg->getGrounded();
                DIOIntDoubl = netwMsg->getNofDoub();
                DIOIntMin = netwMsg->getIMin();
                DIORedun = netwMsg->getK();

                bool needDAO = false;
                if (mop == 2)
                    if ((parentTableRPL->getPrefParentNeighborCache(netwMsg->getVersionNumber())->nceKey->address == ctrlInfo->getSrcAddr()) && (netwMsg->getDTSN() > parentTableRPL->getPrefParentDTSN(netwMsg->getVersionNumber())))
                        /* I get a good rank from my previous (stable) preferred parent, and it has increased
                         * its DTSN (it tell me that I must send a DAO message to it), so I willsend a DAO
                         */
                        needDAO = true;

                if (parentTableRPL->updateTable(ie, ctrlInfo->getSrcAddr(), netwMsg->getRank(), netwMsg->getDTSN(), netwMsg->getVersionNumber()) == 1){  //if table is updated (not creating a new entry)
                    if (needDAO){
                        /* dtsnInstance is accommodated in the next DIO msg, so the down stream nodes find out that
                         * they must send a new DAO because I found a stable preferred (note that if needDAO == true, it is a preferred parent) parent (a preferred parent which is updated again) and want to introduce a downward
                         * route from me to the sink node according to the stable parent, so my down stream nodes can also use my stable route to introduce
                         * themselves to the sink node. Therefore, I increase my dtsnInstance to inform the down stream nodes.
                         */
                        dtsnInstance ++;
                        icmpv6RPL->scheduleNextDAOTransmission(DelayDAO, defaultLifeTime);
                    }
                }

                char buf2[255];
                sprintf(buf2, "A DIO received from node %d !", ctrlInfo->getSrcAddr());
                host->bubble(buf2);
                char buf3[100];
                sprintf(buf3,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, parentTableRPL->getPrefParentIPAddress(VersionNember).getSuffix(96).str().c_str(), numSentDIO, numReceivedDIO, numSuppressedDIO);
                host->getDisplayString().setTagArg("t", 0, buf3);
            }else if(netwMsg->getVersionNumber() < VersionNember){ //This DIO is old. It has been sent before the global repair!
                if (!neighbourDiscoveryRPL->addNeighborFromRPLMessage(ctrlInfo)){
                    throw cRuntimeError("RPLUpwardRouting::handleIncommingDIOMessage(): Neighbor can not be added!");
                }
                host->bubble("DIO deleted!!\nThe sender node should be updated.!!! ");
            }else {
                numReceivedDIO++;
                char buf4[100];
                sprintf(buf4,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnumSentDIO = %d\nnumReceivedDIO = %d\nnumSuppressedDIO = %d", VersionNember, Rank, parentTableRPL->getPrefParentIPAddress(VersionNember).getSuffix(96).str().c_str(), numSentDIO, numReceivedDIO, numSuppressedDIO);
                host->getDisplayString().setTagArg("t", 0, buf4);
                host->bubble("DIO deleted!!");
            }
            delete netwMsg;
        }
    }  // end if DIO

    //if (ctrlInfo != NULL) //EXTRA
    delete ctrlInfo;

    EV << "<-RPLUpwardRouting::handleIncommingDIOMessage()" << endl;
}

void RPLUpwardRouting::handleLowerControl(cMessage *msg)
{
    delete msg;
}

void RPLUpwardRouting::handleUpperMsg(cMessage* msg)
{
    delete msg;
}

void RPLUpwardRouting::finish()
{

}

bool RPLUpwardRouting::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
/*
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_LINK_LAYER) {
            start();
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_LINK_LAYER) {
            stop();
        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH) {
            stop();
        }
    }
    else {
        throw cRuntimeError("Unsupported operation '%s'", operation->getClassName());
    }

    return true;
    */
}

bool RPLUpwardRouting::isNodeJoinedToDAG() const
{
    return isNodeJoined;
}

int RPLUpwardRouting::getVersion() const
{
    return VersionNember;
}

IPv6Address RPLUpwardRouting::getDODAGID() const
{
    return DODAGID;
}

IPv6Address RPLUpwardRouting::getMyLLNetwAddr() const
{
    return myLLNetwAddr;
}

IPv6Address RPLUpwardRouting::getMyGlobalNetwAddr() const
{
    return myGlobalNetwAddr;
}

simtime_t RPLUpwardRouting::getDODAGSartTime() const
{
    return dodagSartTime;
}

int RPLUpwardRouting::getInterfaceID() const
{
    return interfaceID;
}

RPLUpwardRouting::~RPLUpwardRouting()
{
    //cancelAndDelete(DIOTimer); //EXTRA
    DeleteDIOTimer();
}


} // namespace rpl
