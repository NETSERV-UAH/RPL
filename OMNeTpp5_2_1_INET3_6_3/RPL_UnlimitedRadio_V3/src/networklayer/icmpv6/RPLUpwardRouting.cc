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
#include "inet/networklayer/ipv6/IPv6Route.h"
#include "inet/networklayer/contract/ipv6/IPv6ControlInfo.h"
#include "inet/networklayer/ipv6/IPv6InterfaceData.h"

#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"
#include "RPLUpwardRouting.h"
//#include "src/simulationManager/managerRPL.h"

namespace rpl {
using namespace inet;

class managerRPL;

Define_Module(RPLUpwardRouting);

void RPLUpwardRouting::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        host = getContainingNode(this);
        routingTable = getModuleFromPar<IRoutingTable>(par("routingTableModule"), this);

        interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        networkProtocol = getModuleFromPar<INetfilter>(par("networkProtocolModule"), this);
        pManagerRPL = check_and_cast<managerRPL *>(getSimulation()->getSystemModule()->getSubmodule("managerRPL"));

        icmpv6InGateId = findGate("icmpv6In");
        icmpv6OutGateId = findGate("icmpv6Out");

        sinkAddress = IPv6Address(par("sinkAddress"));
        EV << "sink address is " << sinkAddress << endl;
        defaultLifeTime = par ("defaultLifeTime");
        mop = par ("modeOfOperation");
        refreshDAORoutes = par ("refreshDAORoutes");
        DelayDAO = par ("DelayDAO");

        DAOheaderLength = par ("DAOheaderLength");
        DIOheaderLength = par ("DIOheaderLength");
        DISheaderLength = par ("DISheaderLength");

        DIOIntMin = par ("DIOIntMin");
        DIOIntDoubl = par ("DIOIntDoubl");
        DIORedun = par ("DIORedun");
        DISStartDelay = par("DISStartDelay");
        DISIntMin = par ("DISIntMin");
        DISIntDoubl = par ("DISIntDoubl");
        DISRedun = par ("DISRedun");
        DISEnable = par ("DISEnable");

        NodeStartTime=par( "RPLStartTime" );

        ROUTE_INFINITE_LIFETIME = par("ROUTE_INFINITE_LIFETIME");
    }
    else if (stage == INITSTAGE_NETWORK_LAYER_3){
        int nInterfaces = interfaceTable->getNumInterfaces();
        if(nInterfaces > 2) //If host has more than 2 interfaces...
            error("The host has more than 2 interfaces (one connected and loopback) and that's still not implemented!");
        for (int k=0; k<nInterfaces; k++)
        {
            InterfaceEntry *ie = interfaceTable->getInterface(k);
            //We add only the info about the entry which is not the loopback
            if (!ie->isLoopback())
            {
                interfaceID = ie->getInterfaceId();
                myLLNetwAddr = ie->ipv6Data()->getLinkLocalAddress();
                EV << "my link local address is: " << myLLNetwAddr << endl;

            }
        }

        EV << "my Link Local address is " << myLLNetwAddr <<"; my index in the topology is " << pManagerRPL->getIndexFromAddress(myLLNetwAddr) << endl;

        if (myGlobalNetwAddr == IPv6Address::UNSPECIFIED_ADDRESS)
            throw cRuntimeError("RPLUpwardRouting::initialize: This node has not Global Address!");
        else if (myLLNetwAddr != IPv6Address::UNSPECIFIED_ADDRESS)
            collector.registNode(host, myLLNetwAddr, myGlobalNetwAddr);
        else
            throw cRuntimeError("RPLUpwardRouting::initialize: This node has not Link Local Address!");


    }
     else if (stage == INITSTAGE_ROUTING_PROTOCOLS) {
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(host->getSubmodule("status"));
        isOperational = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;

        if (isOperational){
            dtsnInstance = 0;

            DIOTimer = NULL;
            DISTimer = NULL;
            DAOTimer = NULL;

            IsJoined=false;
            VersionNember=-1;
            PrParent = IPv6Address::UNSPECIFIED_ADDRESS;
            DIOIMaxLength=DIOIntMin* int(pow (2.0,DIOIntDoubl));
            DISIMaxLength=DISIntMin* int(pow (2.0,DISIntDoubl));

            // Scheduling the sink node to the first DIO transmission!!
            //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER);

           if (myLLNetwAddr == sinkAddress) // If I am the sink node
            {
                collector->setConvergenceTimeStart(myLLNetwAddr, simTime());
                IsJoined=true;
                ? NodeStartTime = simTime();
                VersionNember=1;
                Version=VersionNember;
                Rank=1; // The value of rank for Sink
                DODAGID=myLLNetwAddr;
                Grounded=1;

                DIO_CurIntsizeNext=DIOIntMin;
                DIO_StofCurIntNext=DODAGSartTime;
                DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;

                scheduleNextDIOTransmission();
                char buf[100];
                sprintf(buf,"Root");
                host->getDisplayString().setTagArg("t", 0, buf);

          }
            else //If I am not the sink node
            {
                char buf[100];
                sprintf(buf,"Not joined!");
                host->getDisplayString().setTagArg("t", 0, buf);
                if (DISEnable)
                {
                    //DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);  //EXTRA
                    DIS_CurIntsizeNext=DISIntMin;
                    DIS_StofCurIntNext =DISStartDelay +DODAGSartTime;
                    DIS_EndofCurIntNext=DIS_StofCurIntNext+DIS_CurIntsizeNext;
                    DISHandler();
                    scheduleNextDISTransmission();
                }
            }

        } //end operational
     } //end stage routing
}


void RPLUpwardRouting::ScheduleNextGlobalRepair()
{

    for(int i=0;i<NodesNumber;i++)
        IsNodeJoined[i] = false;
    IsNodeJoined[pManagerRPL->getIndexFromAddress(sinkAddress)]=true;
    VersionNember++;
    Version=VersionNember;
    dtsnInstance ++;
    NodeCounter_Upward[Version]++;
    EV << "NodeCounter_Upward[" << VersionNember << "] = " << NodeCounter_Upward[VersionNember] << endl;


    DIOStatusNew = CreateNewVersionDIO();
    DIOStatusLast->link = DIOStatusNew;
    DIOStatusLast = DIOStatusNew;

    Rank=1;
    DODAGID=myLLNetwAddr;

    Grounded=1;
    DODAGJoinTimeNew_Upward = CreateNewVersionJoiningTime();
    DODAGJoinTimeNew_Upward->TimetoJoinDODAG=simTime();
    DODAGJoinTimeLast_Upward->link = DODAGJoinTimeNew_Upward;
    DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;

    DODAGSartTime=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
    IsDODAGFormed_Upward= false;
    NodeStateNew = new NodeState;
    NodeStateNew = CreateNewNodeState(pManagerRPL->getIndexFromAddress(myLLNetwAddr),VersionNember,simTime(),Rank);
    NodeStateNew->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = DODAGJoinTimeLast_Upward->TimetoJoinDODAG;

    if(NodeStateHeader==NULL)
    {
        NodeStateLast = NodeStateNew;
        NodeStateHeader = NodeStateNew;
    }
    else
    {
        NodeStateLast ->Link = NodeStateNew;
        NodeStateLast = NodeStateNew;
    }

    DIO_CurIntsizeNext=DIOIntMin;
    DIO_StofCurIntNext=DODAGSartTime;
    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
    for (int i=0; i<NodesNumber;i++){
        NodesAddress[i]->DeleteDIOTimer();
        if (DAOEnable)
            NodesAddress[i]->DeleteDAOTimers();
    }

    if (DISEnable)
        for (int i=0; i<NodesNumber;i++)
        {
            NodesAddress[i]->SetDISParameters();
            NodesAddress[i]->DISHandler();
            NodesAddress[i]->scheduleNextDISTransmission();  //EXTRA
        }
    scheduleAt(simTime()+GlobalRepairTimer,GRepairTimer );
    GRT_Counter++;
}

void RPLUpwardRouting::DeleteScheduledNextGlobalRepair()
{
    Enter_Method("DeleteScheduledNextGlobalRepair()");

    cancelEvent(GRepairTimer);
    scheduleAt(simTime(),GRepairTimer );
}

void RPLUpwardRouting::TrickleReset()
{
    EV << "->RPLUpwardRouting::TrickleReset()" << endl;

    Enter_Method("TrickleReset()");
    if (DIOTimer->isScheduled()){     //EXTRA
        cancelEvent(DIOTimer);
    }
    //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
    DIO_CurIntsizeNext=DIOIntMin;
    DIO_StofCurIntNext=simTime();
    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
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

    scheduleAt(TimetoSendDIO,DIOTimer );
    DIO_CurIntsizeNext*=2;
    EV << "New theoretical I = I * 2 = " << DIO_CurIntsizeNext << endl;
    if (DIO_CurIntsizeNext>DIOIMaxLength)
    {
        EV << "New theoretical I (" << DIO_CurIntsizeNext << " is higher than Imax (" << DIOIMaxLength << "), so I is not changed." << endl;
        DIO_CurIntsizeNext=DIOIMaxLength;
    }

    DIO_StofCurIntNext = DIO_EndofCurIntNext;
    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
    DIO_c=0;

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
    else if (msg->getKind() == SEND_DIS_FLOOD_TIMER)
        handleDISTimer(msg);
    else if (msg->getKind() == SEND_DAO_TIMER)
        handleDAOTimer(msg);
    else if (msg->getKind() == DAO_LIFETIME_TIMER)
        handleDAOTimer(msg);
    else if (msg->getKind() == Global_REPAIR_TIMER)
        handleGlobalRepairTimer(msg);
    else{
        EV << "Unknown self message is deleted." << endl;
        delete msg;
    }
}

void RPLUpwardRouting::handleDIOTimer(cMessage* msg)
{

    if(((DIO_c<DIORedun)&&(Version==VersionNember))||(DIORedun==0))
    {
        // Broadcast DIO message
        ICMPv6DIOMsg* pkt = new ICMPv6DIOMsg("DIO", DIO);
        IPv6ControlInfo *controlInfo = new IPv6ControlInfo;
        controlInfo->setSrcAddr(myLLNetwAddr);
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
        if (myLLNetwAddr == sinkAddress && refreshDAORoutes)
            dtsnInstance ++;
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        controlInfo->setProtocol(IP_PROT_IPv6_ICMP);
        pkt->setControlInfo(controlInfo);
        send(pkt, icmpv6OutGateId);

        if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIO.Sent++;
        DIOStatusLast->nbDIOSent++;
        char buf[100];
        if(myLLNetwAddr==sinkAddress)
             sprintf(buf,"DODAG ROOT\nVerNum = %d\nRank = %d\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
        else
            sprintf(buf,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str(),DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
        host->getDisplayString().setTagArg("t", 0, buf);
        cancelAndDelete(DIOTimer);
        DIOTimer = nullptr; //EXTRA
        //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
        scheduleNextDIOTransmission();
        return;
    }
    else
    {
        if((DIO_c>=DIORedun)&&(Version==VersionNember))
        {
            if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIO.Suppressed++;

            DIOStatusLast->nbDIOSuppressed++;
            char buf1[100];
            sprintf(buf1, "DIO transmission suppressed!");
            host->bubble(buf1);
            char buf2[100];
            sprintf(buf2,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str(),DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
            host->getDisplayString().setTagArg("t", 0, buf2);
            cancelAndDelete(DIOTimer);
            DIOTimer = nullptr; //EXTRA
            //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
            scheduleNextDIOTransmission();
            return;
        }
        else
        {
            if((DIO_c<DIORedun)&&(Version-1==VersionNember))
            {
                cancelAndDelete(DIOTimer);
                DIOTimer = nullptr; //EXTRA
                //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
                scheduleNextDIOTransmission();
                return;
           }
        }
    }

}


void RPLUpwardRouting::handleDAOTimer(cMessage* msg)
{
    if (DAOEnable){
        if (NofParents > 0)  //there is a prparent
            sendDAOMessage(myLLNetwAddr, defaultLifeTime); //or sendDAO(PrParent, defaultLifeTime);
        else
            EV<< "DAO can not be sent. There is no preferred parent." << endl;

        if (DAOTimer){
            cancelAndDelete(DAOTimer);
        }

        if (!DAOLifeTimer){
            scheduleDAOlifetimer(defaultLifeTime);
        }
    }
}

void RPLUpwardRouting::handleGlobalRepairTimer(cMessage* msg)
{

    //DeleteDIOTimer();  //EXTRA this method is called in the ScheduleNextGlobalRepair()
    ScheduleNextGlobalRepair();
   /* if (DISEnable)   //These are in the ScheduleNextGlobalRepair()
        for (int i=0; i<NodesNumber;i++)
            if(i != pManagerRPL->getIndexFromAddress(sinkAddress))
            {
                NodesAddress[i]->SetDISParameters();
                NodesAddress[i]->scheduleNextDISTransmission();  // this is added to ScheduleNextGlobalRepair()
            } */
    scheduleNextDIOTransmission();  // root node must run it.
    return;

}

void RPLUpwardRouting::handleIncommingMessage(cMessage* msg)
{
    EV << "->RPLUpwardRouting::handleIncommingMessage()" << endl;

    EV << "An ICMPv6 message is received from the ICMPv6 module, the message name is " << msg->getName() << ", message kind is " << msg->getKind() << endl;

    if(msg->getKind()==DIO)
    {
        handleIncommingDIOMessage(msg);
    } else if(msg->getKind()==DIS_FLOOD)
    {
        handleIncommingDISMessage(msg);
    } else if(msg->getKind()==DAO)
    {
        handleIncommingDAOMessage(msg);
    } else
        delete msg;


    EV << "<-RPLUpwardRouting::handleIncommingMessage()" << endl;
}

void RPLUpwardRouting::handleIncommingDIOMessage(cMessage* msg)
{

    EV << "->RPLUpwardRouting::handleIncommingDIOMessage()" << endl;
    IPv6ControlInfo *ctrlInfo = nullptr;

    if(msg->getKind()==DIO)
    {
        ICMPv6DIOMsg* netwMsg = check_and_cast<ICMPv6DIOMsg*>(msg);
        ctrlInfo = check_and_cast<IPv6ControlInfo *>(netwMsg->removeControlInfo());
        EV << "Received message is ICMPv6 DIO message, DODAGID ID is " << netwMsg->getDODAGID() << ", src address is " << ctrlInfo->getSrcAddr() << endl;

       if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward)) NodeStateLast->DIO.Received++;  //if simulation is not end ...
       if(myLLNetwAddr == sinkAddress) //I am the sink node, and DIO is deleted.
        {
            DIOStatusLast->nbDIOReceived++;
            char buf2[100];
            sprintf(buf2,"DODAG ROOT\nVerNum = %d\nRank = %d\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
            host->getDisplayString().setTagArg("t", 0, buf2);
            char buf3[50];
            sprintf(buf3,"DIO  deleted!");
            host->bubble(buf3);
            delete msg;
            return;
        }
        else
        {
           if(!IsJoined)
            {
               IsJoined=true;
               IsNodeJoined[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = true;
               VersionNember=netwMsg->getVersionNumber();

               DIOStatusNew = CreateNewVersionDIO();
               DIOStatusNew->nbDIOReceived++;
               if(DIOStatusHeader == NULL)
               {
                   DIOStatusLast = DIOStatusNew;
                   DIOStatusHeader = DIOStatusNew;
               }
               else
               {
                   DIOStatusLast->link = DIOStatusNew;
                   DIOStatusLast = DIOStatusNew;
               }

               NodeCounter_Upward[VersionNember]++;
               EV << "NodeCounter_Upward[" << VersionNember << "] = " << NodeCounter_Upward[VersionNember] << endl;

               DODAGJoinTimeNew_Upward = CreateNewVersionJoiningTime();
               DODAGJoinTimeNew_Upward->TimetoJoinDODAG=netwMsg->getArrivalTime();
               if (DODAGJoinTimeHeader_Upward==NULL)
               {
                   DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;
                   DODAGJoinTimeHeader_Upward = DODAGJoinTimeNew_Upward;
               }
               else
               {
                   DODAGJoinTimeLast_Upward->link = DODAGJoinTimeNew_Upward;
                   DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;

               }

               NodeStateLast->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = DODAGJoinTimeLast_Upward->TimetoJoinDODAG;

               DIO_CurIntsizeNext=DIOIntMin;
               DIO_StofCurIntNext=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
               DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;

               Grounded=netwMsg->getGrounded();
               DIOIntDoubl=netwMsg->getNofDoub();
               DIOIntMin=netwMsg->getIMin();
               DIORedun=netwMsg->getK();
               DODAGID=netwMsg->getDODAGID();
               updateTable(ie, ctrlInfo->getSrcAddr(), netwMsg->getRank(), netwMsg->getDTSN(), netwMsg->getVersionNumber());
               NodeStateNew->Rank[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = Rank;

               char buf0[50];
               sprintf(buf0, "I joined DODAG%d via node %d !!", VersionNember,ctrlInfo->getSrcAddr());
               host->bubble(buf0);
               char buf1[100];
               sprintf(buf1,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str(),DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
               host->getDisplayString().setTagArg("t", 0, buf1);
               cancelAndDelete(DISTimer);
               DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
               scheduleNextDIOTransmission();
            }else
                if(netwMsg->getVersionNumber()>VersionNember)
                {
                    //IsJoined = true;
                    IsNodeJoined[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = true;
                    DeleteDIOTimer();
                    VersionNember=netwMsg->getVersionNumber();
                    dtsnInstance ++; // To inform the down stream nodes. dtsnInstance is accommodated in the DIO msg, so the down stream nodes find out that they must send a new DAO because of the global repair ...

                    DIOStatusNew = CreateNewVersionDIO();
                    DIOStatusNew->nbDIOReceived++;

                    if(DIOStatusHeader == NULL)
                    {
                        DIOStatusLast = DIOStatusNew;
                        DIOStatusHeader = DIOStatusNew;
                    }
                    else
                    {
                        DIOStatusLast->link = DIOStatusNew;
                        DIOStatusLast = DIOStatusNew;
                    }

                    NodeCounter_Upward[VersionNember]++;
                    EV << "NodeCounter_Upward[" << VersionNember << "] = " << NodeCounter_Upward[VersionNember] << endl;

                    DODAGJoinTimeNew_Upward = CreateNewVersionJoiningTime();
                    DODAGJoinTimeNew_Upward->TimetoJoinDODAG=netwMsg->getArrivalTime();
                    if (DODAGJoinTimeHeader_Upward==NULL)
                    {
                        DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;
                        DODAGJoinTimeHeader_Upward = DODAGJoinTimeNew_Upward;
                    }
                    else
                    {
                        DODAGJoinTimeLast_Upward->link = DODAGJoinTimeNew_Upward;
                        DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;
                    }
                    NodeStateLast->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
                    DIOIntDoubl=netwMsg->getNofDoub();
                    DIOIntMin=netwMsg->getIMin();
                    DIORedun=netwMsg->getK();
                    DODAGID=netwMsg->getDODAGID();
                    DIO_CurIntsizeNext=DIOIntMin;
                    DIO_StofCurIntNext=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
                    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
                    Grounded=netwMsg->getGrounded();
                    updateTable(ie, ctrlInfo->getSrcAddr(), netwMsg->getRank(), netwMsg->getDTSN(), netwMsg->getVersionNumber());
                    NodeStateNew->Rank[pManagerRPL->getIndexFromAddress(myLLNetwAddr)] = Rank;

                    char buf0[50];
                    sprintf(buf0, "I joined DODAG %d via node %d !!", VersionNember,ctrlInfo->getSrcAddr());
                    host->bubble(buf0);
                    char buf1[100];
                    sprintf(buf1,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str() ,DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
                    host->getDisplayString().setTagArg("t", 0, buf1);
                    cancelAndDelete(DISTimer);
                    DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
                    scheduleNextDIOTransmission();

                }else
                if((netwMsg->getRank()<=Rank)&&(netwMsg->getVersionNumber()==VersionNember))
                {
                    DIOStatusLast->nbDIOReceived++;
                    DIO_c++;
                    DODAGID=netwMsg->getDODAGID();
                    Grounded=netwMsg->getGrounded();
                    DIOIntDoubl=netwMsg->getNofDoub();
                    DIOIntMin=netwMsg->getIMin();
                    DIORedun=netwMsg->getK();

                    bool needDAO = false;
                    if (mop == 2)
                        if ((getPrefParentNeighborCache(netwMsg->getVersionNumber())->nceKey->address == ctrlInfo->getSrcAddr()) && (netwMsg->getDTSN() > getPrefParentDTSN(netwMsg->getVersionNumber())))
                            /* I get a good rank from my previous (stable) preferred parent, and it has increased
                             * its DTSN (it tell me that I must send a DAO message to it), so I willsend a DAO
                             */
                            needDAO = true;

                    if (updateTable(ie, ctrlInfo->getSrcAddr(), netwMsg->getRank(), netwMsg->getDTSN(), netwMsg->getVersionNumber()) == 1){  //if table is updated (not creating a new entry)
                        if (needDAO){
                            /* dtsnInstance is accommodated in the next DIO msg, so the down stream nodes find out that
                             * they must send a new DAO because I found a stable preferred (note that if needDAO == true, it is a preferred parent) parent (a preferred parent which is updated again) and want to introduce a downward
                             * route from me to the sink node according to the stable parent, so my down stream nodes can also use my stable route to introduce
                             * themselves to the sink node. Therefore, I increase my dtsnInstance to inform the down stream nodes.
                             */
                            dtsnInstance ++;
                            scheduleNextDAOTransmission(DelayDAO, defaultLifeTime);
                        }


                    }

                    char buf2[255];
                    sprintf(buf2, "A DIO received from node %d !", ctrlInfo->getSrcAddr());
                    host->bubble(buf2);
                    char buf3[100];
                    sprintf(buf3,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str() ,DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
                    host->getDisplayString().setTagArg("t", 0, buf3);
                }
                else
                    if(netwMsg->getVersionNumber() < VersionNember) //This DIO is old. It has been sent before the global repair!
                    {
                        host->bubble("DIO deleted!!\nThe sender node should be updated.!!! ");
                    }
                    else
                    {
                        DIOStatusLast->nbDIOReceived++;
                        char buf4[100];
                        sprintf(buf4,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str(),DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
                        host->getDisplayString().setTagArg("t", 0, buf4);
                        host->bubble("DIO deleted!!");
                    }

           if((!DAOEnable) && (NodeCounter_Upward[Version]==NodesNumber) && (!IsDODAGFormed_Upward))
            {
                FileRecordCounter++;
                host->bubble("I'm the last node that joined DODAG! DODAG formed!!");
                IsDODAGFormed_Upward=true;
                NodeStateLast->DODAGsFormationTimeRecords_Upward = netwMsg->getArrivalTime()-DODAGSartTime;
                AvgDODAGFomationTime_Upward+=NodeStateLast->DODAGsFormationTimeRecords_Upward;
                AvgAllDIOsSent+=NodeStateLast->DIO.Sent;
                AvgAllDIOsReceived+=NodeStateLast->DIO.Received;
                AvgAllDIOsSuppressed+=NodeStateLast->DIO.Suppressed;
                NodeCounter_Upward[Version]++;
                EV << "NodeCounter_Upward[" << VersionNember << "] = " << NodeCounter_Upward[VersionNember] << endl;
                NofDODAGformationNormal++;
                EV << "Number of DODAGformationNormal is " << NofDODAGformationNormal << endl;

                if(NodeStateLast->DODAGsFormationTimeRecords_Upward!=0)
                {
                    FileRecord.Collosion[FileRecordCounter] = NodeStateLast->Collision;
                    FileRecord.FormationTime_Upward[FileRecordCounter] = SIMTIME_DBL(NodeStateLast->DODAGsFormationTimeRecords_Upward);
                    FileRecord.DIOSent[FileRecordCounter] = NodeStateLast->DIO.Sent;
                    FileRecord.DISSent[FileRecordCounter] = NodeStateLast->DIS.Sent;
                    FileRecord.PacketLost[FileRecordCounter] = NodeStateLast->PacketLost;
                    FileRecord.numParents[FileRecordCounter] = NodeStateLast->numParents_Upward;
                    FileRecord.numPreferedParents[FileRecordCounter] = NodeStateLast->numPreferedParents_Upward;

                }
                if(NodeStateNew->DODAGsFormationTimeRecords_Upward!=0)
                {
                    for (int i=0; i<NodesNumber;i++)
                    {
                        if(i!=pManagerRPL->getIndexFromAddress(sinkAddress))
                        {
                            FileRecord.OtherFields[FileRecordCounter].JoiningTime[i] = SIMTIME_DBL(NodeStateLast->JoiningDODAGTime_Upward[i] - NodeStateLast->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(sinkAddress)]);
                            FileRecord.OtherFields[FileRecordCounter].NodesRank[i] = NodeStateLast->Rank[i];
                        }
                        FileRecord.OtherFields[FileRecordCounter].ConsumedPower[i] = NodeStateLast->PowerConsumption[i];
                    }
                }
                if (GlobalRepairTimer!=0)
                    NodesAddress[pManagerRPL->getIndexFromAddress(sinkAddress)]->DeleteScheduledNextGlobalRepair();
                else
                    Datasaving(pManagerRPL->getIndexFromAddress(sinkAddress),DISEnable);
            }

            delete netwMsg;

            if(GRT_Counter==NumberofIterations)
            {
                Datasaving(pManagerRPL->getIndexFromAddress(sinkAddress), DISEnable);
                endSimulation();
            }
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



int RPLUpwardRouting::getParentIndex(const IPv6Address& id)
{
    for(int i=0;i<NofParents[VersionNember];i++)
      if (Parents[VersionNember][i].ParentId==id)
          return(i);
    return(-1);
}

RPLUpwardRouting::DIOStatus* RPLUpwardRouting::CreateNewVersionDIO()
{
    DIOStatus* Temp;
    Temp = new DIOStatus;
    Temp->version=VersionNember;
    Temp->nbDIOSent=0;
    Temp->nbDIOReceived=0;
    Temp->nbDIOSuppressed=0;
    Temp->link=NULL;
    return Temp;
}


RPLUpwardRouting::DODAGJoiningtime* RPLUpwardRouting::CreateNewVersionJoiningTime()
{
    DODAGJoiningtime* Temp;
    Temp = new DODAGJoiningtime;
    Temp->version=VersionNember;
    Temp->link=NULL;
    return Temp;
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

RPLUpwardRouting::~RPLUpwardRouting()
{
    //cancelAndDelete(DIOTimer); //EXTRA
    DeleteDIOTimer();
    DeleteDAOTimers();
    cancelAndDelete(DISTimer);
    cancelAndDelete(GRepairTimer);
    delete [] NofParents;
    for( int i = 0 ; i < NumberofIterations+1 ; i++ )
        delete [] Parents[i];
    delete [] Parents;

    DIOStatusNew  = DIOStatusHeader;
    while(DIOStatusNew)
    {
        DIOStatusNew= DIOStatusNew->link;
        delete DIOStatusHeader;
        DIOStatusHeader = DIOStatusNew;
    }

    DISStatusNew  = DISStatusHeader;
    while(DISStatusNew)
    {
        DISStatusNew= DISStatusNew->link;
        delete DISStatusHeader;
        DISStatusHeader = DISStatusNew;
    }

    DODAGJoinTimeNew_Upward  = DODAGJoinTimeHeader_Upward;
    while(DIOStatusNew)
    {
        DODAGJoinTimeNew_Upward= DODAGJoinTimeNew_Upward->link;
        delete DODAGJoinTimeHeader_Upward;
        DODAGJoinTimeHeader_Upward = DODAGJoinTimeNew_Upward;
    }

    if (NodeCounter_Upward){
        delete [] NodeCounter_Upward;
        NodeCounter_Upward = nullptr;    //for mutual exclusion
    }

    FileRecordMemoryDeallocation();

    NodeStateNew = NodeStateHeader;
    while(NodeStateNew)
    {
        NodeStateNew= NodeStateHeader->Link;

        delete [] NodeStateHeader->Rank;
        delete [] NodeStateHeader->JoiningDODAGTime_Upward;
        delete [] NodeStateHeader->PowerConsumption;
        delete NodeStateHeader;
        NodeStateHeader = NodeStateNew;
    }

}

void Datasaving(int sinkAddressIndex, bool DISEnable)
{
    FileRecord.IterationsNumber = NofDODAGformationNormal;
    char * temp = SetPath(MainPath,"IterationsNumber_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    IterationsNumber = fopen(Path,Mode);
    fprintf(IterationsNumber,"%d\n",NofDODAGformationNormal);


    temp = SetPath(MainPath,"JoiningTime_Upward_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    JoiningTime_Upward = fopen(Path,Mode);

    temp = SetPath(MainPath,"DIOSent_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    DIOSent = fopen(Path,Mode);

    temp = SetPath(MainPath,"FormationTime_Upward_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    FormationTime_Upward = fopen(Path,Mode);


    temp = SetPath(MainPath,"numTableEntris_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    numTableEntris = fopen(Path,Mode);

    temp = SetPath(MainPath,"NodesRank_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    NodesRank = fopen(Path,Mode);

    temp = SetPath(MainPath,"numPreferedParent_Upward_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    preferedParent_Upward = fopen(Path,Mode);

    temp = SetPath(MainPath,"numParents_K",K_value);
    strcpy(Path, temp);
    delete[] temp;
    numberOfParents = fopen(Path,Mode);


    if (DISEnable)
    {
        temp = SetPath(MainPath,"DISSent_K",K_value);
        strcpy(Path, temp);
        delete[] temp;
        DISSent = fopen(Path,Mode);
        for(int j=0;j<NofDODAGformationNormal;j++)
            fprintf(DISSent,"%d\n",FileRecord.DISSent[j]);
    }

    for(int j=0;j<NofDODAGformationNormal;j++)
    {
        fprintf(FormationTime_Upward,"%f\n",FileRecord.FormationTime_Upward[j]);
        fprintf(DIOSent,"%d\n",FileRecord.DIOSent[j]);

        fprintf(preferedParent_Upward,"%d\n",FileRecord.numPreferedParents[j]);
        fprintf(numberOfParents,"%d\n",FileRecord.numParents[j]);
        fprintf(numTableEntris,"%d\n",FileRecord.numParents[j] + FileRecord.numPreferedParents[j]);

        for (int i=0; i<NodesNumber;i++)
        {
            if(i != sinkAddressIndex)
            {
                fprintf(JoiningTime_Upward,"%f\n",FileRecord.OtherFields[j].JoiningTime[i]);
                fprintf(NodesRank,"%d\n",FileRecord.OtherFields[j].NodesRank[i]);
            }
        }
    }

    fclose(JoiningTime_Upward);
    fclose(DIOSent);
    fclose(DISSent);
    fclose(FormationTime_Upward);
    fclose(NodesRank);
    fclose(IterationsNumber);
    fclose(preferedParent_Upward);
    fclose(numberOfParents);

}

} // namespace rpl
