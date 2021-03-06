//
// Generated file, do not edit! Created by nedtool 5.4 from messages/ConsensusBasedEventApply.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CONSENSUSBASEDEVENTAPPLY_M_H
#define __CONSENSUSBASEDEVENTAPPLY_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "EventApply_m.h"
// }}

/**
 * Class generated from <tt>messages/ConsensusBasedEventApply.msg:24</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet ConsensusBasedEventApply extends EventApply
 * {
 *     int eventIndex;
 * }
 * </pre>
 */
class ConsensusBasedEventApply : public ::EventApply
{
  protected:
    int eventIndex;

  private:
    void copy(const ConsensusBasedEventApply& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ConsensusBasedEventApply&);

  public:
    ConsensusBasedEventApply(const char *name=nullptr, short kind=0);
    ConsensusBasedEventApply(const ConsensusBasedEventApply& other);
    virtual ~ConsensusBasedEventApply();
    ConsensusBasedEventApply& operator=(const ConsensusBasedEventApply& other);
    virtual ConsensusBasedEventApply *dup() const override {return new ConsensusBasedEventApply(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getEventIndex() const;
    virtual void setEventIndex(int eventIndex);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ConsensusBasedEventApply& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ConsensusBasedEventApply& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CONSENSUSBASEDEVENTAPPLY_M_H

