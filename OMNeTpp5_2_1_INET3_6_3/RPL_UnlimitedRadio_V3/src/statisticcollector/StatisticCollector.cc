/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    (3) UPC, Castelldefels, Spain.
 *
 */
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

#include <vector>
#include "src/statisticcollector/StatisticCollector.h"

namespace rpl {
using namespace inet;

Define_Module(StatisticCollector);

    //if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward)) NodeStateLast->DIO.Received++;  //if simulation is not end ...

void StatisticCollector::registNode(cModule *hostModule, IPv6Address linlklocalAddress, IPv6Address globalAddress)
{
    int vectorIndex = rplManager->getIndexFromLLAddress(linlklocalAddress);
    if(nodeStateList.size() < vectorIndex + 1)
        nodeStateList.resize(vectorIndex + 1);
    nodeStateList.at(vectorIndex).hostModule = hostModule;
    nodeStateList.at(vectorIndex).linklocalAddress = linlklocalAddress;
    nodeStateList.at(vectorIndex).globalAddress = globalAddress;
}

void StatisticCollector::startStatistics(RPLMOP mop, IPv6Address sinkLLAddress, simtime_t time)
{
    //this->version = version;
    this->mop = mop;

    if(GlobalRepairTimer!=0)
    {
        GRepairTimer = new cMessage("GRepair-timer", Global_REPAIR_TIMER);
        scheduleAt(GlobalRepairTimer,GRepairTimer );
    }

    setConvergenceTimeStart(sinkLLAddress, time);

}

void StatisticCollector::setConvergenceTimeStart(IPv6Address sinkLLAddress, simtime_t time)
{
    convergenceTimeStart = time;
    nodeJoinedUpward(sinkLLAddress, time);
}

//When a node receives a DIO message, it calls this method to indicate that it joined to the DAG and has a Upward route
void StatisticCollector::nodeJoinedUpward(IPv6Address linkLocalAddress, simtime_t time)
{
    int vectorIndex = rplManager->getIndexFromLLAddress(linkLocalAddress);
    nodeStateList.at(vectorIndex).isJoin_Upward = true;
    nodeStateList.at(vectorIndex).joiningTime_Upward = time;
    EV << "Node" << vectorIndex+1 << "(Link Local Address: " << nodeStateList.at(vectorIndex).linklocalAddress << ", Global Address: " << nodeStateList.at(vectorIndex).globalAddress << " joins to DODAG." << endl;
    if (mop == No_Downward_Routes_maintained_by_RPL)
        if (isConvergedUpward())
            saveStatistics();



    if((!DAOEnable) && (NodeCounter_Upward[Version]==NodesNumber) && (!IsDODAGFormed_Upward))
     {
         FileRecordCounter++;
         host->bubble("I'm the last node that joined DODAG! DODAG formed!!");
         IsDODAGFormed_Upward=true;
         NodeStateLast->DODAGsFormationTimeRecords_Upward = netwMsg->getArrivalTime()-dodagSartTime;
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

//When the sink/root node receives a DAO message from a node, it calls this method to indicate that the node has a Downward route.
void StatisticCollector::nodeJoinedDownnward(ip, time)
{

    if (mop == Storing_Mode_of_Operation_with_no_multicast_support)
        if (isConvergedDownward())
            saveStatistics();

}

virtual void StatisticCollector::updateRank(IPv6Address ip, int rank)
{

}


bool StatisticCollector::isConvergeedUpward()
{
    for (int i=0; i<nodeStateList.size(); i++){
        if (!nodeStateList.at(i).isJoinUpward)
            return false;
    }
    return true;
}

bool StatisticCollector::isConvergedDownward()
{
    for (int i=0; i<nodeStateList.size(); i++){
        if (!nodeStateList.at(i).isJoinDownward)
            return false;
    }
    return true;
}

void StatisticCollector::ScheduleNextGlobalRepair()
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

    dodagSartTime=DODAGJoinTimeLast_Upward->TimetoJoinDODAG;
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
    DIO_StofCurIntNext=dodagSartTime;
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

void StatisticCollector::DeleteScheduledNextGlobalRepair()
{
    Enter_Method("DeleteScheduledNextGlobalRepair()");

    cancelEvent(GRepairTimer);
    scheduleAt(simTime(),GRepairTimer );
}

void StatisticCollector::handleGlobalRepairTimer(cMessage* msg)
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

void StatisticCollector::saveStatistics()
{

    //Time statistics
    FILE *convergenceTimeStart, *convergenceTimeEndUpward, *convergenceTimeEndDownward, *joiningTimeUpward, *joiningTimeDownward;

    convergenceTimeStart = fopen("convergenceTimeStart.txt", "a");
    fprintf(convergenceTimeStart, "%f\n", this->convergenceTimeStart);
    fclose(convergenceTimeStart);

    convergenceTimeEndUpward = fopen("convergenceTimeEndUpward.txt", "a");
    fprintf(convergenceTimeEndUpward, "%f\n", this->convergenceTimeEndUpward);
    fclose(convergenceTimeEndUpward);

    convergenceTimeEndDownward = fopen("01_convergenceTimeEndDownward.txt", "a");
    fprintf(convergenceTimeEndDownward, "%f\n", this->convergenceTimeEndDownward);
    fclose(convergenceTimeEndDownward);

    joiningTimeUpward = fopen("joiningTimeUpward.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        fprintf(joiningTimeUpward, "%f\n", nodeStateList.at(i).joiningTimeUpward);
    }
    fclose(joiningTimeUpward);

    joiningTimeDownward = fopen("joiningTimeDownward.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        fprintf(joiningTimeDownward, "%f\n", nodeStateList.at(i).joiningTimeDownward);
    }
    fclose(joiningTimeDownward);

    //Message statistics
    FILE *averageSentDIO, *averageSentDIS, *averageSentDAO, *averageNumberOfMessages;
    float averageDIO = 0, averageDIS = 0, averageDAO = 0;

    averageSentDIO = fopen("averageSentDIO.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageDIO += nodeStateList.at(i).hostModule->getNumberOfsentDIOs();
    }
    averageDIO /= nodeStateList.size();
    fprintf(averageSentDIO, "%f\n", averageDIO);
    fclose(averageSentDIO);

    averageSentDIS = fopen("averageSentDIS.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageDIS += nodeStateList.at(i).hostModule->getNumberOfsentDISs();
    }
    averageDIS /= nodeStateList.size();
    fprintf(averageSentDIS, "%f\n", averageDIS);
    fclose(averageSentDIS);

    averageSentDAO = fopen("averageSentDAO.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageDAO += nodeStateList.at(i).hostModule->getNumberOfsentDAOs();
    }
    averageDAO /= nodeStateList.size();
    fprintf(averageSentDAO, "%f\n", averageDAO);
    fclose(averageSentDAO);


    averageNumberOfMessages = fopen("02_averageNumberOfMessages.txt", "a");
    fprintf(averageNumberOfMessages, "%f\n", averageDIO + averageDIS + averageDAO);
    fclose(averageNumberOfMessages);

    //Table statistics
    FILE *averageNumberOfNeighborsFP, *averageNumberOfParentsFP, *averageNumberOfDefaultRoutesFP, *averageNumberOfRoutesFP, *averageNumberofTableEntriesFP;
    float averageNumberOfNeighbors, averageNumberOfParents, averageNumberOfDefaultRoutes, averageNumberOfRoutes;

    averageNumberOfNeighbors = 0;
    averageNumberOfNeighborsFP = fopen("averageNumberOfNeighbors.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageNumberOfNeighbors += nodeStateList.at(i).hostModule->getsubmodule()->getNumberOfNeighbors();
    }
    averageNumberOfNeighbors /= nodeStateList.size();
    fprintf(averageNumberOfNeighborsFP, "%f\n", averageNumberOfNeighbors);
    fclose(averageNumberOfNeighborsFP);

    averageNumberOfParents = 0;
    averageNumberOfParentsFP = fopen("averageNumberOfParents.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageNumberOfParents += nodeStateList.at(i).hostModule->getsubmodule()->getNumberOfParents();
    }
    averageNumberOfParents /= nodeStateList.size();
    fprintf(averageNumberOfParentsFP, "%f\n", averageNumberOfParents);
    fclose(averageNumberOfParentsFP);

    averageNumberOfDefaultRoutes = 0;
    averageNumberOfDefaultRoutesFP = fopen("averageNumberOfDefaultRoutes.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageNumberOfDefaultRoutes += nodeStateList.at(i).hostModule->getsubmodule()->getNumberOfDefaultRoutes();
    }
    averageNumberOfDefaultRoutes /= nodeStateList.size();
    fprintf(averageNumberOfDefaultRoutesFP, "%f\n", averageNumberOfDefaultRoutes);
    fclose(averageNumberOfDefaultRoutesFP);

    averageNumberOfRoutes = 0;
    averageNumberOfRoutesFP = fopen("averageNumberOfRoutes.txt", "a");
    for (int i=0; i<nodeStateList.size(); i++){
        averageNumberOfRoutes += nodeStateList.at(i).hostModule->getsubmodule()->getNumberOfRoutes();
    }
    averageNumberOfRoutes /= nodeStateList.size();
    fprintf(averageNumberOfRoutesFP, "%f\n", averageNumberOfRoutes);
    fclose(averageNumberOfRoutesFP);

    averageNumberofTableEntriesFP = fopen("03_averageNumberofTableEntries.txt", "a");
    fprintf(averageNumberofTableEntriesFP, "%f\n", averageNumberOfNeighbors + averageNumberOfParents + averageNumberOfDefaultRoutes + averageNumberOfRoutes);
    fclose(averageNumberofTableEntriesFP);

    //Other statistics
    //FILE *preferedParent, *nodeRank;
}
StatisticCollector::~StatisticCollector()
{
    cancelAndDelete(GRepairTimer);


}

} // namespace rpl

