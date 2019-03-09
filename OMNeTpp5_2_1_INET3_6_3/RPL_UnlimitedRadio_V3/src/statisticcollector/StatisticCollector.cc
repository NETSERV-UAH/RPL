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

    if (globalRepairInterval != 0)
    {
        globalRepairTimer = new cMessage("globalRepairTimer", Global_REPAIR_TIMER);
        scheduleAt(globalRepairInterval, globalRepairTimer );
    }

    convergenceTimeStart = time;
    nodeJoinedUpward(sinkLLAddress, time);
    sinkID = rplManager->getIndexFromLLAddress(sinkLLAddress);

}

//When a node receives a DIO message, it calls this method to indicate that it joined to the DAG and has a Upward route
void StatisticCollector::nodeJoinedUpward(IPv6Address linkLocalAddress, simtime_t time)
{
    int vectorIndex = rplManager->getIndexFromLLAddress(linkLocalAddress);
    if(!nodeStateList.at(vectorIndex).isJoin_Upward){
        nodeStateList.at(vectorIndex).isJoin_Upward = true;
        nodeStateList.at(vectorIndex).joiningTime_Upward = time;
        EV << "Node" << vectorIndex+1 << "(Link Local Address: " << nodeStateList.at(vectorIndex).linklocalAddress << ", Global Address: " << nodeStateList.at(vectorIndex).globalAddress << " joins to DODAG, and has an Upward route to the root node." << endl;
    }
    if (isConverged){
        EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
        saveStatistics();
        if(numberOfGlogalRepaires == numberOfIterations){
            endSimulation();
        }else{
            scheduleNewGlobalRepair();
        }
    }
}

//When the sink/root node receives a DAO message from a node, it calls this method to indicate that the node has a Downward route.
void StatisticCollector::nodeJoinedDownnward(IPv6Address linkLocalAddress, simtime_t time)
{
    int vectorIndex = rplManager->getIndexFromLLAddress(linkLocalAddress);
    if (!nodeStateList.at(vectorIndex).isJoin_Doward){
        nodeStateList.at(vectorIndex).isJoin_Doward = true;
        nodeStateList.at(vectorIndex).joiningTime_Downward = time;
        EV << "Root has a Downward route to Node" << vectorIndex+1 << "(Link Local Address: " << nodeStateList.at(vectorIndex).linklocalAddress << ", Global Address: " << nodeStateList.at(vectorIndex).globalAddress << "." << endl;
        if (isConverged){
            EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
            saveStatistics();
            if(numberOfGlogalRepaires == numberOfIterations){
                endSimulation();
            }else{
                scheduleNewGlobalRepair();
            }
        }
    }
}

virtual void StatisticCollector::updateRank(IPv6Address ip, int rank)
{

}


bool StatisticCollector::isConverged()
{
    for (int i=0; i<nodeStateList.size(); i++){
        if (mop == No_Downward_Routes_maintained_by_RPL){
            if (!nodeStateList.at(i).isJoinUpward)
                return false;
        }else{
            if ((!nodeStateList.at(i).isJoinUpward) || (!nodeStateList.at(i).isJoinDownward))
                return false;
        }
    }

    numberOfDODAGformationNormal++;
    EV << "Number of DODAGformationNormal is " << numberOfDODAGformationNormal << endl;

    return true;
}

void StatisticCollector::ScheduleNextGlobalRepair()
{

    for (int i=0; i<nodeStateList.size(); i++){
        if (i == sinkID){
            nodeStateList.at(i).isJoinUpward = true;
            nodeStateList.at(i).isJoinDownward = true;
        }else{
            nodeStateList.at(i).isJoinUpward = false;
            nodeStateList.at(i).isJoinDownward = false;
        }
        nodeStateList.at(i).joiningTimeUpward = SIMTIME_ZERO;
        nodeStateList.at(i).joiningTimeDownward = SIMTIME_ZERO;
        nodeStateList.at(i).hostModule->DeleteDIOTimer();
        if (DISEnable){
            nodeStateList.at(i).hostModule->getParentModule()->getSubmodule("icmpv6")->SetDISParameters();
            nodeStateList.at(i).hostModule->getParentModule()->getSubmodule("icmpv6")->DISHandler();
            nodeStateList.at(i).hostModule->getParentModule()->getSubmodule("icmpv6")->scheduleNextDISTransmission();
        }
        if (mop != No_Downward_Routes_maintained_by_RPL){
            nodeStateList.at(i).hostModule->getParentModule()->getSubmodule("icmpv6")->DeleteDAOTimers();
        }
    }

    convergenceTimeStart = simTime();
    nodeJoinedUpward(sinkID, time);
    RPLUpwardRouting->setParametersBeforeGlobalRepair(convergenceTimeStart);
    nodeStateList.at(sinkID).hostModule->scheduleNextDIOTransmission();  // root node

    scheduleAt(simTime() + globalRepairInterval, globalRepairTimer );
    numberOfGlogbalRepaires++;
}

void StatisticCollector::scheduleNewGlobalRepair()
{

    cancelEvent(globalRepairTimer);
    scheduleAt(simTime(), globalRepairTimer );
}

void StatisticCollector::handleMessage(cMessage* msg)
{
    if (msg->getKind() == Global_REPAIR_TIMER)
        scheduleNewGlobalRepair();
    else{
        EV << "Unknown self message is deleted." << endl;
        delete msg;
    }

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
    cancelAndDelete(globalRepairTimer);


}

} // namespace rpl

