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
#include "src/simulationManager/managerRPL.h"
#include "src/networklayer/icmpv6/ParentTableRPL.h"
#include "inet/networklayer/contract/IRoutingTable.h"

namespace rpl {
using namespace inet;

class RPLUpwardRouting;

class StatisticCollector : public cSimpleModule
{

    enum MessageAction{
        sent,
        received,
        suppressed
    };


    //ICMPv6RPL *icmpv6RPL;
    managerRPL *rplManager;

    struct NodeState{
        cModule *host;
        RPLUpwardRouting *pRPLUpwardRouting;
        ParentTableRPL *parentTableRPL;
        IRoutingTable *routingTable;
        IPv6Address linklocalAddress;
        IPv6Address globalAddress;
        int nodeIndex; //According to RPL manager module

        int rank;

        bool isJoinUpward;
        bool isJoinDownward;
        simtime_t joiningTimeUpward; // By DIO
        simtime_t joiningTimeDownward;  //By DAO

        NodeState()
            : host(nullptr)
            , pRPLUpwardRouting (nullptr)
            , parentTableRPL (nullptr)
            , routingTable (nullptr)
            , linklocalAddress (IPv6Address::UNSPECIFIED_ADDRESS)
            , globalAddress (IPv6Address::UNSPECIFIED_ADDRESS)
            , nodeIndex (-1)
            , rank (-1)
            , isJoinUpward (false)
            , isJoinDownward (false)
            , joiningTimeUpward (SIMTIME_ZERO)
            , joiningTimeDownward (SIMTIME_ZERO)
            {};

    };

    typedef std::vector<struct NodeState> NodeStateList;
    NodeStateList nodeStateList;

    //int version;
    int sinkID;
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
    int numSuppressedDAO;

    int numberOfIterations;
    int numberOfGlogalRepaires; //Each Global Repair increments this variable.
    cMessage* globalRepairTimer;
    simtime_t globalRepairInterval;

    int numberOfDODAGformationNormal;

    RPLMOP mop; //Mode Of Operation

public:
    StatisticCollector()
        : convergenceTimeStart(0)
        , convergenceTimeEndUpward(0)
        , convergenceTimeEndDownward(0)
            {};

    ~StatisticCollector();

protected:

    void initialize(int stage);

    virtual bool isConverged();

    virtual void saveStatistics();

    void ScheduleNextGlobalRepair();

    virtual void scheduleNewGlobalRepair();

    virtual void handleMessage(cMessage* msg);


public:
    virtual void registNode(cModule *host, RPLUpwardRouting *pRPLUpwardRouting, ParentTableRPL *parentTableRPL, IRoutingTable *routingTable, IPv6Address linlklocalAddress, IPv6Address globalAddress);

    virtual void startStatistics(RPLMOP mop, IPv6Address sinkLLAddress, simtime_t time);

    virtual void nodeJoinedUpward(int nodeID, simtime_t time);

    virtual void nodeJoinedUpward(IPv6Address linkLocalAddress, simtime_t time);

    virtual void nodeJoinedDownnward(IPv6Address linkLocalAddress, simtime_t time);

    virtual void updateRank(IPv6Address ip, int rank);

    void messageAction(messagesTypes type, MessageAction action);



};

} // namespace rpl

#endif // ifndef _RPL_SRC_STATISTIC_STATISTICCOLLECTOR_H_

