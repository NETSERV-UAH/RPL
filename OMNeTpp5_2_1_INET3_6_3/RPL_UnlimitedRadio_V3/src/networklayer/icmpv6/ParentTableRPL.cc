// Copyright (C) 2013 OpenSim Ltd.
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

//#define MAX_LINE    100

Define_Module(ParentTableRPL);

std::ostream& operator<<(std::ostream& os, const ParentTable::ParentEntry& entry)
{
    os << "{Rank=" << entry.rank << ", DTSN=" << entry.dtsn << "}";
    return os;
}

ParentTableRPL::ParentTableRPL()
{
    parentTable = new ParentTable();
}

void ParentTableRPL::initialize()
{

    ParentTable& parentTable = *this->ParentTable;    // magic to hide the '*' from the name of the watch below
    WATCH_MAP(parentTable);
}

void ParentTableRPL::handleMessage(cMessage *)
{
    throw cRuntimeError("ParentTableRPL::handleMessage: This module doesn't process messages");
}

int ParentTableRPL::IsParent(InterfaceEntry *ie, const IPv6Address& id,int idrank)
{
    IPv6NeighbourCacheRPL::Neighbour *neighbourEntry = neighbourCache.lookup(IPv6Address, ie->getInterfaceId());

    if (neighbourEntry){
        if (parentTable == nullptr)
            return NOT_EXIST;


        auto iter = parentTable->find(neighbourEntry);

        if (iter == table->end()) {
            // not found
            return NOT_EXIST;
        }

        if (iter->second.rank == idrank)
            return(EXIST);
        else
          return(SHOULD_BE_UPDATED);
    }

    return(NOT_EXIST);
}


void RPLRouting::AddParent(const IPv6Address& id, int idrank, unsigned char dtsn)
{
    if(!hasRoute[VersionNember]){
        hasRoute[VersionNember] = true;
        NodeStateLast->numPreferedParents_Upward++; // next condition is not a good place for this statement because when deleteParent() increaments NofParents[VersionNember], the condition is true, and numPreferedParents increaments twice or more
    }
    if(NofParents[VersionNember]==0)
    {
        Parents[VersionNember][0].ParentId=id;
        Parents[VersionNember][0].ParentRank=idrank;
        Parents[VersionNember][0].dtsn = dtsn;
        PrParent=Parents[VersionNember][0].ParentId;
        Rank=Parents[VersionNember][0].ParentRank+1;
        NofParents[VersionNember]++;
        NodeStateLast->numParents_Upward++;
        return;
    }else
    {
        if (NofParents[VersionNember]==MaxNofParents)
        {
           if (idrank >= Parents[VersionNember][NofParents[VersionNember]-1].ParentRank) return;
           else{
               NofParents[VersionNember]--;
               NodeStateLast->numParents_Upward--;
           }
        }
        int i=NofParents[VersionNember]-1;
        while((i>=0)&&(idrank<Parents[VersionNember][i].ParentRank))
        {
            Parents[VersionNember][i+1]=Parents[VersionNember][i];
            i--;
        }
        Parents[VersionNember][i+1].ParentId=id;
        Parents[VersionNember][i+1].ParentRank=idrank;
        Parents[VersionNember][i+1].dtsn = dtsn;
        PrParent=Parents[VersionNember][0].ParentId;
        Rank=Parents[VersionNember][0].ParentRank+1;
        NofParents[VersionNember]++;
        NodeStateLast->numParents_Upward++;
    }
    return;
}







/*
 * Register a new MAC address at addressTable.
 * True if refreshed. False if it is new.
 */

bool ParentTableRPL::updateTableWithAddress(int portno, MACAddress& address, unsigned int vid)
{
    Enter_Method("MACAddressTable::updateTableWithAddress()");
    if (address.isBroadcast())
        return false;

    AddressTable::iterator iter;
    AddressTable *table = getTableForVid(vid);

    if (table == nullptr) {
        // MAC Address Table does not exist for VLAN ID vid, so we create it
        table = new AddressTable();

        // set 'the addressTable' to VLAN ID 0
        if (vid == 0)
            addressTable = table;

        vlanAddressTable[vid] = table;
        iter = table->end();
    }
    else
        iter = table->find(address);

    if (iter == table->end()) {
        removeAgedEntriesIfNeeded();

        // Add entry to table
        EV << "Adding entry to Address Table: " << address << " --> port" << portno << "\n";
        (*table)[address] = AddressEntry(vid, portno, simTime());
        return false;
    }
    else {
        // Update existing entry
        EV << "Updating entry in Address Table: " << address << " --> port" << portno << "\n";
        AddressEntry& entry = iter->second;
        entry.insertionTime = simTime();
        entry.portno = portno;
    }
    return true;
}

/*
 * Clears portno MAC cache.
 */

void ParentTableRPL::flush(int portno)
{
    Enter_Method("MACAddressTable::flush():  Clearing gate %d cache", portno);
    for (auto & elem : vlanAddressTable) {
        AddressTable *table = elem.second;
        for (auto j = table->begin(); j != table->end(); ) {
            auto cur = j++;
            if (cur->second.portno == portno)
                table->erase(cur);
        }
    }
}

/*
 * Prints verbose information
 */

void ParentTableRPL::printState()
{
    EV << endl << "MAC Address Table" << endl;
    EV << "VLAN ID    MAC    Port    Inserted" << endl;
    for (auto & elem : vlanAddressTable) {
        AddressTable *table = elem.second;
        for (auto & table_j : *table)
            EV << table_j.second.vid << "   " << table_j.first << "   " << table_j.second.portno << "   " << table_j.second.insertionTime << endl;
    }
}

void ParentTableRPL::clearTable()
{
    for (auto & elem : vlanAddressTable)
        delete elem.second;

    vlanAddressTable.clear();
    addressTable = nullptr;
}

ParentTableRPL::~MACAddressTable()
{
    for (auto & elem : vlanAddressTable)
        delete elem.second;
}


} // namespace rpl

