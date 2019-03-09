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


namespace rpl {
using namespace inet;


class StatisticCollector : public cSimpleModule
{

    //ICMPv6RPL *icmpv6RPL;

    struct NodeState{
        cModule *hostModule = nullptr;
        IPv6Address linklocalAddress = IPv6Address::UNSPECIFIED_ADDRESS;
        IPv6Address globalAddress = IPv6Address::UNSPECIFIED_ADDRESS;
        int nodeIndex = -1; //According to RPL manager module

        int rank;

        bool isJoinUpward = false;
        bool isJoinDownward = false;
        simtime_t joiningTimeUpward = SIMTIME_ZERO; // By DIO
        simtime_t joiningTimeDownward = SIMTIME_ZERO;  //By DAO

    };

    typedef std::vector<struct NodeState> NodeStateList;
    NodeStateList nodeStateList;

    //int version;
    int sinkID;
    //IPv6Address sinkLLAddress;

    simtime_t convergenceTimeStart;  //DODAG Sart Time
    simtime_t convergenceTimeEndUpward; // DODAG formation time in MOP = 0.
    simtime_t convergenceTimeEndDownward; // DODAG formation time in MOP = 1, 2, or 3.

    RPLMOP mop; //Mode Of Operation

    cMessage* globalRepairTimer;
    simtime_t globalRepairInterval;

public:
    StatisticCollector()
        : convergenceTimeStart(0)
        , convergenceTimeEndUpward(0)
        , convergenceTimeEndDownward(0)
            {};

    ~StatisticCollector();

protected:

    virtual bool isConvergeedUpward();

    virtual bool isConvergedDownward();

    virtual void saveStatistics();

    virtual void handleGlobalRepairTimer(cMessage* msg);

    virtual void scheduleNewGlobalRepair();

    virtual void handleMessage(cMessage* msg);


public:
    virtual void registNode(cModule *hostModule, IPv6Address linlklocalAddress, IPv6Address globalAddress);

    virtual void startStatistics(RPLMOP mop, IPv6Address sinkLLAddress, simtime_t time);

    virtual void nodeJoinedUpward(IPv6Address linkLocalAddress, simtime_t time);

    virtual void nodeJoinedDownnward(IPv6Address ip, simtime_t time);

    virtual void updateRank(IPv6Address ip, int rank);


};

} // namespace rpl

#endif // ifndef _RPL_SRC_STATISTIC_STATISTICCOLLECTOR_H_

