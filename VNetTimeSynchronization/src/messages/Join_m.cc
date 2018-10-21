//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/Join.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Join_m.h"

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




Register_Class(Join);

Join::Join(const char *name, int kind) : cPacket(name,kind)
{
    this->senderName_var = 0;
    this->senderId_var = 0;
    this->joinTime_var = 0;
    this->cycleLength_var = 0;
}

Join::Join(const Join& other) : cPacket(other)
{
    copy(other);
}

Join::~Join()
{
}

Join& Join::operator=(const Join& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void Join::copy(const Join& other)
{
    this->senderName_var = other.senderName_var;
    this->senderId_var = other.senderId_var;
    this->joinTime_var = other.joinTime_var;
    this->cycleLength_var = other.cycleLength_var;
}

void Join::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->senderName_var);
    doPacking(b,this->senderId_var);
    doPacking(b,this->joinTime_var);
    doPacking(b,this->cycleLength_var);
}

void Join::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->senderName_var);
    doUnpacking(b,this->senderId_var);
    doUnpacking(b,this->joinTime_var);
    doUnpacking(b,this->cycleLength_var);
}

const char * Join::getSenderName() const
{
    return senderName_var.c_str();
}

void Join::setSenderName(const char * senderName)
{
    this->senderName_var = senderName;
}

unsigned long Join::getSenderId() const
{
    return senderId_var;
}

void Join::setSenderId(unsigned long senderId)
{
    this->senderId_var = senderId;
}

simtime_t Join::getJoinTime() const
{
    return joinTime_var;
}

void Join::setJoinTime(simtime_t joinTime)
{
    this->joinTime_var = joinTime;
}

simtime_t Join::getCycleLength() const
{
    return cycleLength_var;
}

void Join::setCycleLength(simtime_t cycleLength)
{
    this->cycleLength_var = cycleLength;
}

class JoinDescriptor : public cClassDescriptor
{
  public:
    JoinDescriptor();
    virtual ~JoinDescriptor();

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

Register_ClassDescriptor(JoinDescriptor);

JoinDescriptor::JoinDescriptor() : cClassDescriptor("Join", "cPacket")
{
}

JoinDescriptor::~JoinDescriptor()
{
}

bool JoinDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Join *>(obj)!=NULL;
}

const char *JoinDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int JoinDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int JoinDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *JoinDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "senderName",
        "senderId",
        "joinTime",
        "cycleLength",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int JoinDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderName")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+1;
    if (fieldName[0]=='j' && strcmp(fieldName, "joinTime")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "cycleLength")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *JoinDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "unsigned long",
        "simtime_t",
        "simtime_t",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *JoinDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int JoinDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Join *pp = (Join *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string JoinDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Join *pp = (Join *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getSenderName());
        case 1: return ulong2string(pp->getSenderId());
        case 2: return double2string(pp->getJoinTime());
        case 3: return double2string(pp->getCycleLength());
        default: return "";
    }
}

bool JoinDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Join *pp = (Join *)object; (void)pp;
    switch (field) {
        case 0: pp->setSenderName((value)); return true;
        case 1: pp->setSenderId(string2ulong(value)); return true;
        case 2: pp->setJoinTime(string2double(value)); return true;
        case 3: pp->setCycleLength(string2double(value)); return true;
        default: return false;
    }
}

const char *JoinDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *JoinDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Join *pp = (Join *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


