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
#include "src/networklayer/icmpv6/RPLUpwardRouting.h"
#include "src/networklayer/icmpv6/ICMPv6RPL.h"


namespace rpl {
using namespace inet;

Define_Module(StatisticCollector);

    //if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward)) NodeStateLast->DIO.Received++;  //if simulation is not end ...

void StatisticCollector::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL){
        rplManager = check_and_cast<ManagerRPL *>(getSimulation()->getSystemModule()->getSubmodule("managerRPL"));

        numberOfIterations = par("numberOfIterations");
        globalRepairInterval = par("globalRepairInterval");

    }

}

void StatisticCollector::registNode(cModule *host, RPLUpwardRouting *pRPLUpwardRouting, ParentTableRPL *parentTableRPL, IRoutingTable *routingTable, IPv6Address linlklocalAddress, IPv6Address globalAddress)
{
    Enter_Method("registNode()");

    int vectorIndex = rplManager->getIndexFromLLAddress(linlklocalAddress);
    if(nodeStateList.size() < vectorIndex + 1)
        nodeStateList.resize(vectorIndex + 1);
    nodeStateList.at(vectorIndex).host = host;
    nodeStateList.at(vectorIndex).pRPLUpwardRouting = pRPLUpwardRouting;
    nodeStateList.at(vectorIndex).parentTableRPL = parentTableRPL;
    nodeStateList.at(vectorIndex).routingTable = routingTable;
    nodeStateList.at(vectorIndex).linklocalAddress = linlklocalAddress;
    nodeStateList.at(vectorIndex).globalAddress = globalAddress;
}

void StatisticCollector::startStatistics(RPLMOP mop, IPv6Address sinkLLAddress, simtime_t time)
{
    Enter_Method("startStatistics()");

    //this->version = version;
    this->mop = mop;

    if (globalRepairInterval != 0)
    {
        globalRepairTimer = new cMessage("globalRepairTimer", Global_REPAIR_TIMER);
        scheduleAt(time + globalRepairInterval, globalRepairTimer);
    }

    sinkID = rplManager->getIndexFromLLAddress(sinkLLAddress);
    convergenceTimeStart = time;
    nodeJoinedUpward(sinkLLAddress, time);
    nodeJoinedDownnward(sinkID, time);

}

//When a node receives a DIO message, it calls this method to indicate that it joined to the DAG and has a Upward route
void StatisticCollector::nodeJoinedUpward(int nodeID, simtime_t time)
{
    Enter_Method("nodeJoinedUpward()");

    if(!nodeStateList.at(nodeID).isJoinUpward){
        nodeStateList.at(nodeID).isJoinUpward = true;
        nodeStateList.at(nodeID).joiningTimeUpward = time;
        EV << "Node" << nodeID << "(Link Local Address: " << nodeStateList.at(nodeID).linklocalAddress << ", Global Address: " << nodeStateList.at(nodeID).globalAddress << " joins to DODAG, and has an Upward route to the root node." << endl;
    }
    if (isConverged()){
        EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
        saveStatistics();
        if(numberOfConvergedGlogalRepaires == numberOfIterations - 1){
            endSimulation();
        }else{
            scheduleNewGlobalRepair();
        }
    }
}

void StatisticCollector::nodeJoinedUpward(IPv6Address linkLocalAddress, simtime_t time)
{
    Enter_Method("nodeJoinedUpward()");

    int vectorIndex = rplManager->getIndexFromLLAddress(linkLocalAddress);
    nodeJoinedUpward(vectorIndex, time);
}

//When the sink/root node receives a DAO message from a node, it calls this method to indicate that the node has a Downward route.
void StatisticCollector::nodeJoinedDownnward(IPv6Address globalAddress, simtime_t time)
{
    Enter_Method("nodeJoinedDownnward()");

    int vectorIndex = 0;
    bool found = false;
    while(vectorIndex < nodeStateList.size() && (!found)){
        if (nodeStateList.at(vectorIndex).globalAddress == globalAddress){
            found = true;

            if (!nodeStateList.at(vectorIndex).isJoinDownward){
                nodeStateList.at(vectorIndex).isJoinDownward = true;
                nodeStateList.at(vectorIndex).joiningTimeDownward = time;
                EV << "Root has a Downward route to Node" << vectorIndex << "(Link Local Address: " << nodeStateList.at(vectorIndex).linklocalAddress << ", Global Address: " << nodeStateList.at(vectorIndex).globalAddress << "." << endl;
                if (isConverged()){
                    EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
                    saveStatistics();
                    if(numberOfConvergedGlogalRepaires == numberOfIterations - 1){
                        endSimulation();
                    }else{
                        scheduleNewGlobalRepair();
                    }
                }
            }
        }
        vectorIndex++;
    }
}

void StatisticCollector::nodeJoinedDownnward(int nodeID, simtime_t time)
{
    Enter_Method("nodeJoinedDownnward(nodeID)");

    if (!nodeStateList.at(nodeID).isJoinDownward){
        nodeStateList.at(nodeID).isJoinDownward = true;
        nodeStateList.at(nodeID).joiningTimeDownward = time;
        EV << "Root has a Downward route to Node" << nodeID << "(Link Local Address: " << nodeStateList.at(nodeID).linklocalAddress << ", Global Address: " << nodeStateList.at(nodeID).globalAddress << "." << endl;
        if (isConverged()){
            EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
            saveStatistics();
            if(numberOfConvergedGlogalRepaires == numberOfIterations - 1){
                endSimulation();
            }else{
                scheduleNewGlobalRepair();
            }
        }
    }
}

void StatisticCollector::updateRank(IPv6Address ip, int rank)
{
    Enter_Method("updateRank()");


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

void StatisticCollector::scheduleNextGlobalRepair()
{
    convergenceTimeStart = simTime();
    convergenceTimeEndUpward = SIMTIME_ZERO; // DODAG formation time in MOP = 0.
    convergenceTimeEndDownward = SIMTIME_ZERO; // DODAG formation time in MOP = 1, 2, or 3.

    nodeJoinedUpward(sinkID, convergenceTimeStart);

    for (unsigned int i=0; i<nodeStateList.size(); i++){
        nodeStateList.at(i).pRPLUpwardRouting->setParametersBeforeGlobalRepair(convergenceTimeStart);
        //nodeStateList.at(i).pRPLUpwardRouting->DeleteDIOTimer(); //in RPLUpwardRouting::setParametersBeforeGlobalRepair()
        if (i == sinkID){
            nodeStateList.at(i).isJoinUpward = true;
            nodeStateList.at(i).isJoinDownward = true;
            //nodeStateList.at(i).pRPLUpwardRouting->scheduleNextDIOTransmission();  //in RPLUpwardRouting::setParametersBeforeGlobalRepair()
        }else{
            nodeStateList.at(i).isJoinUpward = false;
            nodeStateList.at(i).isJoinDownward = false;
            if (nodeStateList.at(i).pRPLUpwardRouting->par("DISEnable").boolValue()){
                check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->SetDISParameters(convergenceTimeStart);
                check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->scheduleNextDISTransmission();
            }
            if (mop != No_Downward_Routes_maintained_by_RPL){
                check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->DeleteDAOTimers();
            }
        }
        nodeStateList.at(i).joiningTimeUpward = SIMTIME_ZERO;
        nodeStateList.at(i).joiningTimeDownward = SIMTIME_ZERO;
    }

    cancelEvent(globalRepairTimer);
    scheduleAt(convergenceTimeStart + globalRepairInterval, globalRepairTimer);
    numberOfConvergedGlogalRepaires++;
}

//Close current Global Repair and schedule a new Global Repair.
void StatisticCollector::scheduleNewGlobalRepair()
{

    cancelEvent(globalRepairTimer);
    scheduleAt(simTime(), globalRepairTimer );
}

void StatisticCollector::handleMessage(cMessage* msg)
{
    if (msg->getKind() == Global_REPAIR_TIMER){
        numberOfGlogalRepaires++;
        scheduleNextGlobalRepair();
    }else{
        EV << "Unknown self message is deleted." << endl;
        delete msg;
    }

    return;

}

void StatisticCollector::messageAction(messagesTypes type, MessageAction action){

    if (type == DIO){
        if (action == sent)
            numSentDIO++;
        else if (action == received)
            numReceivedDIO++;
        else if (action == suppressed)
            numSuppressedDIO++;
/*    }else if (type == DIS_UNICAST){
        //Nothing, not imlemented
*/
    }else if (type == DIS_FLOOD){
        if (action == sent)
            numSentDIS++;
        else if (action == received)
            numReceivedDIS++;
        else if (action == suppressed)
            numSuppressedDIS++;
    }else if (type == DAO){
        if (action == sent)
            numSentDAO++;
        else if (action == received)
            numReceivedDAO++;
        else if (action == suppressed)
            numSuppressedDAO++;
    }

}
void StatisticCollector::saveStatistics()
{

    //Time statistics
    FILE *convergenceTimeStart, *convergenceTimeEndUpward, *convergenceTimeEndDownward, *joiningTimeUpward, *joiningTimeDownward;

    convergenceTimeStart = fopen("convergenceTimeStart.txt", "a");
    fprintf(convergenceTimeStart, "%f\n", this->convergenceTimeStart.dbl());
    fclose(convergenceTimeStart);

    convergenceTimeEndUpward = fopen("convergenceTimeEndUpward.txt", "a");
    fprintf(convergenceTimeEndUpward, "%f\n", this->convergenceTimeEndUpward.dbl());
    fclose(convergenceTimeEndUpward);

    convergenceTimeEndDownward = fopen("01_convergenceTimeEndDownward.txt", "a");
    fprintf(convergenceTimeEndDownward, "%f\n", this->convergenceTimeEndDownward.dbl());
    fclose(convergenceTimeEndDownward);

    joiningTimeUpward = fopen("joiningTimeUpward.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        fprintf(joiningTimeUpward, "%f\n", nodeStateList.at(i).joiningTimeUpward.dbl());
    }
    fclose(joiningTimeUpward);

    joiningTimeDownward = fopen("joiningTimeDownward.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        fprintf(joiningTimeDownward, "%f\n", nodeStateList.at(i).joiningTimeDownward.dbl());
    }
    fclose(joiningTimeDownward);

    //Message statistics
    FILE *averageSentDIO, *averageSentDIS, *averageSentDAO, *averageNumberOfMessages;
    float averageDIO = 0, averageDIS = 0, averageDAO = 0;

    averageSentDIO = fopen("averageSentDIO.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        averageDIO += numSentDIO;
    }
    averageDIO /= nodeStateList.size();
    fprintf(averageSentDIO, "%f\n", averageDIO);
    fclose(averageSentDIO);

    averageSentDIS = fopen("averageSentDIS.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        averageDIS += numSentDIS;
    }
    averageDIS /= nodeStateList.size();
    fprintf(averageSentDIS, "%f\n", averageDIS);
    fclose(averageSentDIS);

    averageSentDAO = fopen("averageSentDAO.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        averageDAO += numSentDAO;
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
    for (unsigned int i=0; i<nodeStateList.size(); i++){
       // averageNumberOfNeighbors += nodeStateList.at(i).pRPLUpwardRouting->getsubmodule()->getNumberOfNeighbors();
    }
    averageNumberOfNeighbors /= nodeStateList.size();
    fprintf(averageNumberOfNeighborsFP, "%f\n", averageNumberOfNeighbors);
    fclose(averageNumberOfNeighborsFP);

    averageNumberOfParents = 0;
    averageNumberOfParentsFP = fopen("averageNumberOfParents.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        averageNumberOfParents += nodeStateList.at(i).parentTableRPL->getNumberOfParents(nodeStateList.at(i).pRPLUpwardRouting->getVersion());
    }
    averageNumberOfParents /= nodeStateList.size();
    fprintf(averageNumberOfParentsFP, "%f\n", averageNumberOfParents);
    fclose(averageNumberOfParentsFP);

/*    //DefaultRoutes
    averageNumberOfDefaultRoutes = 0;
    averageNumberOfDefaultRoutesFP = fopen("averageNumberOfDefaultRoutes.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        averageNumberOfDefaultRoutes += nodeStateList.at(i).routingTable->getNum; //needs getNumDefaultRoute() method
    }
    averageNumberOfDefaultRoutes /= nodeStateList.size();
    fprintf(averageNumberOfDefaultRoutesFP, "%f\n", averageNumberOfDefaultRoutes);
    fclose(averageNumberOfDefaultRoutesFP);
*/
    //Routes
    averageNumberOfRoutes = 0;
    averageNumberOfRoutesFP = fopen("averageNumberOfRoutes.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        averageNumberOfRoutes += nodeStateList.at(i).routingTable->getNumRoutes();
    }
    averageNumberOfRoutes /= nodeStateList.size();
    fprintf(averageNumberOfRoutesFP, "%f\n", averageNumberOfRoutes);
    fclose(averageNumberOfRoutesFP);

    averageNumberofTableEntriesFP = fopen("03_averageNumberofTableEntries.txt", "a");
    //fprintf(averageNumberofTableEntriesFP, "%f\n", averageNumberOfNeighbors + averageNumberOfParents + averageNumberOfDefaultRoutes + averageNumberOfRoutes);
    fprintf(averageNumberofTableEntriesFP, "%f\n", averageNumberOfNeighbors + averageNumberOfParents + averageNumberOfRoutes);
    fclose(averageNumberofTableEntriesFP);

    //Other statistics
    //FILE *preferedParent, *nodeRank;
}
StatisticCollector::~StatisticCollector()
{
    cancelAndDelete(globalRepairTimer);


}

} // namespace rpl

