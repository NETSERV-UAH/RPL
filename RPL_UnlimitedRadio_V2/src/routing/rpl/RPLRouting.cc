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
#include "src/routing/rpl/RPLRouting.h"
//#include "src/simulationManager/managerRPL.h"

namespace rpl {
using namespace inet;

class managerRPL;
char Path[100],MainPath[100],Mode[4] = "a+t",K_value[3];
bool* IsNodeJoined,GlobalRepairTimerReset=false,Flg=false,IsFirsttime=true;
bool NeighboursCalculationFlag=false,IsDODAGFormed_Upward=false;
simtime_t DODAGSartTime,AvgDODAGFomationTime_Upward;
int AllDIOsSent=0,AllDIOsReceived=0,AllDIOsSuppressed=0,AvgAllDIOsSent=0;
int AvgAllDIOsReceived=0,AvgAllDIOsSuppressed=0;

int Version,NodesNumber,NumberofIterations,*NodeCounter_Upward=nullptr,GRT_Counter=0;  //initializing nullptr to deallocate memory

static int NofDODAGformationNormal=0; //test11!!
double AvgAllCollisionNarmal=0;
FILE *JoiningTime_Upward,*Collosion,*DIOSent,*DISSent,*FormationTime_Upward,*PacketLost,*NodesRank,*ConsumedPower;
FILE *IterationsNumber;
FILE *preferedParent_Upward, *numberOfParents, *numTableEntris;

RPLRouting **NodesAddress;

struct SubDataStr
{
    float *JoiningTime,*ConsumedPower;
    int *NodesRank;
};

struct DataStructure
{
    SubDataStr *OtherFields;
    float *FormationTime_Upward;
    int *Collosion,*DIOSent,*DISSent,*PacketLost;
    int IterationsNumber;
    //Variables for saving the number of table entries in each iteration
    int *numPreferedParents = nullptr;
    int *numParents = nullptr;

}FileRecord;

int FileRecordCounter=-1;

void FileRecordMemoryAllocation(void)
{
    FileRecord.Collosion               = new int [NumberofIterations+10];
    FileRecord.DIOSent                 = new int [NumberofIterations+10];
    FileRecord.DISSent                 = new int [NumberofIterations+10];
    FileRecord.PacketLost              = new int [NumberofIterations+10];
    FileRecord.FormationTime_Upward    = new float [NumberofIterations+10];
    //Variables for saving the number of table entries
    FileRecord.numPreferedParents = new int[NumberofIterations+10];
    FileRecord.numParents = new int[NumberofIterations+10];

    FileRecord.OtherFields      = new SubDataStr [NumberofIterations+10];
    for(int i =0;i<NumberofIterations+1;i++)
    {
        FileRecord.OtherFields[i].NodesRank     = new int [NodesNumber+10];
        FileRecord.OtherFields[i].JoiningTime   = new float [NodesNumber+10];
        FileRecord.OtherFields[i].ConsumedPower = new float [NodesNumber+10];
    }
}
void FileRecordMemoryDeallocation(void)
{
    if (FileRecord.OtherFields){
        for(int i =0;i<NumberofIterations+1;i++)
        {
            if (!FileRecord.OtherFields[i].NodesRank) delete [] FileRecord.OtherFields[i].NodesRank;
            if (!FileRecord.OtherFields[i].JoiningTime) delete [] FileRecord.OtherFields[i].JoiningTime;
            if (!FileRecord.OtherFields[i].ConsumedPower) delete [] FileRecord.OtherFields[i].ConsumedPower;
        }
        delete [] FileRecord.OtherFields;
        FileRecord.OtherFields = nullptr;  //for mutual exclusion
    }

    if (!FileRecord.Collosion){
        delete [] FileRecord.Collosion;
        FileRecord.Collosion = nullptr;
    }
    if (!FileRecord.DIOSent){
        delete [] FileRecord.DIOSent;
        FileRecord.DIOSent = nullptr;
    }
    if (!FileRecord.DISSent){
        delete [] FileRecord.DISSent;
        FileRecord.DISSent = nullptr;
    }
    if (!FileRecord.PacketLost){
        delete [] FileRecord.PacketLost;
        FileRecord.PacketLost = nullptr;
    }
    if (!FileRecord.FormationTime_Upward){
        delete [] FileRecord.FormationTime_Upward;
        FileRecord.FormationTime_Upward = nullptr;
    }

    if (!FileRecord.numPreferedParents){
        delete [] FileRecord.numPreferedParents;
        FileRecord.numPreferedParents = nullptr;  //for mutual exclusion
    }

    if (!FileRecord.numParents){
        delete [] FileRecord.numParents;
        FileRecord.numParents = nullptr;  //for mutual exclusion
    }

}

void Datasaving(int,bool);

NodeState *CreateNewNodeState(int Index, int VersionNo, simtime_t Time, int NodeRank)
{
    NodeState* Temp;
    Temp = new NodeState;

    Temp->Rank = new int[NodesNumber];
    Temp->JoiningDODAGTime_Upward = new simtime_t[NodesNumber];
    Temp->PowerConsumption = new double[NodesNumber];

    Temp->Version = VersionNo;
    Temp->DIO.Sent=0;
    Temp->DIO.Received = 0;
    Temp->DIO.Suppressed = 0;
    Temp->DIS.Sent=0;
    Temp->DIS.Received = 0;
    Temp->DIS.Suppressed = 0;
    Temp->Collision = 0;
    Temp->PacketLost = 0;
    Temp->Rank[Index] = NodeRank;
    Temp->JoiningDODAGTime_Upward[Index] = Time;
    Temp->DODAGsFormationTimeRecords_Upward = 0;
    Temp->PowerConsumption[Index] = 0;
    Temp->numPreferedParents_Upward = 0;
    Temp->numParents_Upward = 0;

    Temp->Link=NULL;
    return Temp;
}

char * SetPath(char* MainPath, const char* FileName, char* KValue)
{
    char *TempPath = new char[100];
    strcpy(TempPath,MainPath);
    strcat(TempPath,FileName);
    strcat(TempPath,KValue);
    strcat(TempPath,".txt");
    return TempPath;
}

Define_Module(RPLRouting);

void RPLRouting::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        host = getContainingNode(this);
        //routingTable = getModuleFromPar<IRoutingTable>(par("routingTableModule"), this);
        for (int i=0; i<NumberofIterations+2; i++){
            routingTables.push_back(new RoutingTable());
        }
        interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        networkProtocol = getModuleFromPar<INetfilter>(par("networkProtocolModule"), this);
        pManagerRPL = check_and_cast<managerRPL *>(getSimulation()->getSystemModule()->getSubmodule("managerRPL"));

        icmpv6InGateId = findGate("icmpv6In");
        icmpv6OutGateId = findGate("icmpv6Out");

        sinkAddress = IPv6Address(par("sinkAddress"));
        EV << "sink address is " << sinkAddress << endl;
        defaultLifeTime = par ("defaultLifeTime");
        DAOEnable = par ("DAOEnable");
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
        MaxNofParents = par ("MaxNofParents");
        GlobalRepairTimer = par ("GlobalRepairTimer");
        ROUTE_INFINITE_LIFETIME = GlobalRepairTimer;
        NodesNumber=getParentModule()->getParentModule()->par( "numHosts" );  //NodesNumber=getParentModule()->getParentModule()->par( "numNodes" );
        NumberofIterations = par ("NumberofIterations");
        itoa(DIORedun, K_value, 10);
        strcpy(MainPath,par("FilePath").stringValue());
        NofParents = new int[NumberofIterations+2];
        Parents = new ParentStructure* [NumberofIterations+2];
        for( int i = 0 ; i < NumberofIterations+2 ; i++ )
            Parents[i] = new ParentStructure [MaxNofParents];
        if(IsFirsttime)
        {
            IsFirsttime = false;
            NodesAddress = new RPLRouting* [NodesNumber] ;
            IsNodeJoined = new bool[NodesNumber+1];
            for(int i=0;i<NodesNumber+1;i++) IsNodeJoined[i] = false;
            NodeCounter_Upward= new int[NumberofIterations+2];
            for(int i=0;i<NumberofIterations+2;i++) NodeCounter_Upward[i] = 0;
            FileRecordMemoryAllocation();
        }
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
                myNetwAddr = ie->ipv6Data()->getLinkLocalAddress();
                EV << "my link local address is: " << myNetwAddr << endl;

            }
        }
        NodesAddress[pManagerRPL->getIndexFromAddress(myNetwAddr)] = this;
        EV << "my address is " << myNetwAddr <<"; my index in the topology is " << pManagerRPL->getIndexFromAddress(myNetwAddr) << endl;


    }
     else if (stage == INITSTAGE_ROUTING_PROTOCOLS) {
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(host->getSubmodule("status"));
        isOperational = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;

        if (isOperational){
            dtsnInstance = 0;
             hasRoute = new bool[NumberofIterations+2];
            for(int i=0; i<NumberofIterations+2; i++)
                hasRoute[i] = false;
            NofEntry=0;

            DIOStatusHeader = NULL;
            DISStatusHeader = NULL;
            DODAGJoinTimeHeader_Upward = NULL;
            DODAGJoinTimeHeader_Downward = NULL;

            GRepairTimer = NULL;
            DIOTimer = NULL;
            DISTimer = NULL;
            DAOTimer = NULL;
            DODAGSartTime=simTime();
            AvgDODAGFomationTime_Upward=simTime();
            IsJoined=false;
            VersionNember=-1;
            PrParent = IPv6Address::UNSPECIFIED_ADDRESS;
            DIOIMaxLength=DIOIntMin* int(pow (2.0,DIOIntDoubl));
            DISIMaxLength=DISIntMin* int(pow (2.0,DISIntDoubl));
            for(int i=0;i<NumberofIterations+2;i++)
            {
                NofParents[i]=0;
                for(int j=0;j<MaxNofParents;j++)
                {
                    Parents[i][j].ParentId= IPv6Address::UNSPECIFIED_ADDRESS;
                    Parents[i][j].ParentRank=-1;
                }
            }

            // Scheduling the sink node to the first DIO transmission!!
            DODAGSartTime=simTime();
            //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER);

           if (myNetwAddr==sinkAddress)
            {
                AvgDODAGFomationTime_Upward=simTime();  //line 299!!
                IsJoined=true;
                IsNodeJoined[pManagerRPL->getIndexFromAddress(sinkAddress)]=true;
                NodeStartTime=simTime();
                VersionNember=1;
                Version=VersionNember;
                NodeCounter_Upward[VersionNember]++;
                EV << "NodeCounter_Upward[" << VersionNember << "] = " << NodeCounter_Upward[VersionNember] << endl;
                DIOStatusNew = CreateNewVersionDIO();
                DIOStatusLast = DIOStatusNew;
                DIOStatusHeader = DIOStatusNew;
                Rank=1;
                DODAGID=myNetwAddr;
                Grounded=1;
                DODAGJoinTimeNew_Upward = CreateNewVersionJoiningTime();
                DODAGJoinTimeNew_Upward->TimetoJoinDODAG = simTime();
                DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;
                DODAGJoinTimeHeader_Upward = DODAGJoinTimeNew_Upward;

                DIO_CurIntsizeNext=DIOIntMin;
                DIO_StofCurIntNext=DODAGSartTime;
                DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;

                scheduleNextDIOTransmission();
                char buf[100];
                sprintf(buf,"Root");
                host->getDisplayString().setTagArg("t", 0, buf);
                NodeStateNew = CreateNewNodeState(pManagerRPL->getIndexFromAddress(myNetwAddr),VersionNember,simTime(),Rank);
                NodeStateNew->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myNetwAddr)] = DODAGJoinTimeNew_Upward->TimetoJoinDODAG;
                NodeStateLast = NodeStateNew;
                NodeStateHeader = NodeStateNew;
                if(GlobalRepairTimer!=0)
                {
                    GRepairTimer = new cMessage("GRepair-timer", Global_REPAIR_TIMER);
                    scheduleAt(GlobalRepairTimer,GRepairTimer );
                }
          }
            else
            {
                char buf[100];
                sprintf(buf,"Not joined!");
                host->getDisplayString().setTagArg("t", 0, buf);
                if (DISEnable)
                {
                    DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
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


void RPLRouting::ScheduleNextGlobalRepair()
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
    DODAGID=myNetwAddr;

    Grounded=1;
    DODAGJoinTimeNew_Upward = CreateNewVersionJoiningTime();
    DODAGJoinTimeNew_Upward->TimetoJoinDODAG=simTime();
    DODAGJoinTimeLast_Upward->link = DODAGJoinTimeNew_Upward;
    DODAGJoinTimeLast_Upward = DODAGJoinTimeNew_Upward;

    DODAGSartTime=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
    IsDODAGFormed_Upward= false;
    NodeStateNew = new NodeState;
    NodeStateNew = CreateNewNodeState(pManagerRPL->getIndexFromAddress(myNetwAddr),VersionNember,simTime(),Rank);
    NodeStateNew->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myNetwAddr)] = DODAGJoinTimeLast_Upward->TimetoJoinDODAG;

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
        }
    scheduleAt(simTime()+GlobalRepairTimer,GRepairTimer );
    GRT_Counter++;
}

void RPLRouting::DeleteScheduledNextGlobalRepair()
{
    Enter_Method("DeleteScheduledNextGlobalRepair()");

    cancelEvent(GRepairTimer);
    scheduleAt(simTime(),GRepairTimer );
}

void RPLRouting::scheduleNextDIOTransmission()
{

    DIO_CurIntsizeNow = DIO_CurIntsizeNext;
    DIO_StofCurIntNow = DIO_StofCurIntNext;
    DIO_EndofCurIntNow = DIO_EndofCurIntNext;
    TimetoSendDIO=DIO_StofCurIntNow+uniform(0,DIO_CurIntsizeNow/2)+(DIO_CurIntsizeNow/2);

    if (DIOTimer)//EXTRA
        throw cRuntimeError("RPLRouting::scheduleNextDIOTransmission: DIO Timer must be nullptr.");
    else
        DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER);

    scheduleAt(TimetoSendDIO,DIOTimer );
    DIO_CurIntsizeNext*=2;
    if (DIO_CurIntsizeNext>DIOIMaxLength) DIO_CurIntsizeNext=DIOIMaxLength;
    DIO_StofCurIntNext = DIO_EndofCurIntNext;
    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
    DIO_c=0;
}

void RPLRouting::DeleteDIOTimer()
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

void RPLRouting::scheduleNextDAOTransmission(simtime_t delay, simtime_t LifeTime)
{
    EV << "->RPLRouting::scheduleNextDAOTransmission()" << endl;


    if (DAOTimer){
        EV << "DAO timer already scheduled." << endl;
    }else{
        DAOTimer = new cMessage("DAO-timer", SEND_DAO_TIMER);
        simtime_t expirationTime;
        if (delay == 0){
            expirationTime = 0;
            EV << "Next DAO message is scheduled at t = " << expirationTime << ", delay parameter = " << delay << endl;
        }
        else if (delay == DelayDAO){
            expirationTime = par("randomDelayDAO"); //expirationTime = x~[0,1) * delay + delay/2 or x~[0.5,1.5) * delay
            EV << "DAO message is scheduled at t = " << expirationTime << ", delay parameter = DelayDAO = " << DelayDAO << endl;
        }else{
            throw cRuntimeError("RPLRouting::scheduleNextDAOTransmission: the value of the delay parameter is wrong! delay = %e'", delay);
        }
        scheduleAt(expirationTime, DAOTimer);

        //based on the Contiki-ng ambiguity, there is 3 interpretation:
        //1:
        if (DAOLifeTimer){
            EV << "DAOLifeTimer already scheduled. It is again rescheduled now." << endl;
            //cancelAndDelete(DAOLifeTimer);
            cancelAndDelete(DAOLifeTimer);
            DAOLifeTimer = nullptr;
        }else
            EV << "DAOLifeTimer was not scheduled. It is scheduled now." << endl;

        scheduleDAOlifetimer(LifeTime);




    /*      //2:
            if (!DAOLifeTimer){
                EV << "DAOLifeTimer was not scheduled. It is scheduled." << endl;
                scheduleDAOlifetimer(LifeTime);
            }else{
                EV << "DAOLifeTimer already scheduled. It is not again scheduled." << endl;
            }


            //3: Contiki approach, error on OMNeT?
            scheduleDAOlifetimer(LifeTime);
    */
        }
        EV << "<-RPLRouting::scheduleNextDAOTransmission()" << endl;

}

void RPLRouting::scheduleDAOlifetimer(simtime_t lifeTime)
{
    EV << "->RPLRouting::scheduleDAOlifetimer()" << endl;

    if (lifeTime == ROUTE_INFINITE_LIFETIME){
        EV << "DAO life time is infinit. DAOLifeTimer is not scheduled." << endl;
    }else if (!DAOLifeTimer){
        DAOLifeTimer = new cMessage("DAO-life-timer", DAO_LIFETIME_TIMER);
        if (lifeTime == defaultLifeTime){
               simtime_t expirationTime = par("randomDefaultLifeTime"); //expirationTime = x~[0.5,0.75) * lifeTime
               scheduleAt(expirationTime, DAOLifeTimer);
               EV << "The DAO life time is not infinit. The next DAO is scheduled at t = " << expirationTime << ", the lifeTime parameter = defaultLifeTime = " << defaultLifeTime << endl;
           }else{
               throw cRuntimeError("RPLRouting::scheduleDAOlifetimer: the value of the input parameter is wrong! lifeTime = %e'", lifeTime);
           }
    }else{
        throw cRuntimeError("RPLRouting::scheduleDAOlifetimer: DAOLifeTimer must be nullptr.");
    }

    EV << "->RPLRouting::scheduleDAOlifetimer()" << endl;
}

void RPLRouting::DeleteDAOTimers()
{
    Enter_Method("DeleteDAOTimers()");
    if (DAOTimer){
        cancelAndDelete(DAOTimer);
        DAOTimer = nullptr;
    }

    if (DAOLifeTimer){
        cancelAndDelete(DAOLifeTimer);
        DAOLifeTimer = nullptr;
    }

}

void RPLRouting::TrickleReset()
{
    Enter_Method("TrickleReset()");
    if (DIOTimer->isScheduled()){     //EXTRA
        cancelEvent(DIOTimer);
    }
    //DIOTimer = new cMessage("DIO-timer", SEND_DIO_TIMER); //EXTRA
    DIO_CurIntsizeNext=DIOIntMin;
    DIO_StofCurIntNext=simTime();
    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
}

void RPLRouting::SetDISParameters()
{
    Enter_Method("SetDISParameters()");
    cancelAndDelete(DISTimer);
    DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
    DIS_CurIntsizeNext=DISIntMin;
    DIS_StofCurIntNext =DISStartDelay+DODAGSartTime;
    DIS_EndofCurIntNext=DIS_StofCurIntNext+DIS_CurIntsizeNext;
}

void RPLRouting::scheduleNextDISTransmission()
{
    Enter_Method("scheduleNextDISTransmission()");
    DIS_CurIntsizeNow = DIS_CurIntsizeNext;
    DIS_StofCurIntNow = DIS_StofCurIntNext;
    DIS_EndofCurIntNow = DIS_EndofCurIntNext;
    TimetoSendDIS=DIS_StofCurIntNow+uniform(0,DIS_CurIntsizeNow/2)+(DIS_CurIntsizeNow/2);
    scheduleAt(TimetoSendDIS,DISTimer );
    DIS_CurIntsizeNext*=2;
    if (DIS_CurIntsizeNext>DISIMaxLength) DIS_CurIntsizeNext=DISIMaxLength;
    DIS_StofCurIntNext = DIS_EndofCurIntNext;
    DIS_EndofCurIntNext=DIS_StofCurIntNext+DIS_CurIntsizeNext;
    DIS_c=0;
}

void RPLRouting::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage())
        handleSelfMsg(msg);
    else
        handleIncommingMessage(msg);

}

void RPLRouting::handleSelfMsg(cMessage* msg)
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

void RPLRouting::handleDIOTimer(cMessage* msg)
{

    if(((DIO_c<DIORedun)&&(Version==VersionNember))||(DIORedun==0))
    {
        // Broadcast DIO message
        ICMPv6DIOMsg* pkt = new ICMPv6DIOMsg("DIO", DIO);
        IPv6ControlInfo *controlInfo = new IPv6ControlInfo;
        controlInfo->setSrcAddr(myNetwAddr);
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
        if (myNetwAddr == sinkAddress && refreshDAORoutes)
            dtsnInstance ++;
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        controlInfo->setProtocol(IP_PROT_IPv6_ICMP);
        pkt->setControlInfo(controlInfo);
        send(pkt, icmpv6OutGateId);

        if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIO.Sent++;
        DIOStatusLast->nbDIOSent++;
        char buf[100];
        if(myNetwAddr==sinkAddress)
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

void RPLRouting::handleDISTimer(cMessage* msg)
{

    if(((!IsJoined)&&((DIS_c<DISRedun)||(DISRedun==0)))&&(DISVersion==Version))
    {
        ICMPv6DISMsg* pkt = new ICMPv6DISMsg("DIS", DIS_FLOOD);
        IPv6ControlInfo *controlInfo = new IPv6ControlInfo;
        controlInfo->setSrcAddr(myNetwAddr);
        controlInfo->setDestAddr(IPv6Address::ALL_NODES_2);
        pkt->setByteLength(DISheaderLength);
        pkt->setVersionNumber(VersionNember);
        pkt->setDODAGID(DODAGID);
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        controlInfo->setProtocol(IP_PROT_IPv6_ICMP);
        pkt->setControlInfo(controlInfo);
        send(pkt, icmpv6OutGateId);

        if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIS.Sent++;
        DISStatusLast->nbDISSent++;
        cancelAndDelete(DISTimer);
        DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
        scheduleNextDISTransmission();
        return;
    }
    else
        if(((!IsJoined)&&(DIS_c>=DISRedun))&&(DISVersion==Version))

        {
            if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIS.Suppressed++;

            DISStatusLast->nbDISSuppressed++;
            char buf1[100];
            sprintf(buf1, "DIS transmission suppressed!");
            host->bubble(buf1);
            delete msg;
            cancelAndDelete(DISTimer);
            DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
            scheduleNextDISTransmission();
            return;

        }
        else
            if(IsJoined)
            {
                cancelAndDelete(DISTimer);
                DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);
            }
}

void RPLRouting::handleDAOTimer(cMessage* msg)
{
    if (DAOEnable){
        if (NofParents > 0)  //there is a prparent
            sendDAOMessage(defaultLifeTime); //or sendDAO(PrParent, defaultLifeTime);
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

void RPLRouting::handleGlobalRepairTimer(cMessage* msg)
{

    DeleteDIOTimer();
    ScheduleNextGlobalRepair();
    if (DISEnable)
        for (int i=0; i<NodesNumber;i++)
            if(i != pManagerRPL->getIndexFromAddress(sinkAddress))
            {
                NodesAddress[i]->SetDISParameters();
                NodesAddress[i]->scheduleNextDISTransmission();
            }
    scheduleNextDIOTransmission();
    return;

}

void RPLRouting::handleIncommingMessage(cMessage* msg)
{
    EV << "->RPLRouting::handleIncommingMessage()" << endl;

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


    EV << "<-RPLRouting::handleIncommingMessage()" << endl;
}

void RPLRouting::handleIncommingDIOMessage(cMessage* msg)
{

    EV << "->RPLRouting::handleIncommingDIOMessage()" << endl;
    IPv6ControlInfo *ctrlInfo = nullptr;

    if(msg->getKind()==DIO)
    {
        ICMPv6DIOMsg* netwMsg = check_and_cast<ICMPv6DIOMsg*>(msg);
        ctrlInfo = check_and_cast<IPv6ControlInfo *>(netwMsg->removeControlInfo());
        EV << "Received message is ICMPv6 DIO message, DODAGID address is " << netwMsg->getDODAGID() << ", src address is " << ctrlInfo->getSrcAddr() << endl;

       if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward)) NodeStateLast->DIO.Received++;
       if(myNetwAddr==sinkAddress)
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
               IsNodeJoined[pManagerRPL->getIndexFromAddress(myNetwAddr)] = true;
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

               NodeStateLast->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myNetwAddr)] = DODAGJoinTimeLast_Upward->TimetoJoinDODAG;

               DIO_CurIntsizeNext=DIOIntMin;
               DIO_StofCurIntNext=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
               DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;

               Grounded=netwMsg->getGrounded();
               DIOIntDoubl=netwMsg->getNofDoub();
               DIOIntMin=netwMsg->getIMin();
               DIORedun=netwMsg->getK();
               DODAGID=netwMsg->getDODAGID();
               AddParent(ctrlInfo->getSrcAddr(),netwMsg->getRank(), netwMsg->getDTSN());
               NodeStateNew->Rank[pManagerRPL->getIndexFromAddress(myNetwAddr)] = Rank;

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
                    IsNodeJoined[pManagerRPL->getIndexFromAddress(myNetwAddr)] = true;
                    DeleteDIOTimer();
                    VersionNember=netwMsg->getVersionNumber();
                    dtsnInstance ++;

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
                    NodeStateLast->JoiningDODAGTime_Upward[pManagerRPL->getIndexFromAddress(myNetwAddr)] = DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
                    DIOIntDoubl=netwMsg->getNofDoub();
                    DIOIntMin=netwMsg->getIMin();
                    DIORedun=netwMsg->getK();
                    DODAGID=netwMsg->getDODAGID();
                    DIO_CurIntsizeNext=DIOIntMin;
                    DIO_StofCurIntNext=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
                    DIO_EndofCurIntNext=DIO_StofCurIntNext+DIO_CurIntsizeNext;
                    Grounded=netwMsg->getGrounded();
                    AddParent(ctrlInfo->getSrcAddr(),netwMsg->getRank(), netwMsg->getDTSN());
                    NodeStateNew->Rank[pManagerRPL->getIndexFromAddress(myNetwAddr)] = Rank;

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
                    switch(IsParent(ctrlInfo->getSrcAddr(),netwMsg->getRank()))
                    {
                        case NOT_EXIST:
                            AddParent(ctrlInfo->getSrcAddr(),netwMsg->getRank(), netwMsg->getDTSN());
                            break;
                        case SHOULD_BE_UPDATED:
                            if (IsNeedDAO(ctrlInfo->getSrcAddr(), netwMsg->getDTSN()))
                            {
                                dtsnInstance ++;
                                scheduleNextDAOTransmission(DelayDAO, defaultLifeTime);
                            }
                            DeleteParent(ctrlInfo->getSrcAddr());
                            AddParent(ctrlInfo->getSrcAddr(),netwMsg->getRank(), netwMsg->getDTSN());
                            break;
                        case EXIST:
                            if (IsNeedDAO(ctrlInfo->getSrcAddr(), netwMsg->getDTSN()))
                            {
                                dtsnInstance ++;
                                scheduleNextDAOTransmission(DelayDAO, defaultLifeTime);
                            }
                            break;
                    }
                    char buf2[255];
                    sprintf(buf2, "A DIO received from node %d !", ctrlInfo->getSrcAddr());
                    host->bubble(buf2);
                    char buf3[100];
                    sprintf(buf3,"Joined!\nVerNum = %d\nRank = %d\nPrf.Parent = %s\nnbDIOSent = %d\nnbDIOReceived = %d\nnbDIOSuppressed = %d",VersionNember,Rank,PrParent.getSuffix(96).str().c_str() ,DIOStatusLast->nbDIOSent,DIOStatusLast->nbDIOReceived,DIOStatusLast->nbDIOSuppressed);
                    host->getDisplayString().setTagArg("t", 0, buf3);
                }
                else
                    if(netwMsg->getVersionNumber()<VersionNember)
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

    if (ctrlInfo != NULL)
        delete ctrlInfo;
    EV << "<-RPLRouting::handleIncommingDIOMessage()" << endl;
}


void RPLRouting::handleIncommingDISMessage(cMessage* msg)
{

    EV << "->RPLRouting::handleIncommingDISMessage()" << endl;

    IPv6ControlInfo *ctrlInfo = nullptr;

    if ((msg->getKind() == DIS_FLOOD)&&(IsJoined))
    {
        DIS_c++;
        NodeStateLast->DIS.Received++;
        ICMPv6DISMsg *netwMsg = check_and_cast<ICMPv6DISMsg *>(msg);
        ctrlInfo = check_and_cast<IPv6ControlInfo *> (netwMsg->removeControlInfo());
        char buf2[255];
                sprintf(buf2, "A DIS message received from node %d!\nResetting Trickle timer!", ctrlInfo->getSrcAddr());
        host->bubble(buf2);
        TrickleReset();
        delete netwMsg;
        scheduleNextDIOTransmission();
    }
    else
        if ((msg->getKind() == DIS_FLOOD)&&(!IsJoined))
        {
            DIS_c++;
            NodeStateLast->DIS.Received++;
            ICMPv6DISMsg* netwMsg = check_and_cast<ICMPv6DISMsg*>(msg);
            ctrlInfo = check_and_cast<IPv6ControlInfo *>(netwMsg->removeControlInfo());
            char buf2[255];
            sprintf(buf2, "A DIS message received from node %s!\nBut I am not a member of any DODAG!", ctrlInfo->getSrcAddr());
            host->bubble(buf2);
            delete netwMsg;
        }

    if (ctrlInfo != NULL)
        delete ctrlInfo;
    EV << "<-RPLRouting::handleIncommingDISMessage()" << endl;
}


void RPLRouting::handleIncommingDAOMessage(cMessage* msg)
{

    EV << "->RPLRouting::handleIncommingDAOMessage()" << endl;

    ICMPv6DAOMsg* pkt = check_and_cast<ICMPv6DAOMsg*>(msg);
    IPv6ControlInfo *ctrlInfoIn = check_and_cast<IPv6ControlInfo *>(pkt->removeControlInfo());
    IPv6Address prefix = pkt->getPrefix();
    int prefixLen = pkt->getPrefixLen();
    simtime_t lifeTime = pkt->getLifeTime();
    IPv6Address senderIPAddress = ctrlInfoIn->getSrcAddr(); //Sender address

    EV << "Received message is ICMPv6 DAO message, DODAGID address is " << pkt->getDODAGID() << ", src address is " << ctrlInfoIn->getSrcAddr() << endl;

    int parentIndex = getParentIndex(senderIPAddress);

    if (parentIndex >= 0){
        if (Parents[VersionNember][parentIndex].ParentRank < Rank){
            EV << "DAO message is received from a parent with a lower rank and discarded." << endl;
            delete msg;
            return;
        }

        if (senderIPAddress == PrParent){ // parentIndex = 0
            EV << "DAO message is received from the preferred parent and discarded." << endl;
            delete msg;
            return;
        }

    }


    RoutingTable *routingTable = routingTables.at(VersionNember);
    auto iter = routingTable->find(prefix);
    if (iter == routingTable->end()){
        // Add entry to table
        EV << "Adding entry to Routing Table: " << prefix << " nextHop -->" << senderIPAddress << "\n";
        (*routingTable)[prefix] = RoutingEntry( senderIPAddress, simTime() + lifeTime);
    } else {
        // Update existing entry
        EV << "Updating entry in Address Table: " << prefix << " nextHop -->" << senderIPAddress << "\n";
        RoutingEntry& entry = iter->second;
        entry.nextHop = senderIPAddress;
        entry.lifeTime = simTime() + lifeTime;
    }

    if (NofParents > 0){  //this node has a preferred parent, so DAO must forward to the preferred parent.
        IPv6ControlInfo *ctrlInfoOut = new IPv6ControlInfo;
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        ctrlInfoOut->setProtocol(IP_PROT_IPv6_ICMP);
        ctrlInfoOut->setSrcAddr(myNetwAddr);
        ctrlInfoOut->setDestAddr(Parents[VersionNember][0].ParentId);  //ctrlInfo->setDestAddr(PrParent)
        EV << "A DAO message is sent from : " << ctrlInfoOut->getSourceAddress() << " to parent : " << ctrlInfoOut->getDestinationAddress() << " to advertise prefix : " << prefix << "with prefix len : " << prefixLen << endl;
        pkt->setControlInfo(ctrlInfoOut);
        send(pkt, icmpv6OutGateId);
    }
    else{
        EV<< "DAO can not be sent. There is no preferred parent." << endl;
        delete msg;
    }

    delete ctrlInfoIn;

    EV << "<-RPLRouting::handleIncommingDAOMessage()" << endl;

}

void RPLRouting::sendDAOMessage(simtime_t lifetime)
{

    EV << "->RPLRouting::sendDAOMessage()" << endl;

    if (lifetime != ZERO_LIFETIME)
        if (NofParents[VersionNember] > 0){
            ICMPv6DAOMsg* pkt = new ICMPv6DAOMsg("DAO", DAO);
            IPv6ControlInfo *ctrlInfo = new IPv6ControlInfo;
            ctrlInfo->setProtocol(IP_PROT_IPv6_ICMP);
            ctrlInfo->setSrcAddr(myNetwAddr);
            ctrlInfo->setDestAddr(Parents[VersionNember][0].ParentId);  //ctrlInfo->setDestAddr(PrParent)
            pkt->setByteLength(DAOheaderLength);
            pkt->setDODAGID(DODAGID);
            pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
            pkt->setPrefixLen(64); //link local address
            pkt->setPrefix(myNetwAddr);
            pkt->setLifeTime(lifetime);
            pkt->setControlInfo(ctrlInfo);
            send(pkt, icmpv6OutGateId);
        }else
            EV << "Cancel a generated DAO, there is no preferred parent." << endl;
    else
        EV << "Cancel a generated DAO, life time is zero." << endl;

    EV << "<-RPLRouting::sendDAOMessage()" << endl;

}

void RPLRouting::handleLowerControl(cMessage *msg)
{
    delete msg;
}

void RPLRouting::handleUpperMsg(cMessage* msg)
{
    delete msg;
}

void RPLRouting::finish()
{

}

int RPLRouting::IsParent(const IPv6Address& id,int idrank)
{
    for(int i=0;i<NofParents[VersionNember];i++)
      if (Parents[VersionNember][i].ParentId==id)
      {
          if (Parents[VersionNember][i].ParentRank==idrank)
           return(EXIST);
         else
           return(SHOULD_BE_UPDATED);
      }
    return(NOT_EXIST);
}

int RPLRouting::getParentIndex(const IPv6Address& id)
{
    for(int i=0;i<NofParents[VersionNember];i++)
      if (Parents[VersionNember][i].ParentId==id)
          return(i);
    return(-1);
}

void RPLRouting::AddParent(const IPv6Address& id,int idrank, unsigned char dtsn)
{
    if(!hasRoute[VersionNember]){
        hasRoute[VersionNember] = true;
        NodeStateLast->numPreferedParents_Upward++; // next condition is not a good place for this statement because when deleteParent() increaments NofParents[VersionNember], the condition is true, and numPreferedParents increaments twice or more
    }
    if(NofParents[VersionNember]==0)
    {
        Parents[VersionNember][0].ParentId=id;
        Parents[VersionNember][0].ParentRank=idrank;
        Parents[VersionNember][0].dtsn = dtsn;
        PrParent=Parents[VersionNember][0].ParentId;
        Rank=Parents[VersionNember][0].ParentRank+1;
        NofParents[VersionNember]++;
        NodeStateLast->numParents_Upward++;
        return;
    }else
    {
        if (NofParents[VersionNember]==MaxNofParents)
        {
           if (idrank >= Parents[VersionNember][NofParents[VersionNember]-1].ParentRank) return;
           else{
               NofParents[VersionNember]--;
               NodeStateLast->numParents_Upward--;
           }
        }
        int i=NofParents[VersionNember]-1;
        while((i>=0)&&(idrank<Parents[VersionNember][i].ParentRank))
        {
            Parents[VersionNember][i+1]=Parents[VersionNember][i];
            i--;
        }
        Parents[VersionNember][i+1].ParentId=id;
        Parents[VersionNember][i+1].ParentRank=idrank;
        Parents[VersionNember][i+1].dtsn = dtsn;
        PrParent=Parents[VersionNember][0].ParentId;
        Rank=Parents[VersionNember][0].ParentRank+1;
        NofParents[VersionNember]++;
        NodeStateLast->numParents_Upward++;
    }
    return;
}
void RPLRouting::DeleteParent(const IPv6Address& id)
{
    for(int i=0;i<NofParents[VersionNember];i++)
        if (Parents[VersionNember][i].ParentId==id)
        {
            for(int j=i;j<NofParents[VersionNember]-1;j++)
                Parents[VersionNember][j]=Parents[VersionNember][j+1];
            break;
        }
    NofParents[VersionNember]--;
    NodeStateLast->numParents_Upward--;
}

bool RPLRouting::IsNeedDAO(const IPv6Address parent, unsigned char dtsn)
{
    if ((Parents[VersionNember][0].ParentId == parent) && (dtsn > Parents[VersionNember][0].dtsn))
            return true;
    return false;

}
RPLRouting::DIOStatus* RPLRouting::CreateNewVersionDIO()
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

RPLRouting::DISStatus* RPLRouting::CreateNewVersionDIS()
{
    DISStatus* Temp;
    Temp = new DISStatus;
    Temp->nbDISSent=0;
    Temp->nbDISReceived=0;
    Temp->nbDISSuppressed=0;
    Temp->link=NULL;
    return Temp;
}
void RPLRouting::DISHandler()
{
    Enter_Method("DISHandler()");
    DISVersion = Version;
    DISStatusNew = CreateNewVersionDIS();
    if(DISStatusHeader == NULL)
    {
        DISStatusLast = DISStatusNew;
        DISStatusHeader = DISStatusNew;
    }
    else
    {
        DISStatusLast->link = DISStatusNew;
        DISStatusLast = DISStatusNew;
    }
}

RPLRouting::DODAGJoiningtime* RPLRouting::CreateNewVersionJoiningTime()
{
    DODAGJoiningtime* Temp;
    Temp = new DODAGJoiningtime;
    Temp->version=VersionNember;
    Temp->link=NULL;
    return Temp;
}

bool RPLRouting::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
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

RPLRouting::~RPLRouting()
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
