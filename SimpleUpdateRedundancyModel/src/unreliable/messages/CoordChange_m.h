//
// Generated file, do not edit! Created by opp_msgc 4.2 from unreliable/messages/CoordChange.msg.
//

#ifndef _COORDCHANGE_M_H_
#define _COORDCHANGE_M_H_

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
 * Class generated from <tt>unreliable/messages/CoordChange.msg</tt> by opp_msgc.
 * <pre>
 * packet CoordChange {
 *     string displayString = DISPLAY_STR_MSG_GROUP;
 *     
 *     string coordName;
 * }
 * </pre>
 */
class CoordChange : public ::cPacket
{
  protected:
    opp_string displayString_var;
    opp_string coordName_var;

  private:
    void copy(const CoordChange& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CoordChange&);

  public:
    CoordChange(const char *name=NULL, int kind=0);
    CoordChange(const CoordChange& other);
    virtual ~CoordChange();
    CoordChange& operator=(const CoordChange& other);
    virtual CoordChange *dup() const {return new CoordChange(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getDisplayString() const;
    virtual void setDisplayString(const char * displayString);
    virtual const char * getCoordName() const;
    virtual void setCoordName(const char * coordName);
};

inline void doPacking(cCommBuffer *b, CoordChange& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, CoordChange& obj) {obj.parsimUnpack(b);}


#endif // _COORDCHANGE_M_H_