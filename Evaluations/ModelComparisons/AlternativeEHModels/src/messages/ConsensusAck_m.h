//
// Generated file, do not edit! Created by nedtool 5.4 from messages/ConsensusAck.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CONSENSUSACK_M_H
#define __CONSENSUSACK_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "../common/Constants.h"
// }}

/**
 * Class generated from <tt>messages/ConsensusAck.msg:20</tt> by nedtool.
 * <pre>
 * packet ConsensusAck
 * {
 *     string displayString = DISPLAY_STR_MSG_GROUP_EH;
 * 
 *     string senderName;
 * }
 * </pre>
 */
class ConsensusAck : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string displayString;
    ::omnetpp::opp_string senderName;

  private:
    void copy(const ConsensusAck& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ConsensusAck&);

  public:
    ConsensusAck(const char *name=nullptr, short kind=0);
    ConsensusAck(const ConsensusAck& other);
    virtual ~ConsensusAck();
    ConsensusAck& operator=(const ConsensusAck& other);
    virtual ConsensusAck *dup() const override {return new ConsensusAck(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getDisplayString() const;
    virtual void setDisplayString(const char * displayString);
    virtual const char * getSenderName() const;
    virtual void setSenderName(const char * senderName);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ConsensusAck& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ConsensusAck& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CONSENSUSACK_M_H

