/*
 * this module was inspired by the CSMA module in the INET framework.
 *
 *
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    Develpoed on OMNeT++ 5.2.1 & INET 3.6.3 for using in the IoTorii protocol
*/

/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
 *                    (3) UPC, Castelldefels, Spain.
 *                    Develpoed on OMNeT++ 5.2.1 & INET 3.6.3 by using their proposed interfaces to adapt for using in the RPL and other wireless protocols
*/

 /* -*- mode:c++ -*- ********************************************************
 * file:        CSMA.cc
 *
 * author:     Jerome Rousselot, Marcel Steine, Amre El-Hoiydi,
 *                Marc Loebbers, Yosia Hadisusanto
 *
 * copyright:    (C) 2007-2009 CSEM SA
 *              (C) 2009 T.U. Eindhoven
 *                (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 *
 * Funding: This work was partially financed by the European Commission under the
 * Framework 6 IST Project "Wirelessly Accessible Sensor Populations"
 * (WASP) under contract IST-034963.
 ***************************************************************************
 * part of:    Modifications to the MF-2 framework by CSEM
 **************************************************************************/


//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//


#include "src/linklayer/simpleidealmac/SimpleIdealWirelessMAC.h"

#include <cassert>

#include "inet/common/INETUtils.h"
#include "inet/common/INETMath.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/linklayer/contract/IMACProtocolControlInfo.h"
#include "inet/common/FindModule.h"
#include "inet/linklayer/common/SimpleLinkLayerControlInfo.h"

namespace rpl {

Define_Module(SimpleIdealWirelessMAC);
/*
//Because of LayeredProtocolBase
simsignal_t SimpleIdealWirelessMAC::packetSentToUpperSignal = registerSignal("packetSentToUpper");
simsignal_t SimpleIdealWirelessMAC::packetReceivedFromUpperSignal = registerSignal("packetReceivedFromUpper");
simsignal_t SimpleIdealWirelessMAC::packetFromUpperDroppedSignal = registerSignal("packetFromUpperDropped");

simsignal_t SimpleIdealWirelessMAC::packetSentToLowerSignal = registerSignal("packetSentToLower");
simsignal_t SimpleIdealWirelessMAC::packetReceivedFromLowerSignal = registerSignal("packetReceivedFromLower");
simsignal_t SimpleIdealWirelessMAC::packetFromLowerDroppedSignal = registerSignal("packetFromLowerDropped");
*/

SimpleIdealWirelessMAC::SimpleIdealWirelessMAC() :
    headerLength(0),
    bitrate(0)


{}

void SimpleIdealWirelessMAC::initialize(int stage)
{
    MACProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        bitrate = par("bitrate");
        headerLength = par("headerLength");


        initializeMACAddress();
        registerInterface();

    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        EV_DETAIL << "finished SimpleIdealWirelessMAC init stage 1." << endl;
    }
}

void SimpleIdealWirelessMAC::finish()
{
}

SimpleIdealWirelessMAC::~SimpleIdealWirelessMAC()
{
}

void SimpleIdealWirelessMAC::initializeMACAddress()
{
    const char *addrstr = par("address");

    if (!strcmp(addrstr, "auto")) {
        // assign automatic address
        address = MACAddress::generateAutoAddress();

        // change module parameter from "auto" to concrete address
        par("address").setStringValue(address.str().c_str());
    }
    else {
        address.setAddress(addrstr);
    }
}

InterfaceEntry *SimpleIdealWirelessMAC::createInterfaceEntry()
{
    InterfaceEntry *e = new InterfaceEntry(this);

    // data rate
    e->setDatarate(bitrate);

    // generate a link-layer address to be used as interface token for IPv6
    e->setMACAddress(address);
    e->setInterfaceToken(address.formInterfaceIdentifier());

    // capabilities
    e->setMtu(par("mtu").longValue());
    e->setMulticast(false);
    e->setBroadcast(true);

    return e;
}

/**
 * Encapsulates the message to be transmitted and pass it on
 * to the FSM main method for further processing.
 */
void SimpleIdealWirelessMAC::handleUpperPacket(cPacket *msg)
{
    //MacPkt*macPkt = encapsMsg(msg);
    MACFrameBase *macPkt = new MACFrameBase(msg->getName());
    macPkt->setBitLength(headerLength);
    IMACProtocolControlInfo *const cInfo = check_and_cast<IMACProtocolControlInfo *>(msg->removeControlInfo());
    EV_DETAIL << "CSMA received a message from upper layer, name is " << msg->getName() << ", CInfo removed, mac addr=" << cInfo->getDestinationAddress() << endl;
    MACAddress dest = cInfo->getDestinationAddress();
    macPkt->setDestAddr(dest);
    delete cInfo;
    macPkt->setSrcAddr(address);

    assert(static_cast<cPacket *>(msg));
    macPkt->encapsulate(static_cast<cPacket *>(msg));
    EV_DETAIL << "pkt encapsulated, length: " << macPkt->getBitLength() << "\n";

    sendDown(macPkt);

}

/*
void SimpleIdealWirelessMAC::attachSignal(MACFrameIoTorii *mac, simtime_t_cref startTime)
{
    simtime_t duration = mac->getBitLength() / bitrate;
    mac->setDuration(duration);
}
*/

void SimpleIdealWirelessMAC::handleSelfMessage(cMessage *msg)
{

    EV << "SimpleIdealWirelessMAC Error: unknown selfmessage:" << msg << endl;
}

void SimpleIdealWirelessMAC::handleLowerPacket(cPacket *msg)
{
/*    if (msg->hasBitError()) {
        EV << "Received " << msg << " contains bit errors or collision, dropping it\n";
        delete msg;
        return;
    }
    */
    MACFrameBase *macPkt = check_and_cast<MACFrameBase *>(msg);

    EV << "Received frame name= " << macPkt->getName() << endl;
    const MACAddress& src = macPkt->getSrcAddr();
    const MACAddress& dest = macPkt->getDestAddr();

    EV_DETAIL << "Received frame from  " << src << endl;
    if (dest == address) {
        EV_DETAIL << "Destination MAC adress is " << macPkt->getDestAddr() <<"it is mine, send up packet." << endl;
        sendUp(decapsMsg(macPkt));
    }
    else if (dest.isBroadcast()){
            EV_DETAIL << "Destination MAC adress is broadcast" << macPkt->getDestAddr() <<", send up packet." << endl;
            sendUp(decapsMsg(macPkt));
        }
    else
        delete msg;
    /*
    else if (strcmp(macPkt->getName(), "SetHLMAC") != 0){
        EV << "packet not for me, I send up it to IoTorii sublayer to investigate it.\n";
        executeMac(EV_FRAME_RECEIVED, macPkt);
    }
    */
}

cPacket *SimpleIdealWirelessMAC::decapsMsg(MACFrameBase *macPkt)
{
    cPacket *msg = macPkt->decapsulate();
    setUpControlInfo(msg, macPkt->getSrcAddr());

    return msg;
}


/**
 * Attaches a "control info" (MacToNetw) structure (object) to the message pMsg.
 */
cObject *SimpleIdealWirelessMAC::setUpControlInfo(cMessage *const pMsg, const MACAddress& pSrcAddr)
{
    SimpleLinkLayerControlInfo *const cCtrlInfo = new SimpleLinkLayerControlInfo();
    cCtrlInfo->setSrc(pSrcAddr);
    pMsg->setControlInfo(cCtrlInfo);
    return cCtrlInfo;
}

} // namespace inet

