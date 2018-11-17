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

//EXTRA BEGIN
/*
#ifndef RPL_h
#define RPL_h

#include <map>
#include <omnetpp.h>


#include "MiXiMDefs.h"
#include "BaseNetwLayer.h"
#include "SimpleAddress.h"
#include "SimpleBattery.h"
#include "DIOMsg_m.h"
#include "DISMessage_m.h"


class SimTracer;
class DIOMsg;
class DISMessage;
*/

#ifndef _RPL_SRC_ROUTING_RPLROUTING_H
#define _RPL_SRC_ROUTING_RPLROUTING_H

#include <map> //EXTRA

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
//EXTRA END

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

struct NodeState{
    int Version;
    int Collision;
    int PacketLost;
    struct DIOState DIO;
    struct DISState DIS;

    double *PowerConsumption;
    int *Rank;
    simtime_t *JoiningDODAGTime;
    simtime_t DODAGsFormationTimeRecords;
    //EXTRA BEGIN, variables for saving the number of table entries in each iteration
    int numPreferedParents;
    int numParents;
    //EXTRA END
    NodeState* Link;
}*NodeStateHeader=NULL,*NodeStateLast=NULL,*NodeStateNew=NULL;



/**
 * @brief IPv6 Routing Protocol for LLNs (RPL)provides a mechanism whereby
 * multipoint-to-point traffic from devices inside the LLN towards a central
 * control point is supported.
 *
 *
 * @ingroup netwLayer
 * @author Hamidreza Kermajani
 **/
//EXTRA BEGIN
//class RPL : public BaseNetwLayer
class RPLRouting : public cSimpleModule, public ILifecycle//, public INetfilter::IHook//, public cListener

//EXTRA END
{
public:
    /** @brief Copy constructor is not allowed.
     */
   // RPLRouting(const RPLRouting&);
    /** @brief Assignment operator is not allowed.
     */
   // RPLRouting& operator=(const RPLRouting&);


    RPLRouting()
        //: BaseNetwLayer() //EXTRA
        : DIOheaderLength(0)
        , DISheaderLength(0)
        , DAOheaderLength(0) //EXTRA
        , defaultLifeTime(0) //EXTRA
        , lifeTimeUnit(0) //EXTRA
        , macaddress()
        , sinkAddress()
        , debug(false)
        , DISEnable(false) //EXTRA
        , DAOEnable(false) //EXTRA
    {};

    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int) override;  //virtual void initialize(int);  //EXTRA
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }  //EXTRA

    void handleMessage(cMessage* msg) override;  //EXTRA

    virtual void finish() override;  //virtual void finish();  //EXTRA
    friend NodeState* CreateNewNodeState(int Index, int VersionNo, simtime_t Time, int NodeRank);
    virtual ~RPLRouting();

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
        DAO,  //EXTRA
        SEND_DAO_TIMER, //EXTRA
        DAO_LIFETIME_TIMER, //EXTRA
    };

    //EXTRA BEGIN
    /*struct Route_Table
    {
        IPv6Address Address; //LAddress::L3Type Address;  //EXTRA
        IPv6Address NextHop; //LAddress::L3Type NextHop;  //EXTRA
        struct Route_Table* Link;
    }*RoutingTable;*/
    int NofEntry;

    struct RoutingEntry
     {
        int prefixLen;
        IPv6Address nextHop;
        simtime_t lifeTime;
        RoutingEntry() {}
        RoutingEntry(unsigned int vid, int portno, simtime_t insertionTime) :
            prefixLen(prefixLen), nextHop(nextHop), lifeTime(lifeTime) {}
     };

     friend std::ostream& operator<<(std::ostream& os, const RoutingEntry& entry);

     struct IPv6_compare
     {
         bool operator()(const IPv6Address& u1, const IPv6Address& u2) const { return u1.compareTo(u2) < 0; }
     };

     typedef std::map<IPv6Address, RoutingEntry, IPv6_compare> RoutingTable;

     RoutingTable *routingTable = nullptr;

    //EXTRA END


    //EXTRA BEGIN
    // environment
    cModule *host = nullptr;
    IRoutingTable *routingTable = nullptr;
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

    //EXTRA END

    /**
     * @brief Length of the NetwPkt header
     * Read from omnetpp.ini
     **/
    int DIOheaderLength;
    int DISheaderLength;
    int DAOheaderLength;  //EXTRA
    int headerLength;

    /**
     * @brief RPL setting parameters
     * Read from omnetpp.ini
     **/
    simtime_t defaultLifeTime; //EXTRA
    simtime_t lifeTimeUnit; //EXTRA
    bool DAOEnable;  //EXTRA
    simtime_t DelayDAO;  //EXTRA
    bool DISEnable;

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

    MACAddress macaddress; // LAddress::L2Type macaddress;  //EXTRA

    IPv6Address sinkAddress; //LAddress::L3Type sinkAddress;  //EXTRA
    IPv6Address myNetwAddr;  //EXTRA

    bool isSink; //EXTRA

    bool useSimTracer;
    bool trace, stats, debug;
    double rssiThreshold;


    cMessage* GRepairTimer;
    cMessage* DIOTimer;
    cMessage* DISTimer;
    cMessage* DAOTimer;  //EXTRA
    cMessage* DAOLifeTimer; //EXTRA


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


    char *FilePath;
    bool IsJoined;

    double GlobalRepairTimer;

    unsigned char dtsnInstance; //EXTRA

    IPv6Address DODAGID; //LAddress::L3Type DODAGID;
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
    }*DODAGJoinTimeHeader,*DODAGJoinTimeLast,*DODAGJoinTimeNew;

    int DIO_c;
    simtime_t DIO_CurIntsizeNow,DIO_CurIntsizeNext;
    simtime_t DIO_StofCurIntNow,DIO_StofCurIntNext;
    simtime_t DIO_EndofCurIntNow,DIO_EndofCurIntNext;

    int DIS_c;
    simtime_t DIS_CurIntsizeNow,DIS_CurIntsizeNext;
    simtime_t DIS_StofCurIntNow,DIS_StofCurIntNext;
    simtime_t DIS_EndofCurIntNow,DIS_EndofCurIntNext;


    IPv6Address PrParent; //LAddress::L3Type PrParent;  //EXTRA
    int *NofParents;
    int MaxNofParents;
    struct ParentStructure{
        IPv6Address ParentId; //LAddress::L3Type ParentId;  //EXTRA
        int ParentRank;
        unsigned char dtsn;  //EXTRA
    }**Parents;
    enum PARENT_TYPES
    {
      NOT_EXIST,
      EXIST,
      SHOULD_BE_UPDATED,
    };

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage* msg);

    //EXTRA BEGIN
    /** @brief Handle messages from lower layer */
    //virtual void handleLowerMsg(cMessage* msg);

    /** @brief Handle messages from the ICMPv6 module */
    virtual void handleIncommingMessage(cMessage* msg);
    //EXTRA END

    /** @brief Handle self messages */
    virtual void handleSelfMsg(cMessage* msg);

    /** @brief Handle control messages from lower layer */
    virtual void handleLowerControl(cMessage* msg);


    /** @brief Decapsulate a message */
    cMessage* decapsMsg(ICMPv6DIOMsg *msg);

    /** @brief scheduling next DIO message transmission. */
    virtual void scheduleNextDIOTransmission();
    void scheduleNextDISTransmission();
    void ScheduleNextGlobalRepair();
    void DeleteScheduledNextGlobalRepair();

    void TrickleReset();
    void DeleteDIOTimer();
    void SetDIOParameters();
    void SetDISParameters();

    //EXTRA BEGIN
    //virtual int  IsParent(const LAddress::L3Type& id,int idrank);
    //virtual void AddParent(const LAddress::L3Type& id,int idrank);
    //virtual void DeleteParent(const LAddress::L3Type& id);

    virtual int  IsParent(const IPv6Address& id,int idrank);
    virtual void AddParent(const IPv6Address& id,int idrank, unsigned int dtsn);
    virtual void DeleteParent(const IPv6Address& id);
    virtual bool IsNeedDAO(const IPv6Address parent, unsigned char dtsn);


    //EXTRA END

    DIOStatus* CreateNewVersionDIO(void);
    DISStatus* CreateNewVersionDIS(void);
    void DISHandler();
    DODAGJoiningtime* CreateNewVersionJoiningTime(void);

    //EXTRA
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

};

} // namespace rpl

#endif  // ifndef _RPL_SRC_ROUTING_RPLROUTING_H

