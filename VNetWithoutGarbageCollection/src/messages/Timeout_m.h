//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/Timeout.msg.
//

#ifndef _TIMEOUT_M_H_
#define _TIMEOUT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/Timeout.msg</tt> by opp_msgc.
 * <pre>
 * packet Timeout {
 *     string hostName;
 *     simtime_t timeout;
 *     
 *     
 *     int seq;
 *     string event;
 * }
 * </pre>
 */
class Timeout : public ::cPacket
{
  protected:
    opp_string hostName_var;
    simtime_t timeout_var;
    int seq_var;
    opp_string event_var;

  private:
    void copy(const Timeout& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Timeout&);

  public:
    Timeout(const char *name=NULL, int kind=0);
    Timeout(const Timeout& other);
    virtual ~Timeout();
    Timeout& operator=(const Timeout& other);
    virtual Timeout *dup() const {return new Timeout(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getHostName() const;
    virtual void setHostName(const char * hostName);
    virtual simtime_t getTimeout() const;
    virtual void setTimeout(simtime_t timeout);
    virtual int getSeq() const;
    virtual void setSeq(int seq);
    virtual const char * getEvent() const;
    virtual void setEvent(const char * event);
};

inline void doPacking(cCommBuffer *b, Timeout& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Timeout& obj) {obj.parsimUnpack(b);}


#endif // _TIMEOUT_M_H_
