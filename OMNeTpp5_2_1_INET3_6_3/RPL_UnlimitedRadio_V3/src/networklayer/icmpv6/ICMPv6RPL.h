//
// Copyright (C) 2005 Andras Varga
// Copyright (C) 2005 Wei Yang, Ng
/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    (3) UPC, Castelldefels, Spain.
 *                    adapted for using in RPL
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

#ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_ICMPV6RPL_H
#define _RPL_SRC_NETWORKLAYER_ICMPV6_ICMPV6RPL_H

#include "inet/common/INETDefs.h"
#include "src/networklayer/contract/RPLDefs.h" //EXTRA

#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/icmpv6/ICMPv6Message_m.h"
#include "src/networklayer/icmpv6/ICMPv6MessageRPL_m.h" //EXTRA
#include "inet/networklayer/contract/ipv6/IPv6Address.h"
#include "inet/networklayer/contract/ipv6/IPv6ControlInfo.h"
#include "inet/applications/pingapp/PingPayload_m.h"
#include "src/networklayer/icmpv6/RPLUpwardRouting.h" //EXTRA
#include "src/networklayer/icmpv6/ParentTableRPL.h" //EXTRA
#include "inet/networklayer/ipv6/IPv6RoutingTable.h" //EXTRA
#include "inet/networklayer/contract/IInterfaceTable.h" //EXTRA
#include "src/networklayer/icmpv6/IPv6NeighbourDiscoveryRPL.h"//EXTRA
#include "src/statisticcollector/StatisticCollector.h"//EXTRA


namespace rpl {
using namespace inet;

class inet::IPv6Address;
class inet::IPv6ControlInfo;
class inet::IPv6Datagram;
class inet::PingPayload;

class inet::ICMPv6Message;


/**
 * ICMPv6 implementation.
 */
class ICMPv6RPL : public cSimpleModule, public ILifecycle
{
//EXTRA BEGIN
protected:
    enum RPLMOP mop;//
    int interfaceID;
    RPLUpwardRouting *rplUpwardRouting;
    IPv6NeighbourDiscoveryRPL *neighbourDiscoveryRPL;
    StatisticCollector *statisticCollector;
    ParentTableRPL *parentTableRPL;
    //IInterfaceTable *interfaceTable;
    IPv6RoutingTable *routingTable;


    int DIS_c;
    simtime_t DIS_CurIntsizeNow,DIS_CurIntsizeNext;
    simtime_t DIS_StofCurIntNow,DIS_StofCurIntNext;
    simtime_t DIS_EndofCurIntNow,DIS_EndofCurIntNext;
    int DISheaderLength;
    int numReceivedDIS;
    int numSentDIS;
    int numSuppressedDIS;



    cModule *host;

    double DISIntMin;
    double DISStartDelay;
    int DISRedun;
    int DISIntDoubl;
    simtime_t DISIMaxLength;//
    int DISVersion;
    cMessage* DISTimer;//
    simtime_t TimetoSendDIS;

    int DAOheaderLength;//
    cMessage* DAOTimer;//
    cMessage* DAOLifeTimer;//
    simtime_t DelayDAO;
    simtime_t defaultLifeTime;  // only used for DAO//
    simtime_t ROUTE_INFINITE_LIFETIME;//





public:
    ICMPv6RPL()
        : mop(Storing_Mode_of_Operation_with_no_multicast_support)
        , DAOheaderLength(0)
        , defaultLifeTime(0)
        , ROUTE_INFINITE_LIFETIME(0)
        , DAOTimer(nullptr)
        , DAOLifeTimer(nullptr)
        , DISTimer(nullptr)
        , interfaceID(-1)
        , rplUpwardRouting(nullptr)
        , neighbourDiscoveryRPL(nullptr)
        , statisticCollector(nullptr)
        , parentTableRPL(nullptr)
        , routingTable(nullptr)
        , DIS_c(0)
        , DIS_CurIntsizeNow(0)
        , DIS_CurIntsizeNext(0)
        , DIS_StofCurIntNow(0)
        , DIS_StofCurIntNext(0)
        , DIS_EndofCurIntNow(0)
        , DIS_EndofCurIntNext(0)
        , DISheaderLength(0)
        , numReceivedDIS(0)
        , numSentDIS(0)
        , numSuppressedDIS(0)
        , host(nullptr)
        , DISIntMin(0)
        , DISStartDelay(0)
        , DISRedun(0)
        , DISIntDoubl(00)
        , DISVersion()
        , TimetoSendDIS(0)
        , DelayDAO(0)
            {};

    ~ICMPv6RPL();


//EXTRA END

public:
    /**
     *  This method can be called from other modules to send an ICMPv6 error packet.
     *  RFC 2463, Section 3: ICMPv6 Error Messages
     *  There are a total of 4 ICMPv6 error messages as described in the RFC.
     *  This method will construct and send error messages corresponding to the
     *  given type.
     *  Error Types:
     *      - Destination Unreachable Message - 1
     *      - Packet Too Big Message          - 2
     *      - Time Exceeded Message           - 3
     *      - Parameter Problem Message       - 4
     *  Code Types have different semantics for each error type. See RFC 2463.
     */
    virtual void sendErrorMessage(IPv6Datagram *datagram, ICMPv6Type type, int code);

    /**
     * This method can be called from other modules to send an ICMP error packet
     * in response to a received bogus packet from the transport layer (like UDP).
     * The ICMP error packet needs to include (part of) the original IP datagram,
     * so this function will wrap back the transport packet into the IP datagram
     * based on its IPv4ControlInfo.
     */
    virtual void sendErrorMessage(cPacket *transportPacket, IPv6ControlInfo *ctrl, ICMPv6Type type, int code);

  protected:
    // internal helper functions
    virtual void sendToIP(ICMPv6Message *msg, const IPv6Address& dest);
    virtual void sendToIP(ICMPv6Message *msg);    // FIXME check if really needed
    //EXTRA
    virtual void sendToRPL(ICMPv6Message *msg);

    virtual ICMPv6Message *createDestUnreachableMsg(int code);
    virtual ICMPv6Message *createPacketTooBigMsg(int mtu);
    virtual ICMPv6Message *createTimeExceededMsg(int code);
    virtual ICMPv6Message *createParamProblemMsg(int code);    //TODO:Section 3.4 describes a pointer. What is it?

  protected:
    /**
     * Initialization
     */
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    /**
     *  Processing of messages that arrive in this module. Messages arrived here
     *  could be for ICMP ping requests or ICMPv6 messages that require processing.
     */
    virtual void handleMessage(cMessage *msg) override;
    virtual void processICMPv6Message(ICMPv6Message *);

    //EXTRA BEGIN
    virtual void handleSelfMsg(cMessage *msg);

    virtual void processIncommingRPLMessage(ICMPv6Message *msg);

    virtual void processIncommingNonStoringDAOMessage(ICMPv6Message *msg);
    virtual void processIncommingStoringDAOMessage(ICMPv6Message *icmpv6msg);
  public:
    virtual void sendDAOMessage(IPv6Address prefix, simtime_t lifetime, IPv6Address parent = IPv6Address::UNSPECIFIED_ADDRESS);

    virtual void scheduleNextDAOTransmission(simtime_t delay, simtime_t LifeTime);

    virtual void DeleteDAOTimers();

  protected:
    virtual void scheduleDAOlifetimer(simtime_t lifeTime);
    virtual void handleDAOTimer(cMessage* msg);



    virtual void processIncommingDISMessage(ICMPv6Message *msg);
  public:
    virtual void SetDISParameters(simtime_t dodagSartTime);
    virtual void scheduleNextDISTransmission();
    virtual void cancelAndDeleteDISTimer();

  protected:
    virtual void handleDISTimer(cMessage* msg);

    //EXTRA END



    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

    /**
     *  Respond to the machine that tried to ping us.
     */
    virtual void processEchoRequest(ICMPv6EchoRequestMsg *);

    /**
     *  Forward the ping reply to the "pingOut" of this module.
     */
    virtual void processEchoReply(ICMPv6EchoReplyMsg *);

    /**
     *  Ping a machine. The information needed to do this is in the cMessage
     *  parameter.  TODO where in cMessage? document!!!
     */
    virtual void sendEchoRequest(PingPayload *);

    /**
     * Validate the received IPv6 datagram before responding with error message.
     */
    virtual bool validateDatagramPromptingError(IPv6Datagram *datagram);

    virtual void errorOut(ICMPv6Message *);

  protected:
    typedef std::map<long, int> PingMap;
    PingMap pingMap;
};

} // namespace rpl

#endif // ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_ICMPV6RPL_H

