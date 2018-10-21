//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/CCQueryReply.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "CCQueryReply_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(CCQueryReply);

CCQueryReply::CCQueryReply(const char *name, int kind) : cPacket(name,kind)
{
    this->displayString_var = DISPLAY_STR_MSG_GROUP_CC;
    this->senderName_var = 0;
    this->epoch_var = 0;
    this->groupId_var = 0;
    this->groupMembers_var = 0;
    this->Qd_var = 0;
    this->Qp_var = 0;
    this->Qs_var = 0;
    this->escId_var = 0;
    this->esc_var = 0;
}

CCQueryReply::CCQueryReply(const CCQueryReply& other) : cPacket(other)
{
    copy(other);
}

CCQueryReply::~CCQueryReply()
{
}

CCQueryReply& CCQueryReply::operator=(const CCQueryReply& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void CCQueryReply::copy(const CCQueryReply& other)
{
    this->displayString_var = other.displayString_var;
    this->senderName_var = other.senderName_var;
    this->epoch_var = other.epoch_var;
    this->groupId_var = other.groupId_var;
    this->groupMembers_var = other.groupMembers_var;
    this->Qd_var = other.Qd_var;
    this->Qp_var = other.Qp_var;
    this->Qs_var = other.Qs_var;
    this->escId_var = other.escId_var;
    this->esc_var = other.esc_var;
}

void CCQueryReply::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->displayString_var);
    doPacking(b,this->senderName_var);
    doPacking(b,this->epoch_var);
    doPacking(b,this->groupId_var);
    doPacking(b,this->groupMembers_var);
    doPacking(b,this->Qd_var);
    doPacking(b,this->Qp_var);
    doPacking(b,this->Qs_var);
    doPacking(b,this->escId_var);
    doPacking(b,this->esc_var);
}

void CCQueryReply::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->displayString_var);
    doUnpacking(b,this->senderName_var);
    doUnpacking(b,this->epoch_var);
    doUnpacking(b,this->groupId_var);
    doUnpacking(b,this->groupMembers_var);
    doUnpacking(b,this->Qd_var);
    doUnpacking(b,this->Qp_var);
    doUnpacking(b,this->Qs_var);
    doUnpacking(b,this->escId_var);
    doUnpacking(b,this->esc_var);
}

const char * CCQueryReply::getDisplayString() const
{
    return displayString_var.c_str();
}

void CCQueryReply::setDisplayString(const char * displayString)
{
    this->displayString_var = displayString;
}

const char * CCQueryReply::getSenderName() const
{
    return senderName_var.c_str();
}

void CCQueryReply::setSenderName(const char * senderName)
{
    this->senderName_var = senderName;
}

int CCQueryReply::getEpoch() const
{
    return epoch_var;
}

void CCQueryReply::setEpoch(int epoch)
{
    this->epoch_var = epoch;
}

int CCQueryReply::getGroupId() const
{
    return groupId_var;
}

void CCQueryReply::setGroupId(int groupId)
{
    this->groupId_var = groupId;
}

const char * CCQueryReply::getGroupMembers() const
{
    return groupMembers_var.c_str();
}

void CCQueryReply::setGroupMembers(const char * groupMembers)
{
    this->groupMembers_var = groupMembers;
}

const char * CCQueryReply::getQd() const
{
    return Qd_var.c_str();
}

void CCQueryReply::setQd(const char * Qd)
{
    this->Qd_var = Qd;
}

const char * CCQueryReply::getQp() const
{
    return Qp_var.c_str();
}

void CCQueryReply::setQp(const char * Qp)
{
    this->Qp_var = Qp;
}

const char * CCQueryReply::getQs() const
{
    return Qs_var.c_str();
}

void CCQueryReply::setQs(const char * Qs)
{
    this->Qs_var = Qs;
}

int CCQueryReply::getEscId() const
{
    return escId_var;
}

void CCQueryReply::setEscId(int escId)
{
    this->escId_var = escId;
}

long CCQueryReply::getEsc() const
{
    return esc_var;
}

void CCQueryReply::setEsc(long esc)
{
    this->esc_var = esc;
}

class CCQueryReplyDescriptor : public cClassDescriptor
{
  public:
    CCQueryReplyDescriptor();
    virtual ~CCQueryReplyDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(CCQueryReplyDescriptor);

CCQueryReplyDescriptor::CCQueryReplyDescriptor() : cClassDescriptor("CCQueryReply", "cPacket")
{
}

CCQueryReplyDescriptor::~CCQueryReplyDescriptor()
{
}

bool CCQueryReplyDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<CCQueryReply *>(obj)!=NULL;
}

const char *CCQueryReplyDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int CCQueryReplyDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 10+basedesc->getFieldCount(object) : 10;
}

unsigned int CCQueryReplyDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<10) ? fieldTypeFlags[field] : 0;
}

const char *CCQueryReplyDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "displayString",
        "senderName",
        "epoch",
        "groupId",
        "groupMembers",
        "Qd",
        "Qp",
        "Qs",
        "escId",
        "esc",
    };
    return (field>=0 && field<10) ? fieldNames[field] : NULL;
}

int CCQueryReplyDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "displayString")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderName")==0) return base+1;
    if (fieldName[0]=='e' && strcmp(fieldName, "epoch")==0) return base+2;
    if (fieldName[0]=='g' && strcmp(fieldName, "groupId")==0) return base+3;
    if (fieldName[0]=='g' && strcmp(fieldName, "groupMembers")==0) return base+4;
    if (fieldName[0]=='Q' && strcmp(fieldName, "Qd")==0) return base+5;
    if (fieldName[0]=='Q' && strcmp(fieldName, "Qp")==0) return base+6;
    if (fieldName[0]=='Q' && strcmp(fieldName, "Qs")==0) return base+7;
    if (fieldName[0]=='e' && strcmp(fieldName, "escId")==0) return base+8;
    if (fieldName[0]=='e' && strcmp(fieldName, "esc")==0) return base+9;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *CCQueryReplyDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "string",
        "int",
        "int",
        "string",
        "string",
        "string",
        "string",
        "int",
        "long",
    };
    return (field>=0 && field<10) ? fieldTypeStrings[field] : NULL;
}

const char *CCQueryReplyDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int CCQueryReplyDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    CCQueryReply *pp = (CCQueryReply *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string CCQueryReplyDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    CCQueryReply *pp = (CCQueryReply *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDisplayString());
        case 1: return oppstring2string(pp->getSenderName());
        case 2: return long2string(pp->getEpoch());
        case 3: return long2string(pp->getGroupId());
        case 4: return oppstring2string(pp->getGroupMembers());
        case 5: return oppstring2string(pp->getQd());
        case 6: return oppstring2string(pp->getQp());
        case 7: return oppstring2string(pp->getQs());
        case 8: return long2string(pp->getEscId());
        case 9: return long2string(pp->getEsc());
        default: return "";
    }
}

bool CCQueryReplyDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    CCQueryReply *pp = (CCQueryReply *)object; (void)pp;
    switch (field) {
        case 0: pp->setDisplayString((value)); return true;
        case 1: pp->setSenderName((value)); return true;
        case 2: pp->setEpoch(string2long(value)); return true;
        case 3: pp->setGroupId(string2long(value)); return true;
        case 4: pp->setGroupMembers((value)); return true;
        case 5: pp->setQd((value)); return true;
        case 6: pp->setQp((value)); return true;
        case 7: pp->setQs((value)); return true;
        case 8: pp->setEscId(string2long(value)); return true;
        case 9: pp->setEsc(string2long(value)); return true;
        default: return false;
    }
}

const char *CCQueryReplyDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<10) ? fieldStructNames[field] : NULL;
}

void *CCQueryReplyDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    CCQueryReply *pp = (CCQueryReply *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


