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

#ifndef _RPL_SRC_STATISTIC_STATISTICCOLLECTOR_H_
#define _RPL_SRC_STATISTIC_STATISTICCOLLECTOR_H_

#include "inet/common/INETDefs.h"
#include "src/networklayer/contract/RPLDefs.h"
#include "inet/networklayer/contract/ipv6/IPv6AddressType.h"
//#include "src/networklayer/icmpv6/RPLUpwardRouting.h"
#include "src/simulationManager/ManagerRPL.h"
#include "src/networklayer/icmpv6/ParentTableRPL.h"
//#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/networklayer/ipv6/IPv6RoutingTable.h"
#include "src/networklayer/icmpv6/SourceRoutingTable.h"

namespace rpl {
using namespace inet;

class RPLUpwardRouting;

class StatisticCollector : public cSimpleModule
{
/*
    enum MessageAction{
        sent,
        received,
        suppressed
    };

*/
    //ICMPv6RPL *icmpv6RPL;
    ManagerRPL *rplManager;

    struct NodeState{
        cModule *host;
        RPLUpwardRouting *pRPLUpwardRouting;
        ParentTableRPL *parentTableRPL;
        //IRoutingTable *routingTable;
        IPv6RoutingTable *routingTable;
        SourceRoutingTable *sourceRoutingTable;
        IPv6Address linklocalAddress;
        IPv6Address globalAddress;
        int nodeIndex; //According to RPL manager module

        int rank;

        bool isJoinUpward;
        bool isJoinDownward;
        simtime_t joiningTimeUpward; // By DIO
        simtime_t joiningTimeDownward;  //By DAO
        int numSentDIO;
        int numReceivedDIO;
        int numSuppressedDIO;
        int numSentDIS;
        int numReceivedDIS;
        int numSuppressedDIS;
        int numSentDAO;
        int numReceivedDAO;
        //int numSuppressedDAO;
        int numberOfNeighbors;
        int numberOfParents;
        int numberOfRoutes;
        int numberOfSRRoutes;


        NodeState()
            : host(nullptr)
            , pRPLUpwardRouting (nullptr)
            , parentTableRPL (nullptr)
            , routingTable (nullptr)
            , sourceRoutingTable(nullptr)
            , linklocalAddress (IPv6Address::UNSPECIFIED_ADDRESS)
            , globalAddress (IPv6Address::UNSPECIFIED_ADDRESS)
            , nodeIndex (-1)
            , rank (-1)
            , isJoinUpward (false)
            , isJoinDownward (false)
            , joiningTimeUpward (SIMTIME_ZERO)
            , joiningTimeDownward (SIMTIME_ZERO)
            , numSentDIO(0)
            , numReceivedDIO(0)
            , numSuppressedDIO(0)
            , numSentDIS(0)
            , numReceivedDIS(0)
            , numSuppressedDIS(0)
            , numSentDAO(0)
            , numReceivedDAO(0)
            //, numSuppressedDAO(0)
            , numberOfNeighbors(0)
            , numberOfParents(0)
            , numberOfRoutes(0)
            , numberOfSRRoutes(0)
            {};

    };

    typedef std::vector<struct NodeState> NodeStateList;
    NodeStateList nodeStateList;

    //int version;
    unsigned int sinkID;
    //IPv6Address sinkLLAddress;

    simtime_t convergenceTimeStart;  //DODAG Sart Time
    simtime_t convergenceTimeEndUpward; // DODAG formation time in MOP = 0.
    simtime_t convergenceTimeEndDownward; // DODAG formation time in MOP = 1, 2, or 3.
    int numSentDIO;
    int numReceivedDIO;
    int numSuppressedDIO;
    int numSentDIS;
    int numReceivedDIS;
    int numSuppressedDIS;
    int numSentDAO;
    int numReceivedDAO;
    //int numSuppressedDAO;
    int numberOfNeighbors;
    int numberOfParents;
    int numberOfRoutes;
    int numberOfSRRoutes;
    float averageNumberofHopCount;

    int numberOfIterations;
    int numberOfGlogalRepaires; //Each Global Repair increments this variable.
    cMessage* globalRepairTimer;
    simtime_t globalRepairInterval;

    int numberOfDODAGformationNormal;

    RPLMOP mop; //Mode Of Operation

    //Varibales to calculate Hop Count
    std::vector <std::vector <int>> hopCountMat;

    struct Node{
        int nodeIndex;
        int rank;
    };
    /*
     * A map between the node IDs and hopCount matrix indices
     * For the hopCount matrix, Indices are the ordered sequence of ranks
     */
    std::vector <struct Node> mapping;

    /* Compares two Node according to nodeIndex.
     * It's used to sort the mapping array.
     */
    struct compareNodes {
      bool operator() (struct Node n1, struct Node n2) { return (n1.rank < n2.rank);}
    } mapCompare;

public:
    StatisticCollector()
        : rplManager(nullptr)
        , sinkID(-1)
        , convergenceTimeStart(0)
        , convergenceTimeEndUpward(0)
        , convergenceTimeEndDownward(0)
        , numSentDIO(0)
        , numReceivedDIO(0)
        , numSuppressedDIO(0)
        , numSentDIS(0)
        , numReceivedDIS(0)
        , numSuppressedDIS(0)
        , numSentDAO(0)
        , numReceivedDAO(0)
        //, numSuppressedDAO(0)
        , numberOfNeighbors(0)
        , numberOfParents(0)
        , numberOfRoutes(0)
        , numberOfSRRoutes(0)
        , averageNumberofHopCount(0)
        , numberOfIterations(0)
        , numberOfGlogalRepaires(0)
        , globalRepairTimer(nullptr)
        , globalRepairInterval(0)
        , numberOfDODAGformationNormal(0)
        , mop(Storing_Mode_of_Operation_with_no_multicast_support)
            {};

    ~StatisticCollector();

protected:

    virtual void initialize(int stage);

    virtual bool isConverged();

    virtual void saveStatistics();

    virtual void scheduleNextGlobalRepair();

    virtual void scheduleNewGlobalRepair();

    virtual void handleMessage(cMessage* msg);

    virtual void calculateHopCount();

    virtual int nodeIndexToOrderedIndex(int nodeIndex);

    virtual int orderedIndexToNodeIndex(unsigned int orderedIndex);

    virtual int minHopCount(int nodei, int nodej);

public:
    virtual void registNode(cModule *host, RPLUpwardRouting *pRPLUpwardRouting, ParentTableRPL *parentTableRPL, IPv6RoutingTable *routingTable, SourceRoutingTable *sourceRoutingTable, IPv6Address linlklocalAddress, IPv6Address globalAddress);

    virtual void startStatistics(RPLMOP mop, IPv6Address sinkLLAddress, simtime_t time);

    virtual void nodeJoinedUpward(int nodeID, simtime_t time);

    virtual void nodeJoinedUpward(IPv6Address linkLocalAddress, simtime_t time);

    virtual void nodeJoinedDownnward(IPv6Address globalAddress, simtime_t time);

    virtual void nodeJoinedDownnward(int nodeID, simtime_t time);

    virtual void updateRank(IPv6Address ip, int rank);

    //void messageAction(messagesTypes type, MessageAction action);
    virtual void collectOtherMetrics();



};

} // namespace rpl

#endif // ifndef _RPL_SRC_STATISTIC_STATISTICCOLLECTOR_H_

