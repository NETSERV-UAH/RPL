//
// Generated file, do not edit! Created by nedtool 5.2 from src/networklayer/icmpv6/ICMPv6MessageRPL.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __INET_ICMPV6MESSAGERPL_M_H
#define __INET_ICMPV6MESSAGERPL_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0502
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// cplusplus {{
#include "inet/common/INETDefs.h"
#include "inet/networklayer/icmpv6/ICMPv6Message_m.h"
#include "inet/networklayer/contract/ipv6/IPv6Address.h"
#define ICMPv6_HEADER_BYTES  8
// }}


namespace inet {

/**
 * Enum generated from <tt>src/networklayer/icmpv6/ICMPv6MessageRPL.msg:36</tt> by nedtool.
 * <pre>
 * enum ICMPv6TypeRPL
 * {
 * 
 *     ICMPv6_RPL_CONTROL_MESSAGE = 155; //RFC 6550, section 6, page 30
 * }
 * </pre>
 */
enum ICMPv6TypeRPL {
    ICMPv6_RPL_CONTROL_MESSAGE = 155
};

/**
 * Enum generated from <tt>src/networklayer/icmpv6/ICMPv6MessageRPL.msg:45</tt> by nedtool.
 * <pre>
 * //EXTRA
 * //values of code field
 * enum ICMPv6_RPL_CONTROL_MSG
 * {
 * 
 *     DIS = 0x00;
 *     DIO = 0x01;
 * }
 * </pre>
 */
enum ICMPv6_RPL_CONTROL_MSG {
    DIS = 0x00,
    DIO = 0x01
};

/**
 * Enum generated from <tt>src/networklayer/icmpv6/ICMPv6MessageRPL.msg:53</tt> by nedtool.
 * <pre>
 * //EXTRA
 * //DIS options
 * enum RPL_DIS_OPTIONS
 * {
 * 
 *     PAD1 = 0x00;
 *     PADN = 0x01;
 *     Solicited_Information = 0x07;
 * }
 * </pre>
 */
enum RPL_DIS_OPTIONS {
    PAD1 = 0x00,
    PADN = 0x01,
    Solicited_Information = 0x07
};

/**
 * Class generated from <tt>src/networklayer/icmpv6/ICMPv6MessageRPL.msg:63</tt> by nedtool.
 * <pre>
 * //EXTRA 
 * //DIS control message for RPL
 * //message ICMPv6DISMsg extends ICMPv6Message
 * packet ICMPv6DISMsg extends ICMPv6Message
 * {
 *     int code \@enum(ICMPv6_RPL_CONTROL_MSG); // RFC 6550, section 6: set to 0x00
 *     // TODO: checksum 
 *     int flags; // RFC 6550, section 6.2.1: set to 0
 *     int reserved; // RFC 6550, section 6.2.1 set to 0
 * 
 *     int RPLInstanceID;          // The ID of the RPL instance
 *     int VersionNumber;          // DODAG version number
 *     int V;                      // Node's rank
 *     int I;                      // Type of the DODAG, Grounded or Flooding
 *     int D;                      // Destination Advertisement Trigger Sequence Number       
 *     int Flag;                   // The size of Imin in Trcikle algorithm
 *     IPv6Address DODAGID;   // IPv6 address set by DODAG root  
 * 
 *     int options \@enum(RPL_DIS_OPTIONS); //RFC 6550, section 6.2.1
 * }
 * </pre>
 */
class ICMPv6DISMsg : public ::inet::ICMPv6Message
{
  protected:
    int code;
    int flags;
    int reserved;
    int RPLInstanceID;
    int VersionNumber;
    int V;
    int I;
    int D;
    int Flag;
    IPv6Address DODAGID;
    int options;

  private:
    void copy(const ICMPv6DISMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ICMPv6DISMsg&);

  public:
    ICMPv6DISMsg(const char *name=nullptr, short kind=0);
    ICMPv6DISMsg(const ICMPv6DISMsg& other);
    virtual ~ICMPv6DISMsg();
    ICMPv6DISMsg& operator=(const ICMPv6DISMsg& other);
    virtual ICMPv6DISMsg *dup() const override {return new ICMPv6DISMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getCode() const;
    virtual void setCode(int code);
    virtual int getFlags() const;
    virtual void setFlags(int flags);
    virtual int getReserved() const;
    virtual void setReserved(int reserved);
    virtual int getRPLInstanceID() const;
    virtual void setRPLInstanceID(int RPLInstanceID);
    virtual int getVersionNumber() const;
    virtual void setVersionNumber(int VersionNumber);
    virtual int getV() const;
    virtual void setV(int V);
    virtual int getI() const;
    virtual void setI(int I);
    virtual int getD() const;
    virtual void setD(int D);
    virtual int getFlag() const;
    virtual void setFlag(int Flag);
    virtual IPv6Address& getDODAGID();
    virtual const IPv6Address& getDODAGID() const {return const_cast<ICMPv6DISMsg*>(this)->getDODAGID();}
    virtual void setDODAGID(const IPv6Address& DODAGID);
    virtual int getOptions() const;
    virtual void setOptions(int options);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ICMPv6DISMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ICMPv6DISMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>src/networklayer/icmpv6/ICMPv6MessageRPL.msg:84</tt> by nedtool.
 * <pre>
 * //EXTRA 
 * //DIO control message for RPL
 * //message ICMPv6DIOMsg extends ICMPv6Message
 * packet ICMPv6DIOMsg extends ICMPv6Message
 * {
 *     int code \@enum(ICMPv6_RPL_CONTROL_MSG); // RFC 6550, section 6: set to 0x01
 *     // TODO: checksum 
 *     //int rplInstanceId = 0; // RFC 6550, section 6.3.1
 *     int versionNumber = 0; // RFC 6550, section 6.3.1
 *     int rank = 0;
 *     int grounded;
 *     //0
 *     //MOP
 *     //Prf
 *     //int flags = 0; // RFC 6550, section 6.3.1
 *     //int reserved = 0; // RFC 6550, section 6.3.1
 *     int DTSN;                 // Destination Advertisement Trigger Sequence Number       
 *     double IMin;              // The size of Imin in Trcikle algorithm
 *     int NofDoub;              // Number of doubling in Trcikle algorithm
 *     int k;                    // Redundancy constant in Trcikle algorithm
 *     IPv6Address DODAGID;              // IPv6 address set by DODAG root 
 * 
 * 
 * 
 *     int options \@enum(RPL_DIS_OPTIONS); //RFC 6550, section 6.3.1
 * }
 * </pre>
 */
class ICMPv6DIOMsg : public ::inet::ICMPv6Message
{
  protected:
    int code;
    int versionNumber;
    int rank;
    int grounded;
    int DTSN;
    double IMin;
    int NofDoub;
    int k;
    IPv6Address DODAGID;
    int options;

  private:
    void copy(const ICMPv6DIOMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ICMPv6DIOMsg&);

  public:
    ICMPv6DIOMsg(const char *name=nullptr, short kind=0);
    ICMPv6DIOMsg(const ICMPv6DIOMsg& other);
    virtual ~ICMPv6DIOMsg();
    ICMPv6DIOMsg& operator=(const ICMPv6DIOMsg& other);
    virtual ICMPv6DIOMsg *dup() const override {return new ICMPv6DIOMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getCode() const;
    virtual void setCode(int code);
    virtual int getVersionNumber() const;
    virtual void setVersionNumber(int versionNumber);
    virtual int getRank() const;
    virtual void setRank(int rank);
    virtual int getGrounded() const;
    virtual void setGrounded(int grounded);
    virtual int getDTSN() const;
    virtual void setDTSN(int DTSN);
    virtual double getIMin() const;
    virtual void setIMin(double IMin);
    virtual int getNofDoub() const;
    virtual void setNofDoub(int NofDoub);
    virtual int getK() const;
    virtual void setK(int k);
    virtual IPv6Address& getDODAGID();
    virtual const IPv6Address& getDODAGID() const {return const_cast<ICMPv6DIOMsg*>(this)->getDODAGID();}
    virtual void setDODAGID(const IPv6Address& DODAGID);
    virtual int getOptions() const;
    virtual void setOptions(int options);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ICMPv6DIOMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ICMPv6DIOMsg& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_ICMPV6MESSAGERPL_M_H

