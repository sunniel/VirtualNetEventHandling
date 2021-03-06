//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/EventApply.msg.
//

#ifndef _EVENTAPPLY_M_H_
#define _EVENTAPPLY_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/EventApply.msg</tt> by opp_msgc.
 * <pre>
 * packet EventApply {
 *     int index;
 *     string event;
 * }
 * </pre>
 */
class EventApply : public ::cPacket
{
  protected:
    int index_var;
    opp_string event_var;

  private:
    void copy(const EventApply& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const EventApply&);

  public:
    EventApply(const char *name=NULL, int kind=0);
    EventApply(const EventApply& other);
    virtual ~EventApply();
    EventApply& operator=(const EventApply& other);
    virtual EventApply *dup() const {return new EventApply(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getIndex() const;
    virtual void setIndex(int index);
    virtual const char * getEvent() const;
    virtual void setEvent(const char * event);
};

inline void doPacking(cCommBuffer *b, EventApply& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, EventApply& obj) {obj.parsimUnpack(b);}


#endif // _EVENTAPPLY_M_H_
