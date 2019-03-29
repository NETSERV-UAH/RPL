
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

#ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_PARENTTABLE_H
#define _RPL_SRC_NETWORKLAYER_ICMPV6_PARENTTABLE_H

#include "src/networklayer/icmpv6/IPv6NeighbourDiscoveryRPL.h"
#include "src/networklayer/icmpv6/IPv6NeighbourCacheRPL.h"
#include "inet/networklayer/contract/ipv6/IPv6Address.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/linklayer/common/MACAddress.h"


namespace rpl {
using namespace inet;

/**
 * This module handles the mapping between ports and MAC addresses. See the NED definition for details.
 */
class ParentTableRPL : public cSimpleModule
{
  protected:
    IPv6NeighbourDiscoveryRPL *neighbourDiscoveryRPL;
    int maxParents; //-1 means the unlimited value
    //IPv6Address prefParent;
    //int rank;

    struct ParentEntry
    {
        IPv6NeighbourCacheRPL::Neighbour *neighbourEntry;    // A pointer to a neighbor entry to which the parent entry refers.
        int rank;    // Input port
        int dtsn;    // Arrival time of Lookup Address Table entry
        //unsigned int vid;    // Version ID
        ParentEntry() {}
        ParentEntry(IPv6NeighbourCacheRPL::Neighbour *neighbourEntry, int rank, int dtsn) :
            neighbourEntry(neighbourEntry), rank(rank), dtsn(dtsn) {}
    };

    friend std::ostream& operator<<(std::ostream& os, const ParentEntry& entry);

    typedef std::vector<ParentEntry> ParentTable;

    ParentTable parentTable;


  protected:

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override;

    virtual void handleMessage(cMessage *msg) override;

  public:

    ParentTableRPL();

    ~ParentTableRPL();

  // Table management

protected:

    virtual const ParentEntry *getParentEntry(const IPv6Address &ipAddr) const;

    virtual const ParentEntry *getPrefParentEntry() const;

    virtual bool removeWorstParent();

    virtual bool willWorstRank(int rank) const;

  public:

    virtual int getNumberOfParents() const;

    virtual bool updateTable(InterfaceEntry *ie, const IPv6Address &id, int rank, int dtsn);

    virtual const IPv6NeighbourCacheRPL::Neighbour *getParentNeighborCache(const IPv6Address &ip) const;

    virtual const IPv6NeighbourCacheRPL::Neighbour *getPrefParentNeighborCache() const;

    virtual IPv6Address getPrefParentIPAddress() const;

    virtual bool isPrefParent(const IPv6Address &ipAddr) const;

    virtual int getPrefParentDTSN() const;

    virtual int getParentRank(const IPv6Address &ipAddr) const;

    virtual int getRank() const;

    virtual void printState() const;

    virtual void clearTable();

};

} // namespace rpl

#endif // ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_PARENTTABLE_H

