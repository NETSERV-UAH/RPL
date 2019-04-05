
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

#ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_SOURCEROUTINGTABLE_H
#define _RPL_SRC_NETWORKLAYER_ICMPV6_SOURCEROUTINGTABLE_H

#include "inet/networklayer/contract/ipv6/IPv6Address.h"
//#include "inet/networklayer/common/InterfaceEntry.h"

namespace rpl {
using namespace inet;

class RPLUpwardRouting;

class SourceRoutingTable : public cSimpleModule
{
  protected:
    RPLUpwardRouting *pRPLUpwardRouting;

    struct SREntry
    {
        IPv6Address prefix;
        int prefixLen;
        IPv6Address daoParent;
        int interfaceID;
        simtime_t expr;
        SREntry() {}
        SREntry(IPv6Address prefix, int prefixLen, IPv6Address daoParent, int interfaceID, simtime_t exp) :
            prefix(prefix), prefixLen(prefixLen), daoParent(daoParent), interfaceID(interfaceID), expr(expr) {}
    };

    friend std::ostream& operator<<(std::ostream& os, const SREntry& entry);

    typedef std::vector<SREntry> SRTable;

    SRTable srTable;


  protected:

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override;

    virtual void handleMessage(cMessage *msg) override;

  public:

    SourceRoutingTable();

    ~SourceRoutingTable();

  // Table management

  public:

    virtual int getNumberOfRoutes() const;

    virtual bool isRoute(const IPv6Address &prefix, int prefixLen) const;

    virtual void addRoute(const IPv6Address &prefix, int prefixLen, const IPv6Address &daoParent, int interfaceID, simtime_t exp);

    virtual void deleteRoute(const IPv6Address &prefix, int prefixLen);

    virtual void deleteRoute(const IPv6Address &prefix, int prefixLen, const IPv6Address &daoParent);

    virtual void printState() const;

    virtual void clearTable();

};

} // namespace rpl

#endif // ifndef _RPL_SRC_NETWORKLAYER_ICMPV6_SOURCEROUTINGTABLE_H

