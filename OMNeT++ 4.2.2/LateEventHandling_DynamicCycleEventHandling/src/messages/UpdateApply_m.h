//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/UpdateApply.msg.
//

#ifndef _UPDATEAPPLY_M_H_
#define _UPDATEAPPLY_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/UpdateApply.msg</tt> by opp_msgc.
 * <pre>
 * packet UpdateApply {
 *     string content;
 * }
 * </pre>
 */
class UpdateApply : public ::cPacket
{
  protected:
    opp_string content_var;

  private:
    void copy(const UpdateApply& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const UpdateApply&);

  public:
    UpdateApply(const char *name=NULL, int kind=0);
    UpdateApply(const UpdateApply& other);
    virtual ~UpdateApply();
    UpdateApply& operator=(const UpdateApply& other);
    virtual UpdateApply *dup() const {return new UpdateApply(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getContent() const;
    virtual void setContent(const char * content);
};

inline void doPacking(cCommBuffer *b, UpdateApply& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, UpdateApply& obj) {obj.parsimUnpack(b);}


#endif // _UPDATEAPPLY_M_H_
