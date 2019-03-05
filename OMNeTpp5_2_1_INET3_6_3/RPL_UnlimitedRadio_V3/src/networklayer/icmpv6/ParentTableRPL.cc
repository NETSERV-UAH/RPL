
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

#include <map>

#include "src/networklayer/icmpv6/ParentTableRPL.h"

namespace rpl {

Define_Module(ParentTableRPL);

std::ostream& operator<<(std::ostream& os, const ParentTableRPL::ParentEntry& entry)
{
    os << "{Rank=" << entry.rank << ", DTSN=" << entry.dtsn << ", Version ID=" << entry.vid << "}";
    return os;
}

ParentTableRPL::ParentTableRPL()
{
    parentTable = new ParentTable();
    // Set parentTable for Version ID 1
    versionParentTable[1] = parentTable;

    maxParents = -1; //Unlimited
}

void ParentTableRPL::initialize()
{
    maxParents = par("maxParents");

    ParentTable& parentTable = *this->parentTable;    // magic to hide the '*' from the name of the watch below
    WATCH_MAP(parentTable);
}

void ParentTableRPL::handleMessage(cMessage *)
{
    throw cRuntimeError("ParentTableRPL::handleMessage: This module doesn't process messages");
}

int ParentTableRPL::getNumberOfParents(unsigned int vid)
{
    return versionParentTable[vid]->size();

}

/*
 * getTableForVid
 * Returns a parent Table for a specified Version ID
 * or nullptr pointer if it is not found
 */

ParentTableRPL::ParentTable *ParentTableRPL::getTableForVid(unsigned int vid)
{
    if (vid == 1)
        return parentTable;

    auto iter = versionParentTable.find(vid);
    if (iter != versionParentTable.end())
        return iter->second;
    return nullptr;
}

/*
 * Register a new parent at parentTable.
 * -1 if not added. 0 if it is new. 1 if it is updated.
 */

bool ParentTableRPL::updateTable(InterfaceEntry *ie, const IPv6Address& id, int rank, unsigned char dtsn, unsigned int vid)
{
    //Enter_Method("ParentTableRPL::updateTableWithAddress()");

    IPv6NeighbourCacheRPL::Neighbour *neighbourEntry = neighbourCache.lookup(IPv6Address, ie->getInterfaceId());

    if (!neighbourEntry){
        throw cRuntimeError("ParentTableRPL::updateTableWithAddress: This IPv6 Address doesn't any entry in Neighbor table!");
    }

    ParentTable::iterator iter;
    ParentTable *table = getTableForVid(vid);

    if (table == nullptr) {
        // Parent Table does not exist for Version ID vid, so we create it
        table = new ParentTable();

        // set 'the parentTable' to Version 1
        if (vid == 1)
            parentTable = table;

        versionParentTable[vid] = table;
        iter = table->end();
    }
    else
        iter = table->find(neighbourEntry);


    if (iter == table->end()) { //So, it is a new entry. Add entry to table.
        if ((maxParents != -1) && (getNumberOfParents(vid) == maxParents)){ //If there is not a room,
            if (willWorstRank(rank, vid)) //If my rank will be the worst rank, don't add me to the table.
                return -1;
            else //If my rank will not be the worst rank, remove the worst rank in the table, and add me instead of it.
                removeWorstParent(vid);
        }
            EV << "Adding entry to Parent Table: " << IPv6Address << " rank: " << rank << " DTSN: " << dtsn << "version: " << vid << "\n";
            (*table)[neighbourEntry] = ParentEntry(idrank, dtsn, vid);
            return 0;
    }
    else {
        // Update existing entry
        ParentEntry& entry = iter->second;
        EV << "Updating entry in Parent Table: "  << IPv6Address << " old rank: " << entry.rank << " old DTSN: " << entry.dtsn << " old version: " << entry.vid << " new rank: " << rank << " new DTSN: " << dtsn << "new version: " << vid << "\n";
        entry.rank = rank;
        entry.dtsn = dtsn;
        entry.vid = vid;
    }
    return 1;

}

void ParentTableRPL::removeWorstParent(unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist
    if (table == nullptr)
        return FALSE;

    auto worst = table->begin();
    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        ParentEntry& entry = iter->second;
        if (entry.rank > worst->second.rank) {
            worst = iter;
        }
    }
    table->erase(worst);
}

bool ParentTableRPL::willWorstRank(int rank, unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist, and table is empty
    if (table == nullptr)
        return FALSE;

    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        ParentEntry& entry = iter->second;
        if (entry.rank > rank) {
            return FALSE;
        }
    }
    return TRUE;
}

/* If the ipAddr has an entry in the parent table, it returns a pointer to the Neighbour.
 * Otherwise, it returns nullptr.
 */
const IPv6NeighbourCacheRPL::Neighbour *ParentTableRPL::getParentNeighborCache(IPv6Address ipAddr, unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist
    if (table == nullptr)
        return nullptr;

    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        if (iter->first->nceKey->address == ipAddr) {
            return iter->first;
        }
    }
    return nullptr;
}

const IPv6NeighbourCacheRPL::Neighbour *ParentTableRPL::getPrefParentNeighborCache(unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist
    if (table == nullptr)
        return nullptr;

    auto bestRank = table->begin();
    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        ParentEntry& entry = iter->second;
        if (entry.rank < bestRank->second.rank) {
            bestRank = iter;
        }
    }
    return bestRank->first;
}

bool ParentTableRPL::isPrefParent(IPv6Address ipAddr, unsigned int vid)
{
    const IPv6NeighbourCacheRPL::Neighbour *neighbor = getPrefParentNeighborCache(vid);
    if ((neighbor) && (neighbor->nceKey->address))
        return true;
    return false;
}

const int ParentTableRPL::getPrefParentDTSN(unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist
    if (table == nullptr)
        throw cRuntimeError("ParentTableRPL::getRank: This Version has not any parent!");

    auto bestRank = table->begin();
    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        ParentEntry& entry = iter->second;
        if (entry.rank < bestRank->second.rank) {
            bestRank = iter;
        }
    }
    return bestRank->second.dtsn;
}

/*
 * Returne the rank for the parent which is specified by ipAddr.
 * Before using this method, we should use the method of
 * getParentNeighborCache() to check if ipAddr is a a parent or not.
 */
const int ParentTableRPL::getParentRank(IPv6Address ipAddr, unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist
    if (table == nullptr)
        throw cRuntimeError("ParentTableRPL::getParentRank: This Version has not any parent!");

    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        if (iter->first->nceKey->address == ipAddr) {
            return iter->second.rank;
        }
    }
    throw cRuntimeError("ParentTableRPL::getParentRank: ipAddr not found!");
}

/*
 * Returne the DODAG rank (the best rank)
 */
const int ParentTableRPL::getRank(unsigned int vid)
{
    ParentTable *table = getTableForVid(vid);
    // Version ID vid does not exist
    if (table == nullptr)
        throw cRuntimeError("ParentTableRPL::getRank: This Version has not any parent!");

    auto bestRank = table->begin();
    for (auto iter = table->begin(); iter != table->end(); iter++ ) {
        ParentEntry& entry = iter->second;
        if (entry.rank < bestRank->second.rank) {
            bestRank = iter;
        }
    }
    return bestRank->second.rank + 1;
}

/*
 * Prints verbose information
 */

void ParentTableRPL::printState()
{
    EV << endl << "Parent Table" << endl;
    EV << "Version    IP Address    Rank    DTSN" << endl;
    for (auto & elem : versionParentTable) {
        ParentTable *table = elem.second;
        for (auto & table_j : *table)
            EV << table_j.second.vid << "   " << table_j.first->nceKey->address << "   " << table_j.second.rank << "   " << table_j.second.dtsn << endl;
    }
}

void ParentTableRPL::clearTable()
{
    for (auto & elem : versionParentTable)
        delete elem.second;

    versionParentTable.clear();
    parentarentTable = nullptr;
}

ParentTableRPL::~ParentTableRPL()
{
    for (auto & elem : versionParentTable)
        delete elem.second;
}


} // namespace rpl

