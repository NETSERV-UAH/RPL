
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
        //IPv6NeighbourCacheRPL::Neighbour *neighbourEntry;    // A pointer to a neighbor entry to which the parent entry refers.
        int rank;    // Input port
        unsigned char dtsn;    // Arrival time of Lookup Address Table entry
        unsigned int vid;    // Version ID
        ParentEntry() {}
        ParentEntry(int rank, unsigned char dtsn, unsigned int vid) :
            rank(rank), dtsn(dtsn), vid(vid) {}
    };

    friend std::ostream& operator<<(std::ostream& os, const ParentEntry& entry);

    struct IP_compare
    {
        bool operator()(const IPv6NeighbourCacheRPL::Neighbour *u1, const IPv6NeighbourCacheRPL::Neighbour *u2) const
        {
            //IPv6NeighbourCacheRPL::Key *key1 = u1.nceKey;
            //IPv6Address ipAddress1 = key1->address;
            IPv6Address ipAddress1 = u1->nceKey->address;
            IPv6Address ipAddress2 = u2->nceKey->address;

            return ipAddress1 < ipAddress2; }
    };

    /* Map or Vector? our search is based on the IPv6 address, so we choose Map, and key is the IPv6 Address.
     * However, we must search the table based on the rank for finding the preferred parent and worst parent.
     */
    typedef std::map<IPv6NeighbourCacheRPL::Neighbour *, ParentEntry, IP_compare> ParentTable;
    typedef std::map<unsigned int, ParentTable *> VersionParentTable;

    ParentTable *parentTable;  // Version-unaware parent lookup (version = 1)
    VersionParentTable versionParentTable;    // Version-aware parent lookup


  protected:

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;

  public:

    ParentTableRPL();
    ~ParentTableRPL();

  // Table management

protected:
    ParentTableRPL::ParentTable *getTableForVid(unsigned int vid);

    bool removeWorstParent(unsigned int vid);

    bool willWorstRank(int rank, unsigned int vid);


  public:

    int getNumberOfParents(unsigned int vid);

    bool updateTable(InterfaceEntry *ie, const IPv6Address& id, int rank, unsigned char dtsn, unsigned int vid);

    const IPv6NeighbourCacheRPL::Neighbour *getParentNeighborCache(IPv6Address ip, unsigned int vid);

    const IPv6NeighbourCacheRPL::Neighbour *getPrefParentNeighborCache(unsigned int vid);

    bool isPrefParent(IPv6Address ipAddr, unsigned int vid);

    const int getPrefParentDTSN(unsigned int vid);

    const int getParentRank(IPv6Address ipAddr, unsigned int vid);

    const int getRank(unsigned int vid);

    void printState();

    void clearTable();

};

} // namespace rpl

#endif // ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_PARENTTABLE_H

