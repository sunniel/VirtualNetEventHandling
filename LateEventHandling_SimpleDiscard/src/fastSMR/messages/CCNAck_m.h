//
// Generated file, do not edit! Created by opp_msgc 4.2 from fastSMR/messages/CCNAck.msg.
//

#ifndef _CCNACK_M_H_
#define _CCNACK_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "Constants.h"
// }}



/**
 * Class generated from <tt>fastSMR/messages/CCNAck.msg</tt> by opp_msgc.
 * <pre>
 * packet CCNAck {
 *     string displayString = DISPLAY_STR_MSG_GROUP_CC;
 *     
 *     string senderName;
 * }
 * </pre>
 */
class CCNAck : public ::cPacket
{
  protected:
    opp_string displayString_var;
    opp_string senderName_var;

  private:
    void copy(const CCNAck& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CCNAck&);

  public:
    CCNAck(const char *name=NULL, int kind=0);
    CCNAck(const CCNAck& other);
    virtual ~CCNAck();
    CCNAck& operator=(const CCNAck& other);
    virtual CCNAck *dup() const {return new CCNAck(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getDisplayString() const;
    virtual void setDisplayString(const char * displayString);
    virtual const char * getSenderName() const;
    virtual void setSenderName(const char * senderName);
};

inline void doPacking(cCommBuffer *b, CCNAck& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, CCNAck& obj) {obj.parsimUnpack(b);}


#endif // _CCNACK_M_H_
