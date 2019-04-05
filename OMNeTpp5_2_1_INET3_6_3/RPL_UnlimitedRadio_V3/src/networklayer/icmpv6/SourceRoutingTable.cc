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

#include <vector>

#include "src/networklayer/icmpv6/SourceRoutingTable.h"
#include "src/networklayer/icmpv6/RPLUpwardRouting.h"

namespace rpl {

Define_Module(SourceRoutingTable);

std::ostream& operator<<(std::ostream& os, const SourceRoutingTable::SREntry& entry)
{
    os << "{prefix/dst Address=" << entry.prefix << "/" << entry.prefixLen << ", DAO Parent=" << entry.daoParent << ", expr=" << entry.expr << "}";
    return os;
}

SourceRoutingTable::SourceRoutingTable()
{
    pRPLUpwardRouting = nullptr;
}

void SourceRoutingTable::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    pRPLUpwardRouting = check_and_cast<RPLUpwardRouting *>(this->getParentModule()->getSubmodule("rplUpwardRouting"));
    if(stage == INITSTAGE_LOCAL){
        WATCH_VECTOR(srTable);
    }
}

void SourceRoutingTable::handleMessage(cMessage *)
{
    throw cRuntimeError("SourceRoutingTable::handleMessage: This module doesn't process messages");
}

int SourceRoutingTable::getNumberOfRoutes() const
{
    return srTable.size();
}

bool SourceRoutingTable::isRoute(const IPv6Address &prefix, int prefixLen) const
{
    for (unsigned int i = 0; i < srTable.size(); i++){
        if ((srTable.at(i).prefix == prefix) && (srTable.at(i).prefixLen == prefixLen))
            return true;
    }
    return false;
}

void SourceRoutingTable::addRoute(const IPv6Address &prefix, int prefixLen, const IPv6Address &daoParent, int interfaceID, simtime_t exp)
{
    if (getNumberOfRoutes() == 0)
        srTable.push_back(SREntry(IPv6Address::UNSPECIFIED_ADDRESS, 128, pRPLUpwardRouting->getMyGlobalNetwAddr(), interfaceID, exp));
    //Delete old route if there is
    if (isRoute(prefix, prefixLen))
        deleteRoute(prefix, prefixLen);
    //Insert new route
    srTable.push_back(SREntry(prefix, prefixLen, daoParent, interfaceID, exp));

}

void SourceRoutingTable::deleteRoute(const IPv6Address &prefix, int prefixLen)
{
    for (unsigned int i = 0; i < srTable.size(); i++){
        if ((srTable.at(i).prefix == prefix) && (srTable.at(i).prefixLen == prefixLen))
            srTable.erase(srTable.begin() + i);
    }
}

//This method is used by the No-Path DAO message, so it must check the daoParent field to avoid deleting a fresh route,i.e., if a No-Path DAO message is received late.
void SourceRoutingTable::deleteRoute(const IPv6Address &prefix, int prefixLen, const IPv6Address &daoParent)
{
    for (unsigned int i = 0; i < srTable.size(); i++){
        if ((srTable.at(i).prefix == prefix) && (srTable.at(i).prefixLen == prefixLen) && (srTable.at(i).daoParent == daoParent))
            srTable.erase(srTable.begin() + i);
    }
}


/*
 * Prints verbose information
 */

void SourceRoutingTable::printState() const
{
    EV << endl << "Source Routing Table" << endl;
    EV << "Prefix/dst Address     DAO Parent     EXPR" << endl;
    for (unsigned int i = 0; i < srTable.size(); i++){
        EV << srTable.at(i).prefix << "/" << srTable.at(i).prefixLen << "    " << srTable.at(i).daoParent << "    " << srTable.at(i).expr << endl;
    }
}

void SourceRoutingTable::clearTable()
{
    srTable.clear();
}

SourceRoutingTable::~SourceRoutingTable()
{
    clearTable();
}


} // namespace rpl

