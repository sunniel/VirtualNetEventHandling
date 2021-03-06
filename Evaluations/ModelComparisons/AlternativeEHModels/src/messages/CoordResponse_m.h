//
// Generated file, do not edit! Created by nedtool 5.4 from messages/CoordResponse.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __COORDRESPONSE_M_H
#define __COORDRESPONSE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/CoordResponse.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet CoordResponse
 * {
 *     string coordFullName;
 * }
 * </pre>
 */
class CoordResponse : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string coordFullName;

  private:
    void copy(const CoordResponse& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CoordResponse&);

  public:
    CoordResponse(const char *name=nullptr, short kind=0);
    CoordResponse(const CoordResponse& other);
    virtual ~CoordResponse();
    CoordResponse& operator=(const CoordResponse& other);
    virtual CoordResponse *dup() const override {return new CoordResponse(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getCoordFullName() const;
    virtual void setCoordFullName(const char * coordFullName);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const CoordResponse& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, CoordResponse& obj) {obj.parsimUnpack(b);}


#endif // ifndef __COORDRESPONSE_M_H

