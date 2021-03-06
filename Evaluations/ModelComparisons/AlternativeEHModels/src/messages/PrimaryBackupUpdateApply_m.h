//
// Generated file, do not edit! Created by nedtool 5.4 from messages/PrimaryBackupUpdateApply.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __PRIMARYBACKUPUPDATEAPPLY_M_H
#define __PRIMARYBACKUPUPDATEAPPLY_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "UpdateApply_m.h"
// }}

/**
 * Class generated from <tt>messages/PrimaryBackupUpdateApply.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet PrimaryBackupUpdateApply extends UpdateApply
 * {
 *     int stateIndex;
 * }
 * </pre>
 */
class PrimaryBackupUpdateApply : public ::UpdateApply
{
  protected:
    int stateIndex;

  private:
    void copy(const PrimaryBackupUpdateApply& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PrimaryBackupUpdateApply&);

  public:
    PrimaryBackupUpdateApply(const char *name=nullptr, short kind=0);
    PrimaryBackupUpdateApply(const PrimaryBackupUpdateApply& other);
    virtual ~PrimaryBackupUpdateApply();
    PrimaryBackupUpdateApply& operator=(const PrimaryBackupUpdateApply& other);
    virtual PrimaryBackupUpdateApply *dup() const override {return new PrimaryBackupUpdateApply(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getStateIndex() const;
    virtual void setStateIndex(int stateIndex);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const PrimaryBackupUpdateApply& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, PrimaryBackupUpdateApply& obj) {obj.parsimUnpack(b);}


#endif // ifndef __PRIMARYBACKUPUPDATEAPPLY_M_H

