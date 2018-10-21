//
// Generated file, do not edit! Created by opp_msgc 4.2 from basicSMR/messages/CCEnd.msg.
//

#ifndef _CCEND_M_H_
#define _CCEND_M_H_

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
 * Class generated from <tt>basicSMR/messages/CCEnd.msg</tt> by opp_msgc.
 * <pre>
 * packet CCEnd {
 * 	string displayString = DISPLAY_STR_MSG_GROUP_CC;
 *     
 *     string senderName;
 *     int epoch;
 *     int groupId;
 *     string groupMembers;
 *     string state;
 *     int escId;
 *     long esc;    
 * }
 * </pre>
 */
class CCEnd : public ::cPacket
{
  protected:
    opp_string displayString_var;
    opp_string senderName_var;
    int epoch_var;
    int groupId_var;
    opp_string groupMembers_var;
    opp_string state_var;
    int escId_var;
    long esc_var;

  private:
    void copy(const CCEnd& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CCEnd&);

  public:
    CCEnd(const char *name=NULL, int kind=0);
    CCEnd(const CCEnd& other);
    virtual ~CCEnd();
    CCEnd& operator=(const CCEnd& other);
    virtual CCEnd *dup() const {return new CCEnd(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getDisplayString() const;
    virtual void setDisplayString(const char * displayString);
    virtual const char * getSenderName() const;
    virtual void setSenderName(const char * senderName);
    virtual int getEpoch() const;
    virtual void setEpoch(int epoch);
    virtual int getGroupId() const;
    virtual void setGroupId(int groupId);
    virtual const char * getGroupMembers() const;
    virtual void setGroupMembers(const char * groupMembers);
    virtual const char * getState() const;
    virtual void setState(const char * state);
    virtual int getEscId() const;
    virtual void setEscId(int escId);
    virtual long getEsc() const;
    virtual void setEsc(long esc);
};

inline void doPacking(cCommBuffer *b, CCEnd& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, CCEnd& obj) {obj.parsimUnpack(b);}


#endif // _CCEND_M_H_
