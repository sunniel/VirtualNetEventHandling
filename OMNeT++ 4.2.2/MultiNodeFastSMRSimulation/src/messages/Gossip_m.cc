//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/Gossip.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Gossip_m.h"

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




Register_Class(Gossip);

Gossip::Gossip(const char *name, int kind) : cPacket(name,kind)
{
    this->sourceName_var = 0;
    this->destName_var = 0;
    this->sendTime_var = 0;
    this->lastRound_var = 0;
}

Gossip::Gossip(const Gossip& other) : cPacket(other)
{
    copy(other);
}

Gossip::~Gossip()
{
}

Gossip& Gossip::operator=(const Gossip& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void Gossip::copy(const Gossip& other)
{
    this->sourceName_var = other.sourceName_var;
    this->destName_var = other.destName_var;
    this->sendTime_var = other.sendTime_var;
    this->lastRound_var = other.lastRound_var;
}

void Gossip::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->sourceName_var);
    doPacking(b,this->destName_var);
    doPacking(b,this->sendTime_var);
    doPacking(b,this->lastRound_var);
}

void Gossip::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->sourceName_var);
    doUnpacking(b,this->destName_var);
    doUnpacking(b,this->sendTime_var);
    doUnpacking(b,this->lastRound_var);
}

const char * Gossip::getSourceName() const
{
    return sourceName_var.c_str();
}

void Gossip::setSourceName(const char * sourceName)
{
    this->sourceName_var = sourceName;
}

const char * Gossip::getDestName() const
{
    return destName_var.c_str();
}

void Gossip::setDestName(const char * destName)
{
    this->destName_var = destName;
}

simtime_t Gossip::getSendTime() const
{
    return sendTime_var;
}

void Gossip::setSendTime(simtime_t sendTime)
{
    this->sendTime_var = sendTime;
}

int Gossip::getLastRound() const
{
    return lastRound_var;
}

void Gossip::setLastRound(int lastRound)
{
    this->lastRound_var = lastRound;
}

class GossipDescriptor : public cClassDescriptor
{
  public:
    GossipDescriptor();
    virtual ~GossipDescriptor();

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

Register_ClassDescriptor(GossipDescriptor);

GossipDescriptor::GossipDescriptor() : cClassDescriptor("Gossip", "cPacket")
{
}

GossipDescriptor::~GossipDescriptor()
{
}

bool GossipDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Gossip *>(obj)!=NULL;
}

const char *GossipDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int GossipDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int GossipDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *GossipDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "sourceName",
        "destName",
        "sendTime",
        "lastRound",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int GossipDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceName")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destName")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sendTime")==0) return base+2;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastRound")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *GossipDescriptor::getFieldTypeString(void *object, int field) const
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
        "simtime_t",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *GossipDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int GossipDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Gossip *pp = (Gossip *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string GossipDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Gossip *pp = (Gossip *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getSourceName());
        case 1: return oppstring2string(pp->getDestName());
        case 2: return double2string(pp->getSendTime());
        case 3: return long2string(pp->getLastRound());
        default: return "";
    }
}

bool GossipDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Gossip *pp = (Gossip *)object; (void)pp;
    switch (field) {
        case 0: pp->setSourceName((value)); return true;
        case 1: pp->setDestName((value)); return true;
        case 2: pp->setSendTime(string2double(value)); return true;
        case 3: pp->setLastRound(string2long(value)); return true;
        default: return false;
    }
}

const char *GossipDescriptor::getFieldStructName(void *object, int field) const
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

void *GossipDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Gossip *pp = (Gossip *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


