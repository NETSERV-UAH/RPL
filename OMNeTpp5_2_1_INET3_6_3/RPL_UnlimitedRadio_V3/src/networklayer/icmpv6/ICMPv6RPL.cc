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

#include "inet/common/INETDefs.h"

#include "inet/networklayer/ipv6/IPv6InterfaceData.h"

#include "inet/networklayer/common/IPSocket.h"
//#include "inet/networklayer/contract/ipv6/IPv6ControlInfo.h"
#include "inet/networklayer/ipv6/IPv6Datagram.h"

#include "inet/networklayer/contract/IInterfaceTable.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeStatus.h"

//#include "inet/applications/pingapp/PingPayload_m.h"
//EXTRA
#include "src/networklayer/icmpv6/ICMPv6RPL.h"



namespace inet {
//foreign declarations:
class IPv6Address;
}
namespace rpl {
using namespace inet;


Define_Module(ICMPv6RPL);

void ICMPv6RPL::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    //EXTRA BEGIN
    if(stage == INITSTAGE_LOCAL){
        host = getContainingNode(this);
        mop = (RPLMOP)this->getParentModule()->getSubmodule("rplUpwardRouting")->par("modeOfOperation");  //EXTRA
        rplUpwardRouting = check_and_cast<RPLUpwardRouting *>(this->getParentModule()->getSubmodule("rplUpwardRouting"));  //EXTRA
    }
    //EXTRA END

    if (stage == INITSTAGE_NETWORK_LAYER) {
        bool isOperational;
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
    if (stage == INITSTAGE_NETWORK_LAYER_2) {
        IPSocket socket(gate("ipv6Out"));
        socket.registerProtocol(IP_PROT_IPv6_ICMP);
    }
}

void ICMPv6RPL::handleMessage(cMessage *msg)
{
    EV << "->ICMPv6RPL::handleMessage()" << endl;

    ASSERT(!msg->isSelfMessage());    // no timers in ICMPv6

    // process arriving ICMP message
    if (msg->getArrivalGate()->isName("ipv6In")) {
        EV_INFO << "Processing ICMPv6 message.\n";
        processICMPv6Message(check_and_cast<ICMPv6Message *>(msg));
        return;
    }

    // request from application
    if (msg->getArrivalGate()->isName("pingIn")) {
        sendEchoRequest(check_and_cast<PingPayload *>(msg));
        return;
    }

    //EXTRA
    // request from RPL
    if (msg->getArrivalGate()->isName("RPLIn")) {
        EV << "Message " << msg->getName() << "received from RPL module is sent to the IPv6 module."<< endl;
        send(msg, "ipv6Out");
        return;
    }
    EV << "<-ICMPv6RPL::handleMessage()" << endl;
}

void ICMPv6RPL::processICMPv6Message(ICMPv6Message *icmpv6msg)
{
    EV << "->ICMPv6RPL::processICMPv6Message()" << endl;
    ASSERT(dynamic_cast<ICMPv6Message *>(icmpv6msg));
    if (dynamic_cast<ICMPv6DestUnreachableMsg *>(icmpv6msg)) {
        EV_INFO << "ICMPv6 Destination Unreachable Message Received." << endl;
        errorOut(icmpv6msg);
    }
    else if (dynamic_cast<ICMPv6PacketTooBigMsg *>(icmpv6msg)) {
        EV_INFO << "ICMPv6 Packet Too Big Message Received." << endl;
        errorOut(icmpv6msg);
    }
    else if (dynamic_cast<ICMPv6TimeExceededMsg *>(icmpv6msg)) {
        EV_INFO << "ICMPv6 Time Exceeded Message Received." << endl;
        errorOut(icmpv6msg);
    }
    else if (dynamic_cast<ICMPv6ParamProblemMsg *>(icmpv6msg)) {
        EV_INFO << "ICMPv6 Parameter Problem Message Received." << endl;
        errorOut(icmpv6msg);
    }
    else if (dynamic_cast<ICMPv6EchoRequestMsg *>(icmpv6msg)) {
        EV_INFO << "ICMPv6 Echo Request Message Received." << endl;
        processEchoRequest((ICMPv6EchoRequestMsg *)icmpv6msg);
    }
    else if (dynamic_cast<ICMPv6EchoReplyMsg *>(icmpv6msg)) {
        EV_INFO << "ICMPv6 Echo Reply Message Received." << endl;
        processEchoReply((ICMPv6EchoReplyMsg *)icmpv6msg);
    } //EXTRA BEGIN
    else if (dynamic_cast<ICMPv6DIOMsg *>(icmpv6msg)){
        EV << "Message " << icmpv6msg->getName() << " received from IPv6 module is sent to RPL module."<< endl;
        sendToRPL(icmpv6msg);
    }else if (dynamic_cast<ICMPv6DAOMsg *>(icmpv6msg)){
        if ((mop ==     Storing_Mode_of_Operation_with_no_multicast_support) || (mop == Storing_Mode_of_Operation_with_multicast_support))
            processIncommingStoringDAOMessage(icmpv6msg);
    }else if (dynamic_cast<ICMPv6DISMsg *>(icmpv6msg)){
            processIncommingDISMessage(icmpv6msg);
    }else//EXTRA END
        throw cRuntimeError("Unknown message type received: (%s)%s.\n", icmpv6msg->getClassName(),icmpv6msg->getName());
    EV << "<-ICMPv6RPL::processICMPv6Message()" << endl;
}

//EXTRA BEGIN
//////////// DIS Operations ////////////////////
void ICMPv6RPL::processIncommingDISMessage(ICMPv6Message *msg)
{
    EV << "->ICMPv6RPL::handleIncommingDISMessage()" << endl;

    ICMPv6DISMsg *netwMsg = check_and_cast<ICMPv6DISMsg *>(msg);
    IPv6ControlInfo *ctrlInfo = check_and_cast<IPv6ControlInfo *> (netwMsg->removeControlInfo());

    if ((msg->getKind() == DIS_FLOOD) && (rplUpwardRouting->isNodeJoinedToDAG())){
        DIS_c++;
        numReceivedDIS++;
        char buf2[255];
        sprintf(buf2, "A DIS message received from node %d!\nResetting Trickle timer!", ctrlInfo->getSrcAddr());
        host->bubble(buf2);
        rplUpwardRouting->TrickleReset();
        rplUpwardRouting->scheduleNextDIOTransmission();
    }else if ((msg->getKind() == DIS_FLOOD) && (!rplUpwardRouting->isNodeJoinedToDAG())){
        DIS_c++;
        numReceivedDIS++;
        char buf2[255];
        sprintf(buf2, "A DIS message received from node %s!\nBut I am not a member of any DODAG!", ctrlInfo->getSrcAddr());
        host->bubble(buf2);
    }

    delete ctrlInfo;

    delete netwMsg;

    EV << "<-ICMPv6RPL::handleIncommingDISMessage()" << endl;
}


void ICMPv6RPL::SetDISParameters()
{
    Enter_Method("SetDISParameters()");

    if (DISTimer){  //EXTRA
        cancelAndDelete(DISTimer);
        DISTimer = nullptr;
    }

    //DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);  EXTRA
    DIS_CurIntsizeNext=DISIntMin;
    DIS_StofCurIntNext =DISStartDelay+DODAGSartTime;
    DIS_EndofCurIntNext=DIS_StofCurIntNext+DIS_CurIntsizeNext;
}

void ICMPv6RPL::scheduleNextDISTransmission()
{
    Enter_Method("scheduleNextDISTransmission()");
    DIS_CurIntsizeNow = DIS_CurIntsizeNext;
    DIS_StofCurIntNow = DIS_StofCurIntNext;
    DIS_EndofCurIntNow = DIS_EndofCurIntNext;
    TimetoSendDIS=DIS_StofCurIntNow+uniform(0,DIS_CurIntsizeNow/2)+(DIS_CurIntsizeNow/2);

    if (DISTimer)//EXTRA
        throw cRuntimeError("ICMPv6RPL::scheduleNextDISTransmission: DIS Timer must be nullptr.");
    else
        DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);

    scheduleAt(TimetoSendDIS,DISTimer );
    DIS_CurIntsizeNext*=2;
    if (DIS_CurIntsizeNext>DISIMaxLength)
        DIS_CurIntsizeNext=DISIMaxLength;
    DIS_StofCurIntNext = DIS_EndofCurIntNext;
    DIS_EndofCurIntNext=DIS_StofCurIntNext+DIS_CurIntsizeNext;
    DIS_c=0;
}

void ICMPv6RPL::handleDISTimer(cMessage* msg)
{

    if(((!rplUpwardRouting->isNodeJoinedToDAG())&&((DIS_c<DISRedun)||(DISRedun==0)))&&(DISVersion== rplUpwardRouting->getVersion())) //EXTRA
    //if(((!IsJoined)&&((DIS_c<DISRedun)||(DISRedun==0)))&&(DISVersion==Version))  //EXTRA
    {
        ICMPv6DISMsg* pkt = new ICMPv6DISMsg("DIS", DIS_FLOOD);
        IPv6ControlInfo *controlInfo = new IPv6ControlInfo;
        controlInfo->setSrcAddr(myNetwAddr);
        controlInfo->setDestAddr(IPv6Address::ALL_NODES_2);
        pkt->setByteLength(DISheaderLength);
        pkt->setVersionNumber(rplUpwardRouting->getVersion());
        pkt->setDODAGID(rplUpwardRouting->getDODAGID()); ?? why dodagid!!?? since !isNodeJoinedToDAG??;
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        controlInfo->setProtocol(IP_PROT_IPv6_ICMP);
        pkt->setControlInfo(controlInfo);
        send(pkt, "ipv6Out");

        if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIS.Sent++;
        DISStatusLast->nbDISSent++;
        cancelAndDelete(DISTimer);
        //DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER); //EXTRA
        DISTimer = nullptr;  //EXTRA
        scheduleNextDISTransmission();
        return;
    }
    else
        //if(((!IsJoined)&&(DIS_c>=DISRedun))&&(DISVersion==Version)) //EXTRA
        if(((!IsNodeJoined[pManagerRPL->getIndexFromAddress(myNetwAddr)])&&(DIS_c>=DISRedun))&&(DISVersion==Version)) //EXTRA
        {
            if ((NodeCounter_Upward[Version]<NodesNumber)&&(!IsDODAGFormed_Upward))  NodeStateLast->DIS.Suppressed++;  // if !end simulation

            DISStatusLast->nbDISSuppressed++;
            char buf1[100];
            sprintf(buf1, "DIS transmission suppressed!");
            host->bubble(buf1);
            //delete msg; //EXTRA
            cancelAndDelete(DISTimer);
            //DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER); //EXTRA
            DISTimer = nullptr;  //EXTRA
            scheduleNextDISTransmission();
            return;

        }
        else
            //if(IsJoined) //EXTRA
            if(IsNodeJoined[pManagerRPL->getIndexFromAddress(myNetwAddr)]) //EXTRA
            {
                cancelAndDelete(DISTimer);
                //DISTimer = new cMessage("DIS-timer", SEND_DIS_FLOOD_TIMER);  //EXTRA
                DISTimer = nullptr;  //EXTRA
            }
    //EXTRA : addin a condition : if joined Version > disversion, disc < dis redun, disc=0 ==> send dio
    //EXTRA : if not joined || dis_c > dis redun || version < disversion == > suppressed
}

//////////// DAO Operations ////////////////////
void ICMPv6RPL::processIncommingStoringDAOMessage(ICMPv6Message *msg)
{

    EV << "->ICMPV6RPL::processIncommingStoringDAOMessage()" << endl;

    ICMPv6DAOMsg* pkt = check_and_cast<ICMPv6DAOMsg*>(msg);
    IPv6ControlInfo *ctrlInfoIn = check_and_cast<IPv6ControlInfo *>(pkt->removeControlInfo());
    IPv6Address prefix = pkt->getPrefix();
    int prefixLen = pkt->getPrefixLen();
    simtime_t lifeTime = pkt->getLifeTime();
    int flagK = pkt->getKFlag();
    IPv6Address senderIPAddress = ctrlInfoIn->getSrcAddr(); //Sender address

    EV << "Received message is ICMPv6 DAO message, DODAGID address is " << pkt->getDODAGID() << ", src address is " << ctrlInfoIn->getSrcAddr() << endl;

    int parentIndex = getParentIndex(senderIPAddress);

    if (parentIndex >= 0){
        if (Parents[VersionNember][parentIndex].ParentRank < Rank){
            EV << "DAO message is received from a parent with a lower rank and discarded." << endl;
            delete msg;
            return;
        }

        if (senderIPAddress == PrParent){ // parentIndex = 0
            EV << "DAO message is received from the preferred parent and discarded." << endl;
            delete msg;
            return;
        }

    }

    RoutingTable *routingTable = routingTables.at(VersionNember);


    if (lifeTime == ZERO_LIFETIME){ // && (flagK == 1)  //No-Path DAO  //FIXME: this situation must handle ACK message
        auto iter = routingTable->find(prefix);
        if (iter != routingTable->end()){   // prefix has a route
            RoutingEntry& entry = iter->second;
            bool removeable = false;
            if ((!entry.NoPathReceived) && (entry.prefixLen == prefixLen) && (entry.nextHop == senderIPAddress)){
                removeable = true;
                EV << "No-Path DAO received from " << senderIPAddress <<" removes a route with the prefeix of " << prefix << endl;
                entry.NoPathReceived = true;
                //TODO: setting lifetime label on the route
                routingTable->erase(prefix);
            }
            EV << "Deleting the entry :" << prefix << ", nextHop" << senderIPAddress << "\n";
        }
    }else{
        auto iter = routingTable->find(prefix);
        if (iter == routingTable->end()){
            // Add entry to table
            EV << "Adding entry to Routing Table: " << prefix << " nextHop -->" << senderIPAddress << "\n";
            (*routingTable)[prefix] = RoutingEntry( senderIPAddress, simTime() + lifeTime);
        } else {
            // Update existing entry
            EV << "Updating entry in Address Table: " << prefix << " nextHop -->" << senderIPAddress << "\n";
            RoutingEntry& entry = iter->second;
            entry.nextHop = senderIPAddress;
            entry.lifeTime = simTime() + lifeTime;
        }
    }


    if (NofParents > 0){  //this node has a preferred parent, so DAO must forward to the preferred parent.
        IPv6ControlInfo *ctrlInfoOut = new IPv6ControlInfo;
        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        ctrlInfoOut->setProtocol(IP_PROT_IPv6_ICMP);
        ctrlInfoOut->setSrcAddr(myNetwAddr);
        ctrlInfoOut->setDestAddr(Parents[VersionNember][0].ParentId);  //ctrlInfo->setDestAddr(PrParent)
        EV << "A DAO message is sent from : " << ctrlInfoOut->getSourceAddress() << " to parent : " << ctrlInfoOut->getDestinationAddress() << " to advertise prefix : " << prefix << "with prefix len : " << prefixLen << endl;
        pkt->setControlInfo(ctrlInfoOut);
        send(pkt, icmpv6OutGateId);
    }
    else{
        EV<< "DAO can not be sent. There is no preferred parent." << endl;
        delete msg;
    }

    delete ctrlInfoIn;

    EV << "<-ICMPV6RPL::processIncommingStoringDAOMessage()" << endl;

}

void ICMPV6RPL::sendDAOMessage(IPv6Address prefix, simtime_t lifetime)
{

    EV << "->ICMPV6RPL::sendDAOMessage()" << endl;

    if (NofParents[VersionNember] > 0){
        ICMPv6DAOMsg* pkt = new ICMPv6DAOMsg("DAO", DAO);
        IPv6ControlInfo *ctrlInfo = new IPv6ControlInfo;
        ctrlInfo->setProtocol(IP_PROT_IPv6_ICMP);
        ctrlInfo->setSrcAddr(myNetwAddr);
        ctrlInfo->setDestAddr(Parents[VersionNember][0].ParentId);  //ctrlInfo->setDestAddr(PrParent)
        pkt->setByteLength(DAOheaderLength);
        pkt->setDFlag(1); //spesified DAG
        pkt->setDODAGID(DODAGID);

        if (lifetime == ZERO_LIFETIME)
            pkt->setKFlag(1);
        else
            pkt->setKFlag(0);

        pkt->setType(ICMPv6_RPL_CONTROL_MESSAGE);
        pkt->setPrefixLen(64); //link local address
        pkt->setPrefix(prefix);
        pkt->setLifeTime(lifetime);
        pkt->setControlInfo(ctrlInfo);
        send(pkt, icmpv6OutGateId);
    }else
        EV << "Cancel a generated DAO, there is no preferred parent." << endl;


    EV << "<-ICMPV6RPL::sendDAOMessage()" << endl;

}

void ICMPV6RPL::scheduleNextDAOTransmission(simtime_t delay, simtime_t LifeTime)
{
    EV << "->ICMPV6RPL::scheduleNextDAOTransmission()" << endl;


    if (DAOTimer){
        EV << "DAO timer already scheduled." << endl;
    }else{
        DAOTimer = new cMessage("DAO-timer", SEND_DAO_TIMER);
        simtime_t expirationTime;
        if (delay == 0){
            expirationTime = 0;
            EV << "Next DAO message is scheduled at t = " << expirationTime << ", delay parameter = " << delay << endl;
        }
        else if (delay == DelayDAO){
            expirationTime = par("randomDelayDAO"); //expirationTime = x~[0,1) * delay + delay/2 or x~[0.5,1.5) * delay
            EV << "DAO message is scheduled at t = " << expirationTime << ", delay parameter = DelayDAO = " << DelayDAO << endl;
        }else{
            throw cRuntimeError("ICMPV6RPL::scheduleNextDAOTransmission: the value of the delay parameter is wrong! delay = %e'", delay);
        }
        scheduleAt(expirationTime, DAOTimer);

        //based on the Contiki-ng ambiguity, there is 3 interpretation:
        //1:
        if (DAOLifeTimer){
            EV << "DAOLifeTimer already scheduled. It is again rescheduled now." << endl;
            //cancelAndDelete(DAOLifeTimer);
            cancelAndDelete(DAOLifeTimer);
            DAOLifeTimer = nullptr;
        }else
            EV << "DAOLifeTimer was not scheduled. It is scheduled now." << endl;

        scheduleDAOlifetimer(LifeTime);




    /*      //2:
            if (!DAOLifeTimer){
                EV << "DAOLifeTimer was not scheduled. It is scheduled." << endl;
                scheduleDAOlifetimer(LifeTime);
            }else{
                EV << "DAOLifeTimer already scheduled. It is not again scheduled." << endl;
            }


            //3: Contiki approach, error on OMNeT?
            scheduleDAOlifetimer(LifeTime);
    */
        }
        EV << "<-ICMPV6RPL::scheduleNextDAOTransmission()" << endl;

}

void ICMPV6RPL::scheduleDAOlifetimer(simtime_t lifeTime)
{
    EV << "->ICMPV6RPL::scheduleDAOlifetimer()" << endl;

    if (lifeTime == ROUTE_INFINITE_LIFETIME){
        EV << "DAO life time is infinit. DAOLifeTimer is not scheduled." << endl;
    }else if (!DAOLifeTimer){
        DAOLifeTimer = new cMessage("DAO-life-timer", DAO_LIFETIME_TIMER);
        if (lifeTime == defaultLifeTime){
               simtime_t expirationTime = par("randomDefaultLifeTime"); //expirationTime = x~[0.5,0.75) * lifeTime
               scheduleAt(expirationTime, DAOLifeTimer);
               EV << "The DAO life time is not infinit. The next DAO is scheduled at t = " << expirationTime << ", the lifeTime parameter = defaultLifeTime = " << defaultLifeTime << endl;
           }else{
               throw cRuntimeError("ICMPV6RPL::scheduleDAOlifetimer: the value of the input parameter is wrong! lifeTime = %e'", lifeTime);
           }
    }else{
        throw cRuntimeError("ICMPV6RPL::scheduleDAOlifetimer: DAOLifeTimer must be nullptr.");
    }

    EV << "->ICMPV6RPL::scheduleDAOlifetimer()" << endl;
}

void ICMPV6RPL::DeleteDAOTimers()
{
    Enter_Method("DeleteDAOTimers()");
    if (DAOTimer){
        cancelAndDelete(DAOTimer);
        DAOTimer = nullptr;
    }

    if (DAOLifeTimer){
        cancelAndDelete(DAOLifeTimer);
        DAOLifeTimer = nullptr;
    }

}


//EXTRA END

/*
 * RFC 4443 4.2:
 *
 * Every node MUST implement an ICMPv6 Echo responder function that
 * receives Echo Requests and originates corresponding Echo Replies.  A
 * node SHOULD also implement an application-layer interface for
 * originating Echo Requests and receiving Echo Replies, for diagnostic
 * purposes.
 *
 * The source address of an Echo Reply sent in response to a unicast
 * Echo Request message MUST be the same as the destination address of
 * that Echo Request message.
 *
 * An Echo Reply SHOULD be sent in response to an Echo Request message
 * sent to an IPv6 multicast or anycast address.  In this case, the
 * source address of the reply MUST be a unicast address belonging to
 * the interface on which the Echo Request message was received.
 *
 * The data received in the ICMPv6 Echo Request message MUST be returned
 * entirely and unmodified in the ICMPv6 Echo Reply message.
 */
void ICMPv6RPL::processEchoRequest(ICMPv6EchoRequestMsg *request)
{
    //Create an ICMPv6 Reply Message
    ICMPv6EchoReplyMsg *reply = new ICMPv6EchoReplyMsg("Echo Reply");
    reply->setName((std::string(request->getName()) + "-reply").c_str());
    reply->setType(ICMPv6_ECHO_REPLY);
    reply->encapsulate(request->decapsulate());

    IPv6ControlInfo *ctrl = check_and_cast<IPv6ControlInfo *>(request->getControlInfo());
    IPv6ControlInfo *replyCtrl = new IPv6ControlInfo();
    replyCtrl->setProtocol(IP_PROT_IPv6_ICMP);
    replyCtrl->setDestAddr(ctrl->getSrcAddr());

    if (ctrl->getDestAddr().isMulticast()    /*TODO check for anycast too*/) {
        IInterfaceTable *it = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        IPv6InterfaceData *ipv6Data = it->getInterfaceById(ctrl->getInterfaceId())->ipv6Data();
        replyCtrl->setSrcAddr(ipv6Data->getPreferredAddress());
        // TODO implement default address selection properly.
        //      According to RFC 3484, the source address to be used
        //      depends on the destination address
    }
    else
        replyCtrl->setSrcAddr(ctrl->getDestAddr());

    reply->setControlInfo(replyCtrl);

    delete request;
    sendToIP(reply);
}

void ICMPv6RPL::processEchoReply(ICMPv6EchoReplyMsg *reply)
{
    IPv6ControlInfo *ctrl = check_and_cast<IPv6ControlInfo *>(reply->removeControlInfo());
    PingPayload *payload = check_and_cast<PingPayload *>(reply->decapsulate());
    payload->setControlInfo(ctrl);
    delete reply;
    long originatorId = payload->getOriginatorId();
    auto i = pingMap.find(originatorId);
    if (i != pingMap.end())
        send(payload, "pingOut", i->second);
    else {
        EV_WARN << "Received ECHO REPLY has an unknown originator ID: " << originatorId << ", packet dropped." << endl;
        delete payload;
    }
}

void ICMPv6RPL::sendEchoRequest(PingPayload *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    int i = arrivalGate->getIndex();
    pingMap[msg->getOriginatorId()] = i;

    IPv6ControlInfo *ctrl = check_and_cast<IPv6ControlInfo *>(msg->removeControlInfo());
    ctrl->setProtocol(IP_PROT_IPv6_ICMP);
    ICMPv6EchoRequestMsg *request = new ICMPv6EchoRequestMsg(msg->getName());
    request->setType(ICMPv6_ECHO_REQUEST);
    request->encapsulate(msg);
    request->setControlInfo(ctrl);
    sendToIP(request);
}

void ICMPv6RPL::sendErrorMessage(IPv6Datagram *origDatagram, ICMPv6Type type, int code)
{
    Enter_Method("sendErrorMessage(datagram, type=%d, code=%d)", type, code);

    // get ownership
    take(origDatagram);

    if (!validateDatagramPromptingError(origDatagram))
        return;

    ICMPv6Message *errorMsg;

    if (type == ICMPv6_DESTINATION_UNREACHABLE)
        errorMsg = createDestUnreachableMsg(code);
    //TODO: implement MTU support.
    else if (type == ICMPv6_PACKET_TOO_BIG)
        errorMsg = createPacketTooBigMsg(0);
    else if (type == ICMPv6_TIME_EXCEEDED)
        errorMsg = createTimeExceededMsg(code);
    else if (type == ICMPv6_PARAMETER_PROBLEM)
        errorMsg = createParamProblemMsg(code);
    else
        throw cRuntimeError("Unknown ICMPv6 error type: %d\n", type);

    // Encapsulate the original datagram, but the whole ICMPv6 error
    // packet cannot be larger than the minimum IPv6 MTU (RFC 4443 2.4. (c)).
    // NOTE: since we just overwrite the errorMsg length without actually
    // truncating origDatagram, one can get "packet length became negative"
    // error when decapsulating the origDatagram on the receiver side.
    // A workaround is to avoid decapsulation, or to manually set the
    // errorMessage length to be larger than the encapsulated message.
    errorMsg->encapsulate(origDatagram);
    if (errorMsg->getByteLength() + IPv6_HEADER_BYTES > IPv6_MIN_MTU)
        errorMsg->setByteLength(IPv6_MIN_MTU - IPv6_HEADER_BYTES);

    // debugging information
    EV_DEBUG << "sending ICMP error: (" << errorMsg->getClassName() << ")" << errorMsg->getName()
             << " type=" << type << " code=" << code << endl;

    // if srcAddr is not filled in, we're still in the src node, so we just
    // process the ICMP message locally, right away
    if (origDatagram->getSrcAddress().isUnspecified()) {
        // pretend it came from the IP layer
        IPv6ControlInfo *ctrlInfo = new IPv6ControlInfo();
        ctrlInfo->setSrcAddr(IPv6Address::LOOPBACK_ADDRESS);    // FIXME maybe use configured loopback address
        ctrlInfo->setProtocol(IP_PROT_ICMP);
        errorMsg->setControlInfo(ctrlInfo);

        // then process it locally
        processICMPv6Message(errorMsg);
    }
    else {
        sendToIP(errorMsg, origDatagram->getSrcAddress());
    }
}

void ICMPv6RPL::sendErrorMessage(cPacket *transportPacket, IPv6ControlInfo *ctrl, ICMPv6Type type, int code)
{
    Enter_Method("sendErrorMessage(transportPacket, ctrl, type=%d, code=%d)", type, code);

    IPv6Datagram *datagram = ctrl->removeOrigDatagram();
    delete ctrl;
    take(transportPacket);
    take(datagram);
    datagram->encapsulate(transportPacket);
    sendErrorMessage(datagram, type, code);
}

void ICMPv6RPL::sendToIP(ICMPv6Message *msg, const inet::IPv6Address& dest)
{
    IPv6ControlInfo *ctrlInfo = new IPv6ControlInfo();
    ctrlInfo->setDestAddr(dest);
    ctrlInfo->setProtocol(IP_PROT_IPv6_ICMP);
    msg->setControlInfo(ctrlInfo);

    send(msg, "ipv6Out");
}

//EXTRA
void ICMPv6RPL::sendToRPL(ICMPv6Message *msg)
{
    // assumes IPv6ControlInfo is already attached
    send(msg, "RPLOut");
}

void ICMPv6RPL::sendToIP(ICMPv6Message *msg)
{
    // assumes IPv6ControlInfo is already attached
    send(msg, "ipv6Out");
}

ICMPv6Message *ICMPv6RPL::createDestUnreachableMsg(int code)
{
    ICMPv6DestUnreachableMsg *errorMsg = new ICMPv6DestUnreachableMsg("Dest Unreachable");
    errorMsg->setType(ICMPv6_DESTINATION_UNREACHABLE);
    errorMsg->setCode(code);
    return errorMsg;
}

ICMPv6Message *ICMPv6RPL::createPacketTooBigMsg(int mtu)
{
    ICMPv6PacketTooBigMsg *errorMsg = new ICMPv6PacketTooBigMsg("Packet Too Big");
    errorMsg->setType(ICMPv6_PACKET_TOO_BIG);
    errorMsg->setCode(0);    //Set to 0 by sender and ignored by receiver.
    errorMsg->setMTU(mtu);
    return errorMsg;
}

ICMPv6Message *ICMPv6RPL::createTimeExceededMsg(int code)
{
    ICMPv6TimeExceededMsg *errorMsg = new ICMPv6TimeExceededMsg("Time Exceeded");
    errorMsg->setType(ICMPv6_TIME_EXCEEDED);
    errorMsg->setCode(code);
    return errorMsg;
}

ICMPv6Message *ICMPv6RPL::createParamProblemMsg(int code)
{
    ICMPv6ParamProblemMsg *errorMsg = new ICMPv6ParamProblemMsg("Parameter Problem");
    errorMsg->setType(ICMPv6_PARAMETER_PROBLEM);
    errorMsg->setCode(code);
    //TODO: What Pointer? section 3.4
    return errorMsg;
}

bool ICMPv6RPL::validateDatagramPromptingError(IPv6Datagram *origDatagram)
{
    // don't send ICMP error messages for multicast messages
    if (origDatagram->getDestAddress().isMulticast()) {
        EV_INFO << "won't send ICMP error messages for multicast message " << origDatagram << endl;
        delete origDatagram;
        return false;
    }

    // do not reply with error message to error message
    if (origDatagram->getTransportProtocol() == IP_PROT_IPv6_ICMP) {
        ICMPv6Message *recICMPMsg = check_and_cast<ICMPv6Message *>(origDatagram->getEncapsulatedPacket());
        if (recICMPMsg->getType() < 128) {
            EV_INFO << "ICMP error received -- do not reply to it" << endl;
            delete origDatagram;
            return false;
        }
    }
    return true;
}

void ICMPv6RPL::errorOut(ICMPv6Message *icmpv6msg)
{
    send(icmpv6msg, "errorOut");
}

bool ICMPv6RPL::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    //pingMap.clear();
    throw cRuntimeError("Lifecycle operation support not implemented");
}

} // namespace rpl

