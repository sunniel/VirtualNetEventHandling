//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/HBTimeout.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "HBTimeout_m.h"

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




Register_Class(HBTimeout);

HBTimeout::HBTimeout(const char *name, int kind) : cPacket(name,kind)
{
    this->hostName_var = 0;
    this->timeout_var = 0;
}

HBTimeout::HBTimeout(const HBTimeout& other) : cPacket(other)
{
    copy(other);
}

HBTimeout::~HBTimeout()
{
}

HBTimeout& HBTimeout::operator=(const HBTimeout& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void HBTimeout::copy(const HBTimeout& other)
{
    this->hostName_var = other.hostName_var;
    this->timeout_var = other.timeout_var;
}

void HBTimeout::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->hostName_var);
    doPacking(b,this->timeout_var);
}

void HBTimeout::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->hostName_var);
    doUnpacking(b,this->timeout_var);
}

const char * HBTimeout::getHostName() const
{
    return hostName_var.c_str();
}

void HBTimeout::setHostName(const char * hostName)
{
    this->hostName_var = hostName;
}

simtime_t HBTimeout::getTimeout() const
{
    return timeout_var;
}

void HBTimeout::setTimeout(simtime_t timeout)
{
    this->timeout_var = timeout;
}

class HBTimeoutDescriptor : public cClassDescriptor
{
  public:
    HBTimeoutDescriptor();
    virtual ~HBTimeoutDescriptor();

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

Register_ClassDescriptor(HBTimeoutDescriptor);

HBTimeoutDescriptor::HBTimeoutDescriptor() : cClassDescriptor("HBTimeout", "cPacket")
{
}

HBTimeoutDescriptor::~HBTimeoutDescriptor()
{
}

bool HBTimeoutDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<HBTimeout *>(obj)!=NULL;
}

const char *HBTimeoutDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int HBTimeoutDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int HBTimeoutDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *HBTimeoutDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "hostName",
        "timeout",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int HBTimeoutDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='h' && strcmp(fieldName, "hostName")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeout")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *HBTimeoutDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "simtime_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *HBTimeoutDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int HBTimeoutDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    HBTimeout *pp = (HBTimeout *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string HBTimeoutDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    HBTimeout *pp = (HBTimeout *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getHostName());
        case 1: return double2string(pp->getTimeout());
        default: return "";
    }
}

bool HBTimeoutDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    HBTimeout *pp = (HBTimeout *)object; (void)pp;
    switch (field) {
        case 0: pp->setHostName((value)); return true;
        case 1: pp->setTimeout(string2double(value)); return true;
        default: return false;
    }
}

const char *HBTimeoutDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *HBTimeoutDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    HBTimeout *pp = (HBTimeout *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


