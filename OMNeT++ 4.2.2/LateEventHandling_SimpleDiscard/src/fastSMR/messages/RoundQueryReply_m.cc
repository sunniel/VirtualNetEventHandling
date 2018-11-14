//
// Generated file, do not edit! Created by opp_msgc 4.2 from fastSMR/messages/RoundQueryReply.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "RoundQueryReply_m.h"

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




Register_Class(RoundQueryReply);

RoundQueryReply::RoundQueryReply(const char *name, int kind) : cPacket(name,kind)
{
    this->displayString_var = DISPLAY_STR_MSG_GROUP_EH;
    this->senderName_var = 0;
    this->destName_var = 0;
    this->round_var = 0;
    this->events_var = 0;
}

RoundQueryReply::RoundQueryReply(const RoundQueryReply& other) : cPacket(other)
{
    copy(other);
}

RoundQueryReply::~RoundQueryReply()
{
}

RoundQueryReply& RoundQueryReply::operator=(const RoundQueryReply& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void RoundQueryReply::copy(const RoundQueryReply& other)
{
    this->displayString_var = other.displayString_var;
    this->senderName_var = other.senderName_var;
    this->destName_var = other.destName_var;
    this->round_var = other.round_var;
    this->events_var = other.events_var;
}

void RoundQueryReply::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->displayString_var);
    doPacking(b,this->senderName_var);
    doPacking(b,this->destName_var);
    doPacking(b,this->round_var);
    doPacking(b,this->events_var);
}

void RoundQueryReply::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->displayString_var);
    doUnpacking(b,this->senderName_var);
    doUnpacking(b,this->destName_var);
    doUnpacking(b,this->round_var);
    doUnpacking(b,this->events_var);
}

const char * RoundQueryReply::getDisplayString() const
{
    return displayString_var.c_str();
}

void RoundQueryReply::setDisplayString(const char * displayString)
{
    this->displayString_var = displayString;
}

const char * RoundQueryReply::getSenderName() const
{
    return senderName_var.c_str();
}

void RoundQueryReply::setSenderName(const char * senderName)
{
    this->senderName_var = senderName;
}

const char * RoundQueryReply::getDestName() const
{
    return destName_var.c_str();
}

void RoundQueryReply::setDestName(const char * destName)
{
    this->destName_var = destName;
}

int RoundQueryReply::getRound() const
{
    return round_var;
}

void RoundQueryReply::setRound(int round)
{
    this->round_var = round;
}

const char * RoundQueryReply::getEvents() const
{
    return events_var.c_str();
}

void RoundQueryReply::setEvents(const char * events)
{
    this->events_var = events;
}

class RoundQueryReplyDescriptor : public cClassDescriptor
{
  public:
    RoundQueryReplyDescriptor();
    virtual ~RoundQueryReplyDescriptor();

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

Register_ClassDescriptor(RoundQueryReplyDescriptor);

RoundQueryReplyDescriptor::RoundQueryReplyDescriptor() : cClassDescriptor("RoundQueryReply", "cPacket")
{
}

RoundQueryReplyDescriptor::~RoundQueryReplyDescriptor()
{
}

bool RoundQueryReplyDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<RoundQueryReply *>(obj)!=NULL;
}

const char *RoundQueryReplyDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int RoundQueryReplyDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int RoundQueryReplyDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *RoundQueryReplyDescriptor::getFieldName(void *object, int field) const
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
        "destName",
        "round",
        "events",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int RoundQueryReplyDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "displayString")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderName")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "destName")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "round")==0) return base+3;
    if (fieldName[0]=='e' && strcmp(fieldName, "events")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *RoundQueryReplyDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "int",
        "string",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *RoundQueryReplyDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int RoundQueryReplyDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    RoundQueryReply *pp = (RoundQueryReply *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RoundQueryReplyDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    RoundQueryReply *pp = (RoundQueryReply *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDisplayString());
        case 1: return oppstring2string(pp->getSenderName());
        case 2: return oppstring2string(pp->getDestName());
        case 3: return long2string(pp->getRound());
        case 4: return oppstring2string(pp->getEvents());
        default: return "";
    }
}

bool RoundQueryReplyDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    RoundQueryReply *pp = (RoundQueryReply *)object; (void)pp;
    switch (field) {
        case 0: pp->setDisplayString((value)); return true;
        case 1: pp->setSenderName((value)); return true;
        case 2: pp->setDestName((value)); return true;
        case 3: pp->setRound(string2long(value)); return true;
        case 4: pp->setEvents((value)); return true;
        default: return false;
    }
}

const char *RoundQueryReplyDescriptor::getFieldStructName(void *object, int field) const
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

void *RoundQueryReplyDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    RoundQueryReply *pp = (RoundQueryReply *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


