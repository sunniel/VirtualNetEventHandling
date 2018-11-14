//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/Timeout.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Timeout_m.h"

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




Register_Class(Timeout);

Timeout::Timeout(const char *name, int kind) : cPacket(name,kind)
{
    this->hostName_var = 0;
    this->timeout_var = 0;
    this->seq_var = 0;
    this->event_var = 0;
}

Timeout::Timeout(const Timeout& other) : cPacket(other)
{
    copy(other);
}

Timeout::~Timeout()
{
}

Timeout& Timeout::operator=(const Timeout& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void Timeout::copy(const Timeout& other)
{
    this->hostName_var = other.hostName_var;
    this->timeout_var = other.timeout_var;
    this->seq_var = other.seq_var;
    this->event_var = other.event_var;
}

void Timeout::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->hostName_var);
    doPacking(b,this->timeout_var);
    doPacking(b,this->seq_var);
    doPacking(b,this->event_var);
}

void Timeout::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->hostName_var);
    doUnpacking(b,this->timeout_var);
    doUnpacking(b,this->seq_var);
    doUnpacking(b,this->event_var);
}

const char * Timeout::getHostName() const
{
    return hostName_var.c_str();
}

void Timeout::setHostName(const char * hostName)
{
    this->hostName_var = hostName;
}

simtime_t Timeout::getTimeout() const
{
    return timeout_var;
}

void Timeout::setTimeout(simtime_t timeout)
{
    this->timeout_var = timeout;
}

int Timeout::getSeq() const
{
    return seq_var;
}

void Timeout::setSeq(int seq)
{
    this->seq_var = seq;
}

const char * Timeout::getEvent() const
{
    return event_var.c_str();
}

void Timeout::setEvent(const char * event)
{
    this->event_var = event;
}

class TimeoutDescriptor : public cClassDescriptor
{
  public:
    TimeoutDescriptor();
    virtual ~TimeoutDescriptor();

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

Register_ClassDescriptor(TimeoutDescriptor);

TimeoutDescriptor::TimeoutDescriptor() : cClassDescriptor("Timeout", "cPacket")
{
}

TimeoutDescriptor::~TimeoutDescriptor()
{
}

bool TimeoutDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Timeout *>(obj)!=NULL;
}

const char *TimeoutDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int TimeoutDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int TimeoutDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *TimeoutDescriptor::getFieldName(void *object, int field) const
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
        "seq",
        "event",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int TimeoutDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='h' && strcmp(fieldName, "hostName")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeout")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "seq")==0) return base+2;
    if (fieldName[0]=='e' && strcmp(fieldName, "event")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *TimeoutDescriptor::getFieldTypeString(void *object, int field) const
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
        "int",
        "string",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *TimeoutDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int TimeoutDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Timeout *pp = (Timeout *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string TimeoutDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Timeout *pp = (Timeout *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getHostName());
        case 1: return double2string(pp->getTimeout());
        case 2: return long2string(pp->getSeq());
        case 3: return oppstring2string(pp->getEvent());
        default: return "";
    }
}

bool TimeoutDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Timeout *pp = (Timeout *)object; (void)pp;
    switch (field) {
        case 0: pp->setHostName((value)); return true;
        case 1: pp->setTimeout(string2double(value)); return true;
        case 2: pp->setSeq(string2long(value)); return true;
        case 3: pp->setEvent((value)); return true;
        default: return false;
    }
}

const char *TimeoutDescriptor::getFieldStructName(void *object, int field) const
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

void *TimeoutDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Timeout *pp = (Timeout *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


