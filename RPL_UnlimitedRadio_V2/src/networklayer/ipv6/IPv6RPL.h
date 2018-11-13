//
// Copyright (C) 2005 Andras Varga
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

#ifndef _RPL_SRC_NETWORKLAYER_IPV6_IPV6RPL_H
#define _RPL_SRC_NETWORKLAYER_IPV6_IPV6RPL_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/INetfilter.h"
#include "inet/networklayer/contract/INetworkProtocol.h"
#include "inet/common/queue/QueueBase.h"
#include "inet/networklayer/ipv6/IPv6RoutingTable.h"
//#include "inet/networklayer/icmpv6/ICMPv6.h"
//#include "inet/networklayer/icmpv6/IPv6NeighbourDiscovery.h"

#include "inet/networklayer/ipv6tunneling/IPv6Tunneling.h"

#include "inet/networklayer/ipv6/IPv6Datagram.h"
//#include "inet/networklayer/ipv6/IPv6FragBuf.h"
#include "inet/common/ProtocolMap.h"

//EXTRA
#include "src/networklayer/icmpv6/ICMPv6RPL.h"
//#include "src/networklayer/icmpv6/ICMPv6MessageRPL_m.h"
#include "src/networklayer/ipv6/IPv6FragBufRPL.h"
#include "src/networklayer/icmpv6/IPv6NeighbourDiscoveryRPL.h"



namespace rpl {
using namespace inet;


class inet::ICMPv6Message;  //EXTRA

/**
 * IPv6 implementation.
 */
class IPv6RPL : public QueueBase, public ILifecycle, public INetfilter, public INetworkProtocol
{
  public:
    /**
     * Represents an IPv4Datagram, queued by a Hook
     */
    class QueuedDatagramForHook
    {
      public:
        QueuedDatagramForHook(IPv6Datagram *datagram, const InterfaceEntry *inIE, const InterfaceEntry *outIE, const IPv6Address& nextHopAddr, IHook::Type hookType) :
            datagram(datagram), inIE(inIE), outIE(outIE), nextHopAddr(nextHopAddr), hookType(hookType) {}
        virtual ~QueuedDatagramForHook() {}

        IPv6Datagram *datagram = nullptr;
        const InterfaceEntry *inIE = nullptr;
        const InterfaceEntry *outIE = nullptr;
        IPv6Address nextHopAddr;
        const IHook::Type hookType = (IHook::Type)-1;
    };

  protected:
    IInterfaceTable *ift = nullptr;
    IPv6RoutingTable *rt = nullptr;
    IPv6NeighbourDiscoveryRPL *nd = nullptr;  //EXTRA
    ICMPv6RPL *icmp = nullptr;  //EXTRA

    IPv6Tunneling *tunneling = nullptr;

    // working vars
    unsigned int curFragmentId = -1;    // counter, used to assign unique fragmentIds to datagrams
    IPv6FragBufRPL fragbuf;    //EXTRA // fragmentation reassembly buffer
    simtime_t lastCheckTime;    // when fragbuf was last checked for state fragments
    ProtocolMapping mapping;    // where to send packets after decapsulation

    // statistics
    int numMulticast = 0;
    int numLocalDeliver = 0;
    int numDropped = 0;
    int numUnroutable = 0;
    int numForwarded = 0;

#ifdef WITH_xMIPv6
    // 28.9.07 - CB
    // datagrams that are supposed to be sent with a tentative IPv6 address
    // are rescheduled for later resubmission.
    class ScheduledDatagram : public cPacket
    {
      protected:
        IPv6Datagram *datagram = nullptr;
        const InterfaceEntry *ie = nullptr;
        MACAddress macAddr;
        bool fromHL = false;
      public:
        ScheduledDatagram(IPv6Datagram *datagram, const InterfaceEntry *ie, MACAddress macAddr, bool fromHL);
        ~ScheduledDatagram();
        const InterfaceEntry *getIE() { return ie; }
        const IPv6Address& getSrcAddress() {return datagram->getSrcAddress(); }
        const MACAddress& getMACAddress() { return macAddr; }
        bool getFromHL() { return fromHL; }
        IPv6Datagram *removeDatagram() { IPv6Datagram *ret = datagram; datagram = nullptr; return ret; }
    };
#endif /* WITH_xMIPv6 */

    // netfilter hook variables
    typedef std::multimap<int, IHook *> HookList;
    HookList hooks;
    typedef std::list<QueuedDatagramForHook> DatagramQueueForHooks;
    DatagramQueueForHooks queuedDatagramsForHooks;

  protected:
    // utility: look up interface from getArrivalGate()
    virtual InterfaceEntry *getSourceInterfaceFrom(cPacket *msg);

    // utility: show current statistics above the icon
    virtual void refreshDisplay() const override;

    /**
     * Encapsulate packet coming from higher layers into IPv6Datagram
     */
    virtual IPv6Datagram *encapsulate(cPacket *transportPacket, IPv6ControlInfo *ctrlInfo);

    virtual void preroutingFinish(IPv6Datagram *datagram, const InterfaceEntry *fromIE, const InterfaceEntry *destIE, IPv6Address nextHopAddr);

    /**
     * Handle messages (typically packets to be send in IPv6) from transport or ICMP.
     * Invokes encapsulate(), then routePacket().
     */
    virtual void handleMessageFromHL(cPacket *msg);
    virtual void datagramLocalOut(IPv6Datagram *datagram, const InterfaceEntry *destIE, IPv6Address requestedNextHopAddress);

    /**
     * Handle incoming ICMP messages.
     */
    virtual void handleReceivedICMP(ICMPv6Message *msg);

    /**
     * Performs routing. Based on the routing decision, it dispatches to
     * localDeliver() for local packets, to fragmentAndSend() for forwarded packets,
     * to routeMulticastPacket() for multicast packets, or drops the packet if
     * it's unroutable or forwarding is off.
     */
    virtual void routePacket(IPv6Datagram *datagram, const InterfaceEntry *destIE, IPv6Address requestedNextHopAddress, bool fromHL);
    virtual void resolveMACAddressAndSendPacket(IPv6Datagram *datagram, int interfaceID, IPv6Address nextHop, bool fromHL);

    /**
     * Forwards packets to all multicast destinations, using fragmentAndSend().
     */
    virtual void routeMulticastPacket(IPv6Datagram *datagram, const InterfaceEntry *destIE, const InterfaceEntry *fromIE, bool fromHL);

    /**
     * Performs fragmentation if needed, and sends the original datagram or the fragments
     * through the specified interface.
     */
    virtual void fragmentAndSend(IPv6Datagram *datagram, const InterfaceEntry *destIE, const MACAddress& nextHopAddr, bool fromHL);
    /**
     * Perform reassembly of fragmented datagrams, then send them up to the
     * higher layers using sendToHL().
     */
    virtual void localDeliver(IPv6Datagram *datagram);

    /**
     * Decapsulate and return encapsulated packet after attaching IPv6ControlInfo.
     */
    virtual cPacket *decapsulate(IPv6Datagram *datagram);

    /**
     * Last hoplimit check, then send datagram on the given interface.
     */
    virtual void sendDatagramToOutput(IPv6Datagram *datagram, const InterfaceEntry *destIE, const MACAddress& macAddr);

    // NetFilter functions:

  protected:
    /**
     * called before a packet arriving from the network is routed
     */
    IHook::Result datagramPreRoutingHook(INetworkDatagram *datagram, const InterfaceEntry *inIE, const InterfaceEntry *& outIE, L3Address& nextHopAddr);

    /**
     * called before a packet arriving from the network is delivered via the network
     */
    IHook::Result datagramForwardHook(INetworkDatagram *datagram, const InterfaceEntry *inIE, const InterfaceEntry *& outIE, L3Address& nextHopAddr);

    /**
     * called before a packet is delivered via the network
     */
    IHook::Result datagramPostRoutingHook(INetworkDatagram *datagram, const InterfaceEntry *inIE, const InterfaceEntry *& outIE, L3Address& nextHopAddr);

    /**
     * called before a packet arriving from the network is delivered locally
     */
    IHook::Result datagramLocalInHook(INetworkDatagram *datagram, const InterfaceEntry *inIE);

    /**
     * called before a packet arriving locally is delivered
     */
    IHook::Result datagramLocalOutHook(INetworkDatagram *datagram, const InterfaceEntry *& outIE, L3Address& nextHopAddr);

  public:
    IPv6RPL();
    ~IPv6RPL();

    // Netfilter:
    virtual void registerHook(int priority, IHook *hook) override;
    virtual void unregisterHook(int priority, IHook *hook) override;
    virtual void dropQueuedDatagram(const INetworkDatagram *daragram) override;
    virtual void reinjectQueuedDatagram(const INetworkDatagram *datagram) override;

  protected:
    /**
     * Initialization
     */
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    /**
     * Handle message
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * Processing of IPv6 datagrams. Called when a datagram reaches the front
     * of the queue.
     */
    virtual void endService(cPacket *msg) override;

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

    /**
     * Determines the correct interface for the specified destination address.
     * The nextHop and interfaceId are output parameter.
     */
    bool determineOutputInterface(const IPv6Address& destAddress, IPv6Address& nextHop, int& interfaceId,
            IPv6Datagram *datagram, bool fromHL);

#ifdef WITH_xMIPv6
    /**
     * Process the extension headers of the datagram.
     * Returns true if all have been processed successfully and false if errors occured
     * and the packet has to be dropped or if the datagram has been forwarded to another
     * module for further processing.
     */
    bool processExtensionHeaders(IPv6Datagram *datagram);
#endif /* WITH_xMIPv6 */
};

} // namespace rpl

#endif // ifndef _RPL_SRC_NETWORKLAYER_IPV6_IPV6RPL_H

