
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

#include "src/networklayer/icmpv6/ParentTableRPL.h"

namespace rpl {

Define_Module(ParentTableRPL);

std::ostream& operator<<(std::ostream& os, const ParentTableRPL::ParentEntry& entry)
{
    os << "{IP Address=" << entry.neighbourEntry->nceKey->address << "Rank=" << entry.rank << ", DTSN=" << entry.dtsn << "}";
    return os;
}

ParentTableRPL::ParentTableRPL()
{
    maxParents = -1; //Unlimited
}

void ParentTableRPL::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if(stage == INITSTAGE_LOCAL){
        neighbourDiscoveryRPL = check_and_cast<IPv6NeighbourDiscoveryRPL *>(this->getParentModule()->getSubmodule("neighbourDiscovery"));
        maxParents = par("maxParents");

        WATCH_VECTOR(parentTable);
    }

}

void ParentTableRPL::handleMessage(cMessage *)
{
    throw cRuntimeError("ParentTableRPL::handleMessage: This module doesn't process messages");
}

int ParentTableRPL::getNumberOfParents() const
{
    return parentTable.size();
}


/*
 * Register a new parent at parentTable.
 * -1 if not added. 0 if it is new. 1 if it is updated.
 */

bool ParentTableRPL::updateTable(InterfaceEntry *ie, const IPv6Address& id, int rank, int dtsn)
{
    Enter_Method("ParentTableRPL::updateTableWithAddress()");

    IPv6NeighbourCacheRPL::Neighbour *neighbourEntry = neighbourDiscoveryRPL->neighbourCache.lookup(id, ie->getInterfaceId());

    if (!neighbourEntry){
        throw cRuntimeError("ParentTableRPL::updateTableWithAddress: This IPv6 Address doesn't any entry in Neighbor table!");
    }

    for (unsigned int i = 0; i < parentTable.size(); i++){
        if (neighbourEntry == parentTable.at(i).neighbourEntry){
            // Update existing entry
            ParentEntry &entry = parentTable.at(i);
            EV << "Updating entry in Parent Table: "  << id << " old rank: " << entry.rank << " old DTSN: " << entry.dtsn << " new rank: " << rank << " new DTSN: " << dtsn << "\n";
            entry.rank = rank;
            entry.dtsn = dtsn;
            return 1;
        }
    }

    if ((maxParents != -1) && (getNumberOfParents() == maxParents)){ //If there is not a room,
        if (willWorstRank(rank)) //If my rank will be the worst rank, don't add me to the table.
            return -1;
        else //If my rank will not be the worst rank, remove the worst rank in the table, and add me instead of it.
            removeWorstParent();
    }
    EV << "Adding entry to Parent Table: " << id << " rank: " << rank << " DTSN: " << dtsn << "\n";
    parentTable.push_back(ParentEntry(neighbourEntry, rank, dtsn));
    return 0;
}

bool ParentTableRPL::removeWorstParent()
{
    if (parentTable.size() == 0)
        return false;

    ParentEntry worst = parentTable.at(0);
    unsigned int worstIndex = 0;
    for (unsigned int i = 0; i < parentTable.size(); i++ ) {
        ParentEntry &entry = parentTable.at(i);
        if (entry.rank > worst.rank) {
            worst = entry;
            worstIndex = i;
        }
    }
    parentTable.erase(parentTable.begin() + worstIndex);
    return true;
}

bool ParentTableRPL::willWorstRank(int rank) const
{
    if (parentTable.size() == 0)
        return false;

    for (unsigned int i = 0; i < parentTable.size(); i++) {
        const ParentEntry &entry = parentTable.at(i);
        if (entry.rank > rank) {
            return false;
        }
    }
    return true;
}

const ParentTableRPL::ParentEntry *ParentTableRPL::getParentEntry(const IPv6Address &ipAddr) const
{
    if (parentTable.size() == 0)
        return nullptr;

    for (unsigned int i = 0; i < parentTable.size(); i++) {
        if (parentTable.at(i).neighbourEntry->nceKey->address == ipAddr) {
            return &parentTable.at(i);
        }
    }
    return nullptr;
}

/* If the ipAddr has an entry in the parent table, it returns a pointer to the Neighbour.
 * Otherwise, it returns nullptr.
 */
const IPv6NeighbourCacheRPL::Neighbour *ParentTableRPL::getParentNeighborCache(const IPv6Address &ipAddr) const
{
    const ParentEntry *parentEntry = getParentEntry(ipAddr);

    if (parentEntry)
        return parentEntry->neighbourEntry;
    return nullptr;
}

const ParentTableRPL::ParentEntry *ParentTableRPL::getPrefParentEntry() const
{
    if (parentTable.size() == 0)
        return nullptr;

    unsigned int bestRankIdex = 0;
    for (unsigned int i = 0; i < parentTable.size(); i++) {
        if (parentTable.at(i).rank < parentTable.at(bestRankIdex).rank) {
            bestRankIdex = i;
        }
    }
    return &parentTable.at(bestRankIdex);
}

const IPv6NeighbourCacheRPL::Neighbour *ParentTableRPL::getPrefParentNeighborCache() const
{
    const ParentEntry *parentEntry = getPrefParentEntry();

    if (parentEntry)
        return parentEntry->neighbourEntry;
    return nullptr;
}

bool ParentTableRPL::isPrefParent(const IPv6Address &ipAddr) const
{
    const IPv6NeighbourCacheRPL::Neighbour *neighbor = getPrefParentNeighborCache();

    if ((neighbor) && (neighbor->nceKey->address == ipAddr))
        return true;
    return false;
}

IPv6Address ParentTableRPL::getPrefParentIPAddress() const
{
    const IPv6NeighbourCacheRPL::Neighbour *neighbor = getPrefParentNeighborCache();

    if (neighbor)
        return neighbor->nceKey->address;

    return IPv6Address::UNSPECIFIED_ADDRESS;

}

int ParentTableRPL::getPrefParentDTSN() const
{
    const ParentEntry *parentEntry = getPrefParentEntry();

    if (parentEntry)
        return parentEntry->dtsn;

    //return -1;
    throw cRuntimeError("ParentTableRPL::getPrefParentDTSN(): there is not any parent!");
}

/*
 * Returne the rank for the parent which is specified by ipAddr.
 * Before using this method, we should use the method of
 * getParentNeighborCache() to check if ipAddr is a a parent or not.
 */
int ParentTableRPL::getParentRank(const IPv6Address &ipAddr) const
{
    const ParentEntry *parentEntry = getParentEntry(ipAddr);

    if (parentEntry)
        return parentEntry->rank;

    //return -1;
    throw cRuntimeError("ParentTableRPL::getPrefParentDTSN(): there is not any parent/IP address(%s)!", ipAddr.getSuffix(128).str());
}

/*
 * return the DODAG rank (the best rank) in a non-root node.
 */
int ParentTableRPL::getRank() const
{
    const ParentEntry *parentEntry = getPrefParentEntry();

    if (parentEntry)
        return parentEntry->rank + 1;

    //return -1;
    throw cRuntimeError("ParentTableRPL::getPrefParentDTSN(): there is not any parent!");
}

/*
 * Prints verbose information
 */

void ParentTableRPL::printState() const
{
    EV << endl << "Parent Table" << endl;
    EV << "IP Address    Rank    DTSN" << endl;
    for (unsigned int i = 0; i < parentTable.size(); i++){
        EV << parentTable.at(i).neighbourEntry->nceKey->address << "   " << parentTable.at(i).rank << "   " << parentTable.at(i).dtsn << endl;
    }
}

void ParentTableRPL::clearTable()
{
    parentTable.clear();
}

ParentTableRPL::~ParentTableRPL()
{
    clearTable();
}


} // namespace rpl

