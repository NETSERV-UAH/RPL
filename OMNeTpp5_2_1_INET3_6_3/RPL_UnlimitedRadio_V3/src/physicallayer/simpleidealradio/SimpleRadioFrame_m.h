//
// Generated file, do not edit! Created by nedtool 5.2 from src/physicallayer/simpleidealradio/SimpleRadioFrame.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __RPL_SIMPLERADIOFRAME_M_H
#define __RPL_SIMPLERADIOFRAME_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0502
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// cplusplus {{
    #include "inet/common/geometry/common/Coord.h"
// }}


namespace rpl {

/**
 * Class generated from <tt>src/physicallayer/simpleidealradio/SimpleRadioFrame.msg:40</tt> by nedtool.
 * <pre>
 * packet SimpleRadioFrame
 * {
 *     double communicationRange;
 *     ::inet::Coord startPosition;
 * }
 * </pre>
 */
class SimpleRadioFrame : public ::omnetpp::cPacket
{
  protected:
    double communicationRange;
    ::inet::Coord startPosition;

  private:
    void copy(const SimpleRadioFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SimpleRadioFrame&);

  public:
    SimpleRadioFrame(const char *name=nullptr, short kind=0);
    SimpleRadioFrame(const SimpleRadioFrame& other);
    virtual ~SimpleRadioFrame();
    SimpleRadioFrame& operator=(const SimpleRadioFrame& other);
    virtual SimpleRadioFrame *dup() const override {return new SimpleRadioFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual double getCommunicationRange() const;
    virtual void setCommunicationRange(double communicationRange);
    virtual ::inet::Coord& getStartPosition();
    virtual const ::inet::Coord& getStartPosition() const {return const_cast<SimpleRadioFrame*>(this)->getStartPosition();}
    virtual void setStartPosition(const ::inet::Coord& startPosition);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const SimpleRadioFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, SimpleRadioFrame& obj) {obj.parsimUnpack(b);}

} // namespace rpl

#endif // ifndef __RPL_SIMPLERADIOFRAME_M_H

