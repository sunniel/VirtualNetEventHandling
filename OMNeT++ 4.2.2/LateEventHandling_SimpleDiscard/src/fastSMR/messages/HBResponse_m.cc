//
// Generated file, do not edit! Created by opp_msgc 4.2 from fastSMR/messages/HBResponse.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "HBResponse_m.h"

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




Register_Class(HBResponse);

HBResponse::HBResponse(const char *name, int kind) : cPacket(name,kind)
{
    this->displayString_var = DISPLAY_STR_MSG_HIDDEN;
    this->sourceName_var = 0;
    this->destName_var = 0;
    this->timestamp_var = 0;
}

HBResponse::HBResponse(const HBResponse& other) : cPacket(other)
{
    copy(other);
}

HBResponse::~HBResponse()
{
}

HBResponse& HBResponse::operator=(const HBResponse& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void HBResponse::copy(const HBResponse& other)
{
    this->displayString_var = other.displayString_var;
    this->sourceName_var = other.sourceName_var;
    this->destName_var = other.destName_var;
    this->timestamp_var = other.timestamp_var;
}

void HBResponse::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->displayString_var);
    doPacking(b,this->sourceName_var);
    doPacking(b,this->destName_var);
    doPacking(b,this->timestamp_var);
}

void HBResponse::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->displayString_var);
    doUnpacking(b,this->sourceName_var);
    doUnpacking(b,this->destName_var);
    doUnpacking(b,this->timestamp_var);
}

const char * HBResponse::getDisplayString() const
{
    return displayString_var.c_str();
}

void HBResponse::setDisplayString(const char * displayString)
{
    this->displayString_var = displayString;
}

const char * HBResponse::getSourceName() const
{
    return sourceName_var.c_str();
}

void HBResponse::setSourceName(const char * sourceName)
{
    this->sourceName_var = sourceName;
}

const char * HBResponse::getDestName() const
{
    return destName_var.c_str();
}

void HBResponse::setDestName(const char * destName)
{
    this->destName_var = destName;
}

simtime_t HBResponse::getTimestamp() const
{
    return timestamp_var;
}

void HBResponse::setTimestamp(simtime_t timestamp)
{
    this->timestamp_var = timestamp;
}

class HBResponseDescriptor : public cClassDescriptor
{
  public:
    HBResponseDescriptor();
    virtual ~HBResponseDescriptor();

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

Register_ClassDescriptor(HBResponseDescriptor);

HBResponseDescriptor::HBResponseDescriptor() : cClassDescriptor("HBResponse", "cPacket")
{
}

HBResponseDescriptor::~HBResponseDescriptor()
{
}

bool HBResponseDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<HBResponse *>(obj)!=NULL;
}

const char *HBResponseDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int HBResponseDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int HBResponseDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *HBResponseDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "displayString",
        "sourceName",
        "destName",
        "timestamp",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int HBResponseDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "displayString")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceName")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "destName")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *HBResponseDescriptor::getFieldTypeString(void *object, int field) const
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
        "simtime_t",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *HBResponseDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int HBResponseDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    HBResponse *pp = (HBResponse *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string HBResponseDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    HBResponse *pp = (HBResponse *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDisplayString());
        case 1: return oppstring2string(pp->getSourceName());
        case 2: return oppstring2string(pp->getDestName());
        case 3: return double2string(pp->getTimestamp());
        default: return "";
    }
}

bool HBResponseDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    HBResponse *pp = (HBResponse *)object; (void)pp;
    switch (field) {
        case 0: pp->setDisplayString((value)); return true;
        case 1: pp->setSourceName((value)); return true;
        case 2: pp->setDestName((value)); return true;
        case 3: pp->setTimestamp(string2double(value)); return true;
        default: return false;
    }
}

const char *HBResponseDescriptor::getFieldStructName(void *object, int field) const
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

void *HBResponseDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    HBResponse *pp = (HBResponse *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


