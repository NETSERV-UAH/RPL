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
 * file:        CSMA.h
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

#ifndef __RPL_SRC_LINKLAYER_SIMPLEIDEALMAC_SIMPLEIDEALWIRELESMAC_H
#define __RPL_SRC_LINKLAYER_SIMPLEIDEALMAC_SIMPLEIDEALWIRELESMAC_H

#include "inet/linklayer/contract/IMACProtocol.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/base/MACProtocolBase.h"
 #include "inet/linklayer/base/MACFrameBase_m.h"

namespace rpl {

using namespace inet;

class SimpleIdealWirelessMAC : public MACProtocolBase, public IMACProtocol
{

/*    //Because of from LayeredProtocolBase
    public:
      static simsignal_t packetSentToUpperSignal;
      static simsignal_t packetReceivedFromUpperSignal;
      static simsignal_t packetFromUpperDroppedSignal;

      static simsignal_t packetSentToLowerSignal;
      static simsignal_t packetReceivedFromLowerSignal;
      static simsignal_t packetFromLowerDroppedSignal;
*/
  public:
    /** @brief Length of the header*/
    int headerLength;

    /** @brief the bit rate at which we transmit */
    double bitrate;


  public:
    SimpleIdealWirelessMAC();

    virtual ~SimpleIdealWirelessMAC();

    virtual MACAddress getMACAddress() { return address; }

    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int) override;

    /** @brief Delete all dynamically allocated objects of the module*/
    virtual void finish() override;

    /** @brief Handle messages from lower layer */
    virtual void handleLowerPacket(cPacket *) override;

    /** @brief Handle messages from upper layer */
    virtual void handleUpperPacket(cPacket *) override;

    /** @brief Handle self messages such as timers */
    virtual void handleSelfMessage(cMessage *) override;

  protected:
    /** @brief The MAC address of the interface. */
    MACAddress address;

  protected:
    /** @brief Generate new interface address*/
    virtual void initializeMACAddress();
    virtual InterfaceEntry *createInterfaceEntry() override;
    virtual cPacket *decapsMsg(MACFrameBase *macPkt);

    virtual cObject *setUpControlInfo(cMessage *const pMsg, const MACAddress& pSrcAddr);
//  cObject* setDownControlInfo(cMessage * const pMsg, Signal * const pSignal);

  private:
    /** @brief Copy constructor is not allowed.
     */
    SimpleIdealWirelessMAC(const SimpleIdealWirelessMAC&);
    /** @brief Assignment operator is not allowed.
     */
    SimpleIdealWirelessMAC& operator=(const SimpleIdealWirelessMAC&);
};

} // namespace rpl

#endif // ifndef __RPL_SRC_LINKLAYER_SIMPLEIDEALMAC_SIMPLEIDEALWIRELESMAC_H

