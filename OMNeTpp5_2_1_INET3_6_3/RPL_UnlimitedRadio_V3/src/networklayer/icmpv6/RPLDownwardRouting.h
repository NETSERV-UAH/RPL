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


#ifndef _RPL_SRC_ROUTING_RPLROUTING_H
#define _RPL_SRC_ROUTING_RPLROUTING_H

#include <map>

#include "src/networklayer/icmpv6/ICMPv6MessageRPL_m.h"
#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/contract/ipv6/IPv6AddressType.h"
#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/contract/INetfilter.h"
#include "inet/common/lifecycle/NodeStatus.h"

#include "src/simulationManager/managerRPL.h"


namespace rpl {
using namespace inet;

#define ZERO_LIFETIME 0 // This feature is used for No-Path DAO, is not implemented yet.

struct DIOState{
    int version;
    int Sent;
    int Received;
    int Suppressed;
};

struct DISState{
    int Sent;
    int Received;
    int Suppressed;
};

struct DAOState{
    int Sent;
    int Received;
    int Suppressed;
};

struct NodeState{
    int Version;
    int Collision;
    int PacketLost;
    struct DIOState DIO;
    struct DISState DIS;
    struct DAOState DAO;

    double *PowerConsumption;
    int *Rank;
    simtime_t *JoiningDODAGTime_Upward;
    simtime_t *JoiningDODAGTime_Downward;

    simtime_t DODAGsFormationTimeRecords_Upward;
    simtime_t DODAGsFormationTimeRecords_Downward;

    //Variables for saving the number of table entries in each iteration
    int numPreferedParents_Upward;
    int numParents_Upward;
    int numParents_Downward;


    NodeState* Link;
}*NodeStateHeader=NULL,*NodeStateLast=NULL,*NodeStateNew=NULL;



/**
 * @brief IPv6 Routing Protocol for LLNs (RPL)provides a mechanism whereby
 * multipoint-to-point traffic from devices inside the LLN towards a central
 * control point is supported.
 *
**/

class RPLRouting : public cSimpleModule, public ILifecycle//, public INetfilter::IHook//, public cListener

{
public:

    enum RPLMOP{
        MOP_NO_UPWARDROUTE = 0,
        MOP_NON_STORING,
        MOP_STORING_NO_MULTICAST,
        MOP_STORING_WITH_MULTICAST,
    };

    enum messagesTypes {
        UNKNOWN=0,
        DATA,
        Global_REPAIR_TIMER,
        DIO,
        SEND_DIO_TIMER,
        DIS_UNICAST,
        SEND_DIS_UNICAST_TIMER,
        DIS_FLOOD,
        SEND_DIS_FLOOD_TIMER,
        RESET_Global_REPAIR_TIMER,
        DAO,
        SEND_DAO_TIMER,
        DAO_LIFETIME_TIMER,
    };

    int NofEntry;

    struct RoutingEntry
     {
        int prefixLen;
        IPv6Address nextHop;
        simtime_t lifeTime;
        bool NoPathReceived = false;
        RoutingEntry() {}
        RoutingEntry(IPv6Address nextHop, simtime_t insertionTime) :
            prefixLen(prefixLen), nextHop(nextHop), lifeTime(lifeTime), NoPathReceived(NoPathReceived) {}
     };

     //friend std::ostream& operator<<(std::ostream& os, const RoutingEntry& entry);

     struct IPv6_compare
     {
         bool operator()(const IPv6Address& u1, const IPv6Address& u2) const { return u1.compare(u2) < 0; }
     };

     typedef std::map<IPv6Address, RoutingEntry, IPv6_compare> RoutingTable;  // Each prefix is mapped to one RoutingEntry

     typedef std::vector<RoutingTable *> RoutingTables; //To maintain one routing table for each version number(global repair).
     RoutingTables routingTables;


    // environment
    cModule *host = nullptr;
    //IRoutingTable *routingTable = nullptr;
    IInterfaceTable *interfaceTable = nullptr;
    managerRPL *pManagerRPL = nullptr;
    INetfilter *networkProtocol = nullptr;
    int interfaceID;
    bool *hasRoute;

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
    int DISheaderLength;
    int DAOheaderLength;
    int headerLength;

    /**
     * @brief RPL setting parameters
     * Read from omnetpp.ini
     **/
    simtime_t defaultLifeTime;  // only used for DAO
    simtime_t ROUTE_INFINITE_LIFETIME;

    bool DAOEnable;
    simtime_t DelayDAO;
    bool DISEnable;
    bool refreshDAORoutes;

    double DIOIntMin;
    int DIORedun;
    int DIOIntDoubl;
    simtime_t DIOIMaxLength;

    double DISIntMin;
    double DISStartDelay;
    int DISRedun;
    int DISIntDoubl;
    simtime_t DISIMaxLength;
    int DISVersion;

    MACAddress macaddress;

    IPv6Address sinkAddress;
    IPv6Address myNetwAddr;

    bool isSink;

    bool useSimTracer;
    bool trace, stats, debug;
    double rssiThreshold;


    cMessage* GRepairTimer;
    cMessage* DIOTimer;
    cMessage* DISTimer;
    cMessage* DAOTimer;
    cMessage* DAOLifeTimer;


    struct DIOStatus
    {
        unsigned int nbDIOSent;
        unsigned int nbDIOReceived;
        unsigned int nbDIOSuppressed;
        int version;
        struct DIOStatus* link;
    }*DIOStatusHeader,*DIOStatusNew,*DIOStatusLast;

    struct DISStatus
    {
        unsigned int nbDISSent;
        unsigned int nbDISReceived;
        unsigned int nbDISSuppressed;
        struct DISStatus* link;
    }*DISStatusHeader,*DISStatusNew,*DISStatusLast;

    struct DAOStatus
    {
        unsigned int nbDAOSent;
        unsigned int nbDAOReceived;
        unsigned int nbDAOSuppressed;
        struct DAOStatus* link;
    }*DAOStatusHeader,*DAOStatusNew,*DAOStatusLast;


    char *FilePath;
    bool IsJoined;

    double GlobalRepairTimer;

    unsigned char dtsnInstance;

    IPv6Address DODAGID;
    int Rank;
    simtime_t NodeStartTime;
    int VersionNember;
    int Grounded;
    simtime_t TimetoSendDIO;
    simtime_t TimetoSendDIS;

    struct DODAGJoiningtime{
        simtime_t TimetoJoinDODAG;
        int version;
        struct DODAGJoiningtime* link;
    }*DODAGJoinTimeHeader_Upward,*DODAGJoinTimeLast_Upward,*DODAGJoinTimeNew_Upward,*DODAGJoinTimeHeader_Downward,*DODAGJoinTimeLast_Downward,*DODAGJoinTimeNew_Downward;

    int DIO_c;
    simtime_t DIO_CurIntsizeNow,DIO_CurIntsizeNext;
    simtime_t DIO_StofCurIntNow,DIO_StofCurIntNext;
    simtime_t DIO_EndofCurIntNow,DIO_EndofCurIntNext;

    int DIS_c;
    simtime_t DIS_CurIntsizeNow,DIS_CurIntsizeNext;
    simtime_t DIS_StofCurIntNow,DIS_StofCurIntNext;
    simtime_t DIS_EndofCurIntNow,DIS_EndofCurIntNext;


    IPv6Address PrParent;
    int *NofParents;
    int MaxNofParents;
    struct ParentStructure{
        IPv6Address ParentId;
        int ParentRank;
        unsigned char dtsn;
    }**Parents;
    typedef ParentStructure Parent;



    /** @brief Copy constructor is not allowed.
     */
   // RPLRouting(const RPLRouting&);
    /** @brief Assignment operator is not allowed.
     */
   // RPLRouting& operator=(const RPLRouting&);


    RPLRouting()
        : DIOheaderLength(0)
        , DISheaderLength(0)
        , DAOheaderLength(0)
        , defaultLifeTime(0)
        , ROUTE_INFINITE_LIFETIME(0)
        , macaddress()
        , sinkAddress()
        , debug(false)
        , DISEnable(false)
        , DAOEnable(false)
        , refreshDAORoutes(false)
        , GRepairTimer(nullptr)
        , DIOTimer(nullptr)
        , DISTimer(nullptr)
        , DAOTimer(nullptr)
        , DAOLifeTimer(nullptr)
    {};

    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    void handleMessage(cMessage* msg) override;

    virtual void finish() override;
    friend NodeState* CreateNewNodeState(int Index, int VersionNo, simtime_t Time, int NodeRank);
    virtual ~RPLRouting();

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

    /** @brief scheduling next DIO message transmission. */
    virtual void scheduleNextDIOTransmission();
    void scheduleNextDISTransmission();
    virtual void scheduleNextDAOTransmission(simtime_t delay, simtime_t LifeTime);
    virtual void scheduleDAOlifetimer(simtime_t LifeTime);
    void ScheduleNextGlobalRepair();
    void DeleteScheduledNextGlobalRepair();

    void TrickleReset();
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

    DIOStatus* CreateNewVersionDIO(void);
    DISStatus* CreateNewVersionDIS(void);
    void DISHandler();
    DODAGJoiningtime* CreateNewVersionJoiningTime(void);

    virtual void sendDAOMessage(IPv6Address prefix, simtime_t lifetime);

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

};

} // namespace rpl

#endif  // ifndef _RPL_SRC_ROUTING_RPLROUTING_H

