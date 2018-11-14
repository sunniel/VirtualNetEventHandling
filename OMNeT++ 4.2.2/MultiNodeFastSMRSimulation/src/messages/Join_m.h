//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/Join.msg.
//

#ifndef _JOIN_M_H_
#define _JOIN_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/Join.msg</tt> by opp_msgc.
 * <pre>
 * packet Join {
 *     string senderName;
 *     unsigned long senderId;
 *     simtime_t joinTime;
 *     simtime_t cycleLength;
 * }
 * </pre>
 */
class Join : public ::cPacket
{
  protected:
    opp_string senderName_var;
    unsigned long senderId_var;
    simtime_t joinTime_var;
    simtime_t cycleLength_var;

  private:
    void copy(const Join& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Join&);

  public:
    Join(const char *name=NULL, int kind=0);
    Join(const Join& other);
    virtual ~Join();
    Join& operator=(const Join& other);
    virtual Join *dup() const {return new Join(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getSenderName() const;
    virtual void setSenderName(const char * senderName);
    virtual unsigned long getSenderId() const;
    virtual void setSenderId(unsigned long senderId);
    virtual simtime_t getJoinTime() const;
    virtual void setJoinTime(simtime_t joinTime);
    virtual simtime_t getCycleLength() const;
    virtual void setCycleLength(simtime_t cycleLength);
};

inline void doPacking(cCommBuffer *b, Join& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Join& obj) {obj.parsimUnpack(b);}


#endif // _JOIN_M_H_