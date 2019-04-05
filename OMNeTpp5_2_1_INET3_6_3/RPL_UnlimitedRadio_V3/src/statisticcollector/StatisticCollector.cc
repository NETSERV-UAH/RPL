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
#include "src/networklayer/icmpv6/IPv6NeighbourDiscoveryRPL.h"
//#include "src/networklayer/icmpv6/IPv6NeighbourCacheRPL.h"
#include <algorithm>    // std::sort


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

void StatisticCollector::registNode(cModule *host, RPLUpwardRouting *pRPLUpwardRouting, ParentTableRPL *parentTableRPL, IPv6RoutingTable *routingTable, SourceRoutingTable *sourceRoutingTable, IPv6Address linlklocalAddress, IPv6Address globalAddress)
{
    Enter_Method("registNode()");

    unsigned int vectorIndex = rplManager->getIndexFromLLAddress(linlklocalAddress);
    if(nodeStateList.size() < vectorIndex + 1)
        nodeStateList.resize(vectorIndex + 1);
    nodeStateList.at(vectorIndex).host = host;
    nodeStateList.at(vectorIndex).pRPLUpwardRouting = pRPLUpwardRouting;
    nodeStateList.at(vectorIndex).parentTableRPL = parentTableRPL;
    nodeStateList.at(vectorIndex).routingTable = routingTable;
    nodeStateList.at(vectorIndex).sourceRoutingTable = sourceRoutingTable;
    nodeStateList.at(vectorIndex).linklocalAddress = linlklocalAddress;
    nodeStateList.at(vectorIndex).globalAddress = globalAddress;
    nodeStateList.at(vectorIndex).nodeIndex = vectorIndex;
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
        if (convergenceTimeEndUpward < time){
            convergenceTimeEndUpward = time;
        }
        EV << "Node" << nodeID << "(Link Local Address: " << nodeStateList.at(nodeID).linklocalAddress << ", Global Address: " << nodeStateList.at(nodeID).globalAddress << " joins to DODAG, and has an Upward route to the root node." << endl;
    }
    if (isConverged()){
        EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
        collectOtherMetrics();
        saveStatistics();
        //if(numberOfDODAGformationNormal == numberOfIterations){
        if(numberOfGlogalRepaires + 1 == numberOfIterations){
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

    unsigned int vectorIndex = 0;
    bool found = false;
    while(vectorIndex < nodeStateList.size() && (!found)){
        if (nodeStateList.at(vectorIndex).globalAddress == globalAddress){
            found = true;
            nodeJoinedDownnward(vectorIndex, time);
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
        if (convergenceTimeEndDownward < time){
            convergenceTimeEndDownward = time;
        }
        EV << "Root has a Downward route to Node" << nodeID << "(Link Local Address: " << nodeStateList.at(nodeID).linklocalAddress << ", Global Address: " << nodeStateList.at(nodeID).globalAddress << "." << endl;
        if (isConverged()){
            EV << "This node is the last node that joined DODAG! DODAG formed!!" << endl;
            collectOtherMetrics();
            saveStatistics();
            //if(numberOfDODAGformationNormal == numberOfIterations){
            if(numberOfGlogalRepaires + 1 == numberOfIterations){
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
    for (unsigned int i = 0; i < nodeStateList.size(); i++){
        if (nodeStateList.at(i).linklocalAddress == ip)
            nodeStateList.at(i).rank = rank;
    }
}
/*
void StatisticCollector::messageAction(messagesTypes type, MessageAction action){

    if (type == DIO){
        if (action == sent)
            numSentDIO++;
        else if (action == received)
            numReceivedDIO++;
        else if (action == suppressed)
            numSuppressedDIO++;
//    }else if (type == DIS_UNICAST){
        //Nothing, not imlemented
//
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
*/
void StatisticCollector::collectOtherMetrics()
{
    for (unsigned int i = 0; i < nodeStateList.size(); i++){
        //Collects message statistics
        nodeStateList.at(i).pRPLUpwardRouting->getDIOStatistics(nodeStateList.at(i).numSentDIO, nodeStateList.at(i).numReceivedDIO, nodeStateList.at(i).numSuppressedDIO);
        numSentDIO += nodeStateList.at(i).numSentDIO;
        numReceivedDIO += nodeStateList.at(i).numReceivedDIO;
        numSuppressedDIO += nodeStateList.at(i).numSuppressedDIO;

        check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->getDISStatistics(nodeStateList.at(i).numSentDIS, nodeStateList.at(i).numReceivedDIS, nodeStateList.at(i).numSuppressedDIS);
        numSentDIS += nodeStateList.at(i).numSentDIS;
        numReceivedDIS += nodeStateList.at(i).numReceivedDIS;
        numSuppressedDIS += nodeStateList.at(i).numSuppressedDIS;

        check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->getDAOStatistics(nodeStateList.at(i).numSentDAO, nodeStateList.at(i).numReceivedDAO);  //, nodeStateList.at(i).numSuppressedDAO);
        numSentDAO += nodeStateList.at(i).numSentDAO;
        numReceivedDAO += nodeStateList.at(i).numReceivedDAO;
        //numSuppressedDAO += nodeStateList.at(i).numSuppressedDAO;

        //Collects tables statistics
        //numberOfNeighbors
        nodeStateList.at(i).numberOfNeighbors = check_and_cast<IPv6NeighbourDiscoveryRPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("neighbourDiscovery"))->neighbourCache.getNumberOfNeighbores();
        numberOfNeighbors += nodeStateList.at(i).numberOfNeighbors;
        nodeStateList.at(i).numberOfParents = nodeStateList.at(i).parentTableRPL->getNumberOfParents();
        numberOfParents += nodeStateList.at(i).numberOfParents;
        nodeStateList.at(i).numberOfRoutes = nodeStateList.at(i).routingTable->getNumRoutes();
        numberOfRoutes += nodeStateList.at(i).numberOfRoutes;
        nodeStateList.at(i).numberOfSRRoutes = nodeStateList.at(i).sourceRoutingTable->getNumberOfRoutes();
        numberOfSRRoutes += nodeStateList.at(i).numberOfSRRoutes;
    }
}

bool StatisticCollector::isConverged()
{
    for (unsigned int i=0; i<nodeStateList.size(); i++){
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

    numSentDIO = 0;
    numReceivedDIO = 0;
    numSuppressedDIO = 0;
    numSentDIS = 0;
    numReceivedDIS = 0;
    numSuppressedDIS = 0;
    numSentDAO = 0;
    numReceivedDAO = 0;
    //numSuppressedDAO = 0;
    numberOfNeighbors = 0;
    numberOfParents = 0;
    numberOfRoutes = 0;
    numberOfSRRoutes = 0;
    averageNumberofHopCount = 0;

    for (unsigned int i=0; i<nodeStateList.size(); i++){
        //Reset statistics for each node
        nodeStateList.at(i).joiningTimeUpward = SIMTIME_ZERO;
        nodeStateList.at(i).joiningTimeDownward = SIMTIME_ZERO;
        nodeStateList.at(i).isJoinUpward = false;
        nodeStateList.at(i).isJoinDownward = false;

        nodeStateList.at(i).numSentDIO = 0;
        nodeStateList.at(i).numReceivedDIO = 0;
        nodeStateList.at(i).numSuppressedDIO = 0;
        nodeStateList.at(i).numSentDIS = 0;
        nodeStateList.at(i).numReceivedDIS = 0;
        nodeStateList.at(i).numSuppressedDIS = 0;
        nodeStateList.at(i).numSentDAO = 0;
        nodeStateList.at(i).numReceivedDAO = 0;
        //nodeStateList.at(i).numSuppressedDAO = 0;
        nodeStateList.at(i).numberOfNeighbors = 0;
        nodeStateList.at(i).numberOfParents = 0;
        nodeStateList.at(i).numberOfRoutes = 0;
        nodeStateList.at(i).numberOfSRRoutes = 0;
        check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->resetStatistics();

        //Reset the tables
        check_and_cast<IPv6NeighbourDiscoveryRPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("neighbourDiscovery"))->neighbourCache.clearTable();
        nodeStateList.at(i).parentTableRPL->clearTable();
        nodeStateList.at(i).routingTable->deleteAllRoutes();

        nodeStateList.at(i).pRPLUpwardRouting->setParametersBeforeGlobalRepair(convergenceTimeStart);
        //nodeStateList.at(i).pRPLUpwardRouting->DeleteDIOTimer(); //in RPLUpwardRouting::setParametersBeforeGlobalRepair()

        if (i != sinkID){
            if (nodeStateList.at(i).pRPLUpwardRouting->par("DISEnable").boolValue()){
                check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->SetDISParameters(convergenceTimeStart);
                check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->scheduleNextDISTransmission();
            }
            if (mop != No_Downward_Routes_maintained_by_RPL){
                check_and_cast<ICMPv6RPL *>(nodeStateList.at(i).pRPLUpwardRouting->getParentModule()->getSubmodule("icmpv6"))->DeleteDAOTimers();
            }

        }else if (mop == Non_Storing_Mode_of_Operation){
            nodeStateList.at(i).sourceRoutingTable->clearTable();
        }
    }
    nodeJoinedUpward(sinkID, convergenceTimeStart);
    nodeJoinedDownnward(sinkID, convergenceTimeStart);

    cancelEvent(globalRepairTimer);
    scheduleAt(convergenceTimeStart + globalRepairInterval, globalRepairTimer);
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

void StatisticCollector::calculateHopCount()
{
    //Initialize the map array
    mapping.resize(nodeStateList.size());
    for (unsigned int i = 0; i < mapping.size(); i++){
        mapping.at(i).nodeIndex = nodeStateList.at(i).nodeIndex;
        mapping.at(i).rank = nodeStateList.at(i).rank - 1;  //The root rank is 1, but we need that the rank is started from 0 for the hop count calculation
    }

    //Sort map in increasing order of rank
    sort(mapping.begin(), mapping.end(), mapCompare);

    //Initialize hopCountMat
    hopCountMat.resize(nodeStateList.size());
    for (unsigned int i = 0; i < hopCountMat.size(); i++){
        hopCountMat.at(i).resize(nodeStateList.size());
    }
    for (unsigned int i = 0; i < hopCountMat.size(); i++){
        for (unsigned int j = 0; j < hopCountMat.at(i).size(); j++){
            if (i == j)
                hopCountMat.at(i).at(j) = 0;
            else
                hopCountMat.at(i).at(j) = -1;
        }
    }

    //Insert rank to hopCountMat
    /* Inserting the rank is not necessary.
     * The algorithm can calculate the rank based on the adjacency/(or preferred parent) info.
     */
/*    for (unsigned int i = 0; i < hopCountMat.size(); i++){
        hopCountMat.at(0).at(i) = hopCountMat.at(i).at(0) = mapping.at(i).rank;  //For storing mode
    }
*/

    //Insert adjacency(or preferred parent) to hopCountMat
    for (unsigned int i = 0; i < nodeStateList.size(); i++){
        IPv6Address prefParent = nodeStateList.at(i).parentTableRPL->getPrefParentIPAddress();
        if (prefParent != IPv6Address::UNSPECIFIED_ADDRESS){  //because root has not any prefparent
            if (mop == Storing_Mode_of_Operation_with_no_multicast_support){
                hopCountMat.at(nodeIndexToOrderedIndex(i)).at(nodeIndexToOrderedIndex(rplManager->getIndexFromLLAddress(prefParent))) = hopCountMat.at(nodeIndexToOrderedIndex(rplManager->getIndexFromLLAddress(prefParent))).at(nodeIndexToOrderedIndex(i)) = 1;  //DAO parent & preferred parent are alike. Upward & Downward adjacency.
            }else if ((mop == Non_Storing_Mode_of_Operation) || (mop == No_Downward_Routes_maintained_by_RPL)){
                hopCountMat.at(nodeIndexToOrderedIndex(i)).at(nodeIndexToOrderedIndex(rplManager->getIndexFromLLAddress(prefParent))) = 1;  // For a adjacency between node i and preferred parent. Upward adjacency.
            }
        }
        if (mop == Non_Storing_Mode_of_Operation){
            hopCountMat.at(0).at(i) = mapping.at(i).rank;  // For a n-hop adjacency between node 0/root and Node i. Downward adjacency.
        }
    }


    //Calculate other elements of the hopCountArray

    if (mop == No_Downward_Routes_maintained_by_RPL){
        for (unsigned int i = 0; i < hopCountMat.size(); i++){ // If we had inserted the rank to hopCountMat, the first column and row have been filled already, so it could be "unsigned int i = 1";
            for (unsigned int j = 0; j < hopCountMat.size(); j++){
                if (hopCountMat.at(i).at(j) == -1){
                    hopCountMat.at(i).at(j) = minHopCount (i, j);
                }
            }
        }
    }else if (mop == Storing_Mode_of_Operation_with_no_multicast_support){
        for (unsigned int i = 0; i < hopCountMat.size(); i++){ // If we had inserted the rank to hopCountMat, the first column and row have been filled already, so it could be "unsigned int i = 1";
            for (unsigned int j = i + 1; j < hopCountMat.size(); j++){
                if (hopCountMat.at(i).at(j) == -1){
                    hopCountMat.at(i).at(j) = hopCountMat.at(j).at(i) = minHopCount (i, j);
                }
            }
        }
    }else if (mop == Non_Storing_Mode_of_Operation){
        for (unsigned int i = 0; i < hopCountMat.size(); i++){
            for (unsigned int j = 0; j < hopCountMat.size(); j++){
                if (hopCountMat.at(i).at(j) == -1){
                    hopCountMat.at(i).at(j) = minHopCount (i, j);
                }
            }
        }
    }

}

int StatisticCollector::nodeIndexToOrderedIndex(int nodeIndex)
{
    for (unsigned int i = 0; i < nodeStateList.size(); i++){
        if (mapping.at(i).nodeIndex == nodeIndex){
            return i;
        }
    }
    throw cRuntimeError("StatisticCollector::nodeIndexToOrderedIndex(%d) is out of range!", nodeIndex);
}

int StatisticCollector::orderedIndexToNodeIndex(unsigned int orderedIndex)
{
    if ((orderedIndex >= 0) && (orderedIndex < nodeStateList.size()))
        return mapping.at(orderedIndex).nodeIndex;
    throw cRuntimeError("StatisticCollector::orderedIndexToNodeIndex(%d) is out of range!", orderedIndex);
}

int StatisticCollector::minHopCount(int nodei, int nodej)
{
    int firstCommonAncestor = -1;  // nodeIndexToOrderedIndex(sinkID); //We assume the first common ancestor is the root node. Then, we update it.
    int minHopCount = -1;  // hopCountMat.at(nodei).at(firstCommonAncestor) + hopCountMat.at(nodej).at(firstCommonAncestor);
    for (unsigned int i = 0; i < hopCountMat.size(); i++){
        if ((mop == Storing_Mode_of_Operation_with_no_multicast_support) || (mop == Non_Storing_Mode_of_Operation) || (mop == No_Downward_Routes_maintained_by_RPL)){
            /* For Storing Mode, both the following approach is true
             * because storing mode potentially uses symmetric paths
             * in bidirectional communication.
             * Result: Storing Mode doesn't need all elements of N*N matrix. It only uses (N*N - N)/2 enties of (N*N) matrix.
             */
            //if ((hopCountMat.at(nodei).at(i) != -1) && (hopCountMat.at(nodej).at(i) != -1))
                //if ((minHopCount == -1) || (hopCountMat.at(nodei).at(i) + hopCountMat.at(nodej).at(i) < minHopCount)){
                    //firstCommonAncestor = i;
                    //minHopCount = hopCountMat.at(nodei).at(i) + hopCountMat.at(nodej).at(i);  // Hop Count(nodei-->nodej) = Hop Count(nodei-->k) + Hop Count(k-->nodej)
                //}
            if ((hopCountMat.at(nodei).at(i) != -1) && (hopCountMat.at(i).at(nodej) != -1))
                if ((minHopCount == -1) || (hopCountMat.at(nodei).at(i) + hopCountMat.at(i).at(nodej) < minHopCount)){
                    firstCommonAncestor = i;
                    minHopCount = hopCountMat.at(nodei).at(i) + hopCountMat.at(i).at(nodej);  // Hop Count(nodei-->nodej) = Hop Count(nodei-->k) + Hop Count(k-->nodej)
                }
        }
    }
    return minHopCount;
}

void StatisticCollector::saveStatistics()
{

    //Time statistics
    FILE *convergenceTimeStart, *convergenceTimeEndUpward, *convergenceTimeEndDownward,  *convergenceTimeDownward, *convergenceTimeUpward, *joiningTimeUpward, *joiningTimeDownward;

    convergenceTimeStart = fopen("convergenceTimeStart.txt", "a");
    fprintf(convergenceTimeStart, "%f\n", this->convergenceTimeStart.dbl());
    fclose(convergenceTimeStart);

    convergenceTimeEndUpward = fopen("convergenceTimeEndUpward.txt", "a");
    fprintf(convergenceTimeEndUpward, "%f\n", this->convergenceTimeEndUpward.dbl());
    fclose(convergenceTimeEndUpward);

    joiningTimeUpward = fopen("joiningTimeUpward.txt", "a");
    for (unsigned int i=0; i<nodeStateList.size(); i++){
        fprintf(joiningTimeUpward, "%f\n", nodeStateList.at(i).joiningTimeUpward.dbl());
    }
    fclose(joiningTimeUpward);

    convergenceTimeUpward = fopen("01_1_convergenceTimeUpward.txt", "a");
    fprintf(convergenceTimeUpward, "%f\n", this->convergenceTimeEndUpward.dbl() - this->convergenceTimeStart.dbl());
    fclose(convergenceTimeUpward);

    if ((mop == Storing_Mode_of_Operation_with_no_multicast_support) || (mop == Non_Storing_Mode_of_Operation)){
        convergenceTimeEndDownward = fopen("convergenceTimeEndDownward.txt", "a");
        fprintf(convergenceTimeEndDownward, "%f\n", this->convergenceTimeEndDownward.dbl());
        fclose(convergenceTimeEndDownward);

        joiningTimeDownward = fopen("joiningTimeDownward.txt", "a");
        for (unsigned int i=0; i<nodeStateList.size(); i++){
            fprintf(joiningTimeDownward, "%f\n", nodeStateList.at(i).joiningTimeDownward.dbl());
        }
        fclose(joiningTimeDownward);

        convergenceTimeDownward = fopen("01_2_convergenceTimeDownward.txt", "a");
        fprintf(convergenceTimeDownward, "%f\n", this->convergenceTimeEndDownward.dbl() - this->convergenceTimeStart.dbl());
        fclose(convergenceTimeDownward);
    }

    //Message statistics
    FILE *averageSentDIO, *averageSentDIS, *averageSentDAO, *averageNumberOfMessages;
    double averageDIO = 0, averageDIS = 0, averageDAO = 0;

    averageSentDIO = fopen("averageSentDIO.txt", "a");
    averageDIO = (double)(numSentDIO) / nodeStateList.size();
    fprintf(averageSentDIO, "%f\n", averageDIO);
    fclose(averageSentDIO);

    averageSentDIS = fopen("averageSentDIS.txt", "a");
    averageDIS = (double)(numSentDIS) / nodeStateList.size();
    fprintf(averageSentDIS, "%f\n", averageDIS);
    fclose(averageSentDIS);

    if ((mop == Storing_Mode_of_Operation_with_no_multicast_support) || (mop == Non_Storing_Mode_of_Operation)){
        averageSentDAO = fopen("averageSentDAO.txt", "a");
        averageDAO = (double)(numSentDAO) / nodeStateList.size();
        fprintf(averageSentDAO, "%f\n", averageDAO);
        fclose(averageSentDAO);
    }

    averageNumberOfMessages = fopen("02_averageNumberOfMessages.txt", "a");
    fprintf(averageNumberOfMessages, "%f\n", averageDIO + averageDIS + averageDAO);
    fclose(averageNumberOfMessages);

    //Table statistics
    FILE *averageNumberOfNeighborsFP, *averageNumberOfParentsFP, *averageNumberOfRoutesFP, *averageNumberOfSRRoutesFP, *averageNumberofTableEntriesFP;
    double averageNumberOfNeighbors, averageNumberOfParents, averageNumberOfRoutes, averageNumberOfSRRoutes;

    averageNumberOfNeighbors = (double) (numberOfNeighbors) / nodeStateList.size();
    averageNumberOfNeighborsFP = fopen("averageNumberOfNeighbors.txt", "a");
    fprintf(averageNumberOfNeighborsFP, "%f\n", averageNumberOfNeighbors);
    fclose(averageNumberOfNeighborsFP);

    averageNumberOfParents = (double) (numberOfParents) / nodeStateList.size();
    averageNumberOfParentsFP = fopen("averageNumberOfParents.txt", "a");
    fprintf(averageNumberOfParentsFP, "%f\n", averageNumberOfParents);
    fclose(averageNumberOfParentsFP);

    averageNumberOfRoutes = (double)(numberOfRoutes) / nodeStateList.size();
    averageNumberOfRoutesFP = fopen("averageNumberOfRoutes.txt", "a");
    fprintf(averageNumberOfRoutesFP, "%f\n", averageNumberOfRoutes);
    fclose(averageNumberOfRoutesFP);

    if (mop == Non_Storing_Mode_of_Operation){
        averageNumberOfSRRoutes = (double)(numberOfSRRoutes) / nodeStateList.size();
        averageNumberOfSRRoutesFP = fopen("averageNumberOfSRRoutes.txt", "a");
        fprintf(averageNumberOfSRRoutesFP, "%f\n", averageNumberOfSRRoutes);
        fclose(averageNumberOfSRRoutesFP);
    }

    averageNumberofTableEntriesFP = fopen("03_averageNumberofTableEntries.txt", "a");
    //fprintf(averageNumberofTableEntriesFP, "%f\n", averageNumberOfNeighbors + averageNumberOfParents + averageNumberOfDefaultRoutes + averageNumberOfRoutes);
    fprintf(averageNumberofTableEntriesFP, "%f\n", averageNumberOfNeighbors + averageNumberOfParents + averageNumberOfRoutes);
    fclose(averageNumberofTableEntriesFP);

    //Hop count statistics
    if ((mop == Storing_Mode_of_Operation_with_no_multicast_support) || (mop == Non_Storing_Mode_of_Operation) || (mop == No_Downward_Routes_maintained_by_RPL)){
        calculateHopCount();

        FILE *numberofHopCountFP, *averageNumberofHopCountFP;
        numberofHopCountFP = fopen("numberofHopCount.txt", "a");
        averageNumberofHopCount = 0;
        int numflows = 0;
        for (unsigned int i = 0; i < nodeStateList.size(); i++){
            for (unsigned int j = 0; j < nodeStateList.size(); j++){
                fprintf(numberofHopCountFP, "%3d\t", hopCountMat.at(nodeIndexToOrderedIndex(i)).at(nodeIndexToOrderedIndex(j)));  //hopCountMat must be converted from ordered to non-ordered
                if ((i != j) && (hopCountMat.at(nodeIndexToOrderedIndex(i)).at(nodeIndexToOrderedIndex(j)) != -1)){
                    averageNumberofHopCount += hopCountMat.at(nodeIndexToOrderedIndex(i)).at(nodeIndexToOrderedIndex(j));
                    numflows++; // Finally, numflows will be nodeStateList.size() ^ 2 - nodeStateList.size()
                }
            }
            fprintf(numberofHopCountFP, "\n");
        }
        fprintf(numberofHopCountFP, "\n -------------------------------------------------------------\n");
        fclose(numberofHopCountFP);
        averageNumberofHopCount /= numflows;

        averageNumberofHopCountFP = fopen("04_averageNumberofHopCount.txt", "a");
        fprintf(averageNumberofHopCountFP, "%f\n", averageNumberofHopCount);
        fclose(averageNumberofHopCountFP);
    }

    //Other statistics
    //FILE *preferedParent, *nodeRank;
}
StatisticCollector::~StatisticCollector()
{
    cancelAndDelete(globalRepairTimer);


}

} // namespace rpl

