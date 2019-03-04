/***************************************************************************
 * file:        RPLRouting.h
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
 * To read more information about the Kermajani's article, you can use [1].
 *
 *                    [1] Kermajani, Hamidreza, and Carles Gomez. "On the network convergence process
 *                    in RPL over IEEE 802.15. 4 multihop networks: Improvement and trade-offs."
 *                    Sensors 14.7 (2014): 11993-12022.þ
 */


#ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_RPLUPWARDROUTING_H
#define _RPL_SRC_NETWORKLAYER_ICMPV6_RPLUPWARDROUTING_H

#include <map>

#include "src/networklayer/icmpv6/ICMPv6MessageRPL_m.h"
#include "inet/common/INETDefs.h"
#include "src/networklayer/contract/RPLDefs.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/contract/ipv6/IPv6AddressType.h"
#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/contract/INetfilter.h"
#include "inet/common/lifecycle/NodeStatus.h"

#include "src/simulationManager/managerRPL.h"
#include "src/statisticcollector/StatisticCollector.h"



namespace rpl {
using namespace inet;

#define ZERO_LIFETIME 0 // This feature is used for No-Path DAO.

/**
 * @brief IPv6 Routing Protocol for LLNs (RPL)provides a mechanism whereby
 * multipoint-to-point traffic from devices inside the LLN towards a central
 * control point is supported.
 *
**/

class RPLUpwardRouting : public cSimpleModule, public ILifecycle//, public INetfilter::IHook//, public cListener

{
protected:

    // environment
    IRoutingTable *routingTable = nullptr;
    IInterfaceTable *interfaceTable = nullptr;
    managerRPL *pManagerRPL = nullptr;
    StatisticCollector *statistisCollector = nullptr;
    INetfilter *networkProtocol = nullptr;
    int interfaceID;

    /** @brief Gate ids */
    //@{
    int icmpv6InGateId;
    int icmpv6OutGateId;
    //@}

    // lifecycle
    bool isOperational = false;

    /**
     * @brief Length of the NetwPkt header
     * Read from omnetpp.ini
     **/
    int DIOheaderLength;
    int DAOheaderLength;
    int headerLength;

    /**
     * @brief RPL setting parameters
     * Read from omnetpp.ini
     **/
    simtime_t defaultLifeTime;  // only used for DAO
    simtime_t ROUTE_INFINITE_LIFETIME;

    RPLMOP mop; //Mode Of Operation
    simtime_t DelayDAO;
    bool DISEnable;
    bool refreshDAORoutes;

    double DIOIntMin;
    int DIORedun;
    int DIOIntDoubl;
    simtime_t DIOIMaxLength;

    cModule *host;

    MACAddress macaddress;

    IPv6Address sinkAddress;
    IPv6Address myLLNetwAddr; //Link Local address
    IPv6Address myGlobalNetwAddr; //Global Address
    IPv6Address DODAGID;
    IPv6Address PrParent;


    bool isSink;

    cMessage* GRepairTimer;
    cMessage* DIOTimer;
    cMessage* DAOTimer;
    cMessage* DAOLifeTimer;


    bool IsJoined;
    bool isNodeJoined;

    double GlobalRepairTimer;

    unsigned char dtsnInstance;

    int Rank;
    simtime_t NodeStartTime;
    int VersionNember;
    int Grounded;
    simtime_t TimetoSendDIO;

    int DIO_c;
    simtime_t DIO_CurIntsizeNow,DIO_CurIntsizeNext;
    simtime_t DIO_StofCurIntNow,DIO_StofCurIntNext;
    simtime_t DIO_EndofCurIntNow,DIO_EndofCurIntNext;

public:
    /** @brief Copy constructor is not allowed.
     */
   // RPLRouting(const RPLRouting&);
    /** @brief Assignment operator is not allowed.
     */
   // RPLRouting& operator=(const RPLRouting&);


    RPLUpwardRouting()
        : DIOheaderLength(0)
        , DISheaderLength(0)
        , DAOheaderLength(0)
        , defaultLifeTime(0)
        , ROUTE_INFINITE_LIFETIME(0)
        , macaddress()
        , host(nullptr)
        , myLLNetwAddr(IPv6Address::UNSPECIFIED_ADDRESS)
        , myGlobalNetwAddr(IPv6Address::UNSPECIFIED_ADDRESS)
        , DODAGID(IPv6Address::UNSPECIFIED_ADDRESS)
        , sinkAddress()
        , DISEnable(false)
        , refreshDAORoutes(false)
        , GRepairTimer(nullptr)
        , DIOTimer(nullptr)
        , DAOTimer(nullptr)
        , DAOLifeTimer(nullptr)
        , mop(Storing_Mode_of_Operation_with_no_multicast_support)
    {};

    virtual ~RPLUpwardRouting();

protected:
    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void handleMessage(cMessage* msg) override;

    virtual void finish() override;

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage* msg);


    /** @brief Handle messages from the ICMPv6 module */
    virtual void handleIncommingMessage(cMessage* msg);

    virtual void handleIncommingDIOMessage(cMessage* msg);

    virtual void handleIncommingDISMessage(cMessage* msg);

    virtual void handleIncommingDAOMessage(cMessage* msg);


    /** @brief Handle self messages */
    virtual void handleSelfMsg(cMessage* msg);

    virtual void handleGlobalRepairTimer(cMessage* msg);


    /** @brief Handle control messages from lower layer */
    virtual void handleLowerControl(cMessage* msg);

    virtual void handleDISTimer(cMessage* msg);

    virtual void handleDAOTimer(cMessage* msg);



    /** @brief Decapsulate a message */
    cMessage* decapsMsg(ICMPv6DIOMsg *msg);

public:
    /** @brief scheduling next DIO message transmission. */
    virtual void scheduleNextDIOTransmission();

protected:
    void scheduleNextDISTransmission();
    virtual void scheduleNextDAOTransmission(simtime_t delay, simtime_t LifeTime);
    virtual void scheduleDAOlifetimer(simtime_t LifeTime);
    void ScheduleNextGlobalRepair();
    void DeleteScheduledNextGlobalRepair();

public:
    void TrickleReset();

protected:
    void DeleteDIOTimer();
    void DeleteDAOTimers();
    void SetDIOParameters();
    void SetDISParameters();

    virtual int  IsParent(const IPv6Address& id,int idrank);
    virtual void AddParent(const IPv6Address& id,int idrank, unsigned char dtsn);
    virtual void DeleteParent(const IPv6Address& id);
    virtual int getParentIndex(const IPv6Address& id);


    virtual bool IsNeedDAO(const IPv6Address parent, unsigned char dtsn);
    virtual void handleDIOTimer(cMessage* msg);

    virtual void sendDAOMessage(IPv6Address prefix, simtime_t lifetime);

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

public:
    virtual bool isNodeJoinedToDAG();
    virtual int getVersion();
    virtual IPv6Address getDODAGID();


};

} // namespace rpl

#endif  // ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_RPLUPWARDROUTING_H

