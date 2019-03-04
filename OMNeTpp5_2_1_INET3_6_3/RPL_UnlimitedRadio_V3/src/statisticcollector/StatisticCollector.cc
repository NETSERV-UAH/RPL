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

#include "../statisticcollector/StatisticCollector.h"

#include <vector>

namespace rpl {
using namespace inet;

/*
int AllDIOsSent=0,AllDIOsReceived=0,AllDIOsSuppressed=0,AvgAllDIOsSent=0;
int AvgAllDIOsReceived=0,AvgAllDIOsSuppressed=0;

int Version,NodesNumber,NumberofIterations,*NodeCounter_Upward=nullptr,GRT_Counter=0;  //initializing nullptr to deallocate memory

static int NofDODAGformationNormal=0; //test11!!
double AvgAllCollisionNarmal=0;
FILE *JoiningTime_Upward,*Collosion,*DIOSent,*DISSent,*FormationTime_Upward,*PacketLost,*NodesRank,*ConsumedPower;
FILE *IterationsNumber;
FILE *preferedParent_Upward, *numberOfParents, *numTableEntris;

RPLRouting **NodesAddress;

bool* IsNodeJoined,GlobalRepairTimerReset=false,Flg=false,IsFirsttime=true;
simtime_t DODAGSartTime,AvgDODAGFomationTime_Upward;

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



initialize
GlobalRepairTimer = par ("GlobalRepairTimer");
        double GlobalRepairTimer @unit(s) = default(10000 s);

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

        NodesAddress[pManagerRPL->getIndexFromAddress(myNetwAddr)] = this;
init routing

hasRoute = new bool[NumberofIterations+2];
for(int i=0; i<NumberofIterations+2; i++)
   hasRoute[i] = false;
NofEntry=0;


GRepairTimer = NULL;



for(int i=0;i<NumberofIterations+2;i++)
{
    NofParents[i]=0;
    for(int j=0;j<MaxNofParents;j++)
    {
        Parents[i][j].ParentId= IPv6Address::UNSPECIFIED_ADDRESS;
        Parents[i][j].ParentRank=-1;
    }
}
*/

void StatisticCollector::initialize()
{
    mop = par("...");
}
void StatisticCollector::registNode(cModule *hostModule, IPv6Address linlklocalAddress, IPv6Address globalAddress)
{
    int vectorIndex = rplManager->getIndexFromLLAddress(linlklocalAddress);
    if(nodeStateList.size() < vectorIndex + 1)
        nodeStateList.resize(vectorIndex + 1);
    nodeStateList.at(vectorIndex).hostModule = hostModule;
    nodeStateList.at(vectorIndex).linklocalAddress = linlklocalAddress;
    nodeStateList.at(vectorIndex).globalAddress = globalAddress;
}

void StatisticCollector::startStatistics(int version)
{
    this->version = version;

    if(GlobalRepairTimer!=0)
    {
        GRepairTimer = new cMessage("GRepair-timer", Global_REPAIR_TIMER);
        scheduleAt(GlobalRepairTimer,GRepairTimer );
    }

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

}

//When the sink/root node receives a DAO message from a node, it calls this method to indicate that the node has a Downward route.
void StatisticCollector::nodeJoinedDownnward(ip, time)
{

    if (mop == Storing_Mode_of_Operation_with_no_multicast_support)
        if (isConvergedDownward())
            saveStatistics();

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

}

} // namespace rpl

