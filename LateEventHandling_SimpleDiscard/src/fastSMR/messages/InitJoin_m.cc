//
// Generated file, do not edit! Created by opp_msgc 4.2 from fastSMR/messages/InitJoin.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "InitJoin_m.h"

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




Register_Class(InitJoin);

InitJoin::InitJoin(const char *name, int kind) : cPacket(name,kind)
{
    this->senderId_var = 0;
    this->seq_var = 0;
    this->sourceName_var = 0;
    this->destName_var = 0;
    this->joinTime_var = 0;
}

InitJoin::InitJoin(const InitJoin& other) : cPacket(other)
{
    copy(other);
}

InitJoin::~InitJoin()
{
}

InitJoin& InitJoin::operator=(const InitJoin& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void InitJoin::copy(const InitJoin& other)
{
    this->senderId_var = other.senderId_var;
    this->seq_var = other.seq_var;
    this->sourceName_var = other.sourceName_var;
    this->destName_var = other.destName_var;
    this->joinTime_var = other.joinTime_var;
}

void InitJoin::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->senderId_var);
    doPacking(b,this->seq_var);
    doPacking(b,this->sourceName_var);
    doPacking(b,this->destName_var);
    doPacking(b,this->joinTime_var);
}

void InitJoin::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->senderId_var);
    doUnpacking(b,this->seq_var);
    doUnpacking(b,this->sourceName_var);
    doUnpacking(b,this->destName_var);
    doUnpacking(b,this->joinTime_var);
}

unsigned long InitJoin::getSenderId() const
{
    return senderId_var;
}

void InitJoin::setSenderId(unsigned long senderId)
{
    this->senderId_var = senderId;
}

int InitJoin::getSeq() const
{
    return seq_var;
}

void InitJoin::setSeq(int seq)
{
    this->seq_var = seq;
}

const char * InitJoin::getSourceName() const
{
    return sourceName_var.c_str();
}

void InitJoin::setSourceName(const char * sourceName)
{
    this->sourceName_var = sourceName;
}

const char * InitJoin::getDestName() const
{
    return destName_var.c_str();
}

void InitJoin::setDestName(const char * destName)
{
    this->destName_var = destName;
}

simtime_t InitJoin::getJoinTime() const
{
    return joinTime_var;
}

void InitJoin::setJoinTime(simtime_t joinTime)
{
    this->joinTime_var = joinTime;
}

class InitJoinDescriptor : public cClassDescriptor
{
  public:
    InitJoinDescriptor();
    virtual ~InitJoinDescriptor();

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

Register_ClassDescriptor(InitJoinDescriptor);

InitJoinDescriptor::InitJoinDescriptor() : cClassDescriptor("InitJoin", "cPacket")
{
}

InitJoinDescriptor::~InitJoinDescriptor()
{
}

bool InitJoinDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<InitJoin *>(obj)!=NULL;
}

const char *InitJoinDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int InitJoinDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int InitJoinDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *InitJoinDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "senderId",
        "seq",
        "sourceName",
        "destName",
        "joinTime",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int InitJoinDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "seq")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceName")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "destName")==0) return base+3;
    if (fieldName[0]=='j' && strcmp(fieldName, "joinTime")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *InitJoinDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "unsigned long",
        "int",
        "string",
        "string",
        "simtime_t",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *InitJoinDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int InitJoinDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    InitJoin *pp = (InitJoin *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string InitJoinDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    InitJoin *pp = (InitJoin *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getSenderId());
        case 1: return long2string(pp->getSeq());
        case 2: return oppstring2string(pp->getSourceName());
        case 3: return oppstring2string(pp->getDestName());
        case 4: return double2string(pp->getJoinTime());
        default: return "";
    }
}

bool InitJoinDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    InitJoin *pp = (InitJoin *)object; (void)pp;
    switch (field) {
        case 0: pp->setSenderId(string2ulong(value)); return true;
        case 1: pp->setSeq(string2long(value)); return true;
        case 2: pp->setSourceName((value)); return true;
        case 3: pp->setDestName((value)); return true;
        case 4: pp->setJoinTime(string2double(value)); return true;
        default: return false;
    }
}

const char *InitJoinDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<5) ? fieldStructNames[field] : NULL;
}

void *InitJoinDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    InitJoin *pp = (InitJoin *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


