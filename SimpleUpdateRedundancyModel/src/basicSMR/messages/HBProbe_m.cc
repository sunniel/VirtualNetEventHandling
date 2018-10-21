//
// Generated file, do not edit! Created by opp_msgc 4.2 from basicSMR/messages/HBProbe.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "HBProbe_m.h"

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




Register_Class(HBProbe);

HBProbe::HBProbe(const char *name, int kind) : cPacket(name,kind)
{
    this->displayString_var = DISPLAY_STR_MSG_HIDDEN;
    this->sourceName_var = 0;
    this->destName_var = 0;
    this->survivals_var = 0;
    this->timestamp_var = 0;
    this->isInit_var = 0;
}

HBProbe::HBProbe(const HBProbe& other) : cPacket(other)
{
    copy(other);
}

HBProbe::~HBProbe()
{
}

HBProbe& HBProbe::operator=(const HBProbe& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void HBProbe::copy(const HBProbe& other)
{
    this->displayString_var = other.displayString_var;
    this->sourceName_var = other.sourceName_var;
    this->destName_var = other.destName_var;
    this->survivals_var = other.survivals_var;
    this->timestamp_var = other.timestamp_var;
    this->isInit_var = other.isInit_var;
}

void HBProbe::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->displayString_var);
    doPacking(b,this->sourceName_var);
    doPacking(b,this->destName_var);
    doPacking(b,this->survivals_var);
    doPacking(b,this->timestamp_var);
    doPacking(b,this->isInit_var);
}

void HBProbe::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->displayString_var);
    doUnpacking(b,this->sourceName_var);
    doUnpacking(b,this->destName_var);
    doUnpacking(b,this->survivals_var);
    doUnpacking(b,this->timestamp_var);
    doUnpacking(b,this->isInit_var);
}

const char * HBProbe::getDisplayString() const
{
    return displayString_var.c_str();
}

void HBProbe::setDisplayString(const char * displayString)
{
    this->displayString_var = displayString;
}

const char * HBProbe::getSourceName() const
{
    return sourceName_var.c_str();
}

void HBProbe::setSourceName(const char * sourceName)
{
    this->sourceName_var = sourceName;
}

const char * HBProbe::getDestName() const
{
    return destName_var.c_str();
}

void HBProbe::setDestName(const char * destName)
{
    this->destName_var = destName;
}

const char * HBProbe::getSurvivals() const
{
    return survivals_var.c_str();
}

void HBProbe::setSurvivals(const char * survivals)
{
    this->survivals_var = survivals;
}

simtime_t HBProbe::getTimestamp() const
{
    return timestamp_var;
}

void HBProbe::setTimestamp(simtime_t timestamp)
{
    this->timestamp_var = timestamp;
}

bool HBProbe::getIsInit() const
{
    return isInit_var;
}

void HBProbe::setIsInit(bool isInit)
{
    this->isInit_var = isInit;
}

class HBProbeDescriptor : public cClassDescriptor
{
  public:
    HBProbeDescriptor();
    virtual ~HBProbeDescriptor();

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

Register_ClassDescriptor(HBProbeDescriptor);

HBProbeDescriptor::HBProbeDescriptor() : cClassDescriptor("HBProbe", "cPacket")
{
}

HBProbeDescriptor::~HBProbeDescriptor()
{
}

bool HBProbeDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<HBProbe *>(obj)!=NULL;
}

const char *HBProbeDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int HBProbeDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int HBProbeDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *HBProbeDescriptor::getFieldName(void *object, int field) const
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
        "survivals",
        "timestamp",
        "isInit",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int HBProbeDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "displayString")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceName")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "destName")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "survivals")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+4;
    if (fieldName[0]=='i' && strcmp(fieldName, "isInit")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *HBProbeDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "simtime_t",
        "bool",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *HBProbeDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int HBProbeDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    HBProbe *pp = (HBProbe *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string HBProbeDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    HBProbe *pp = (HBProbe *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDisplayString());
        case 1: return oppstring2string(pp->getSourceName());
        case 2: return oppstring2string(pp->getDestName());
        case 3: return oppstring2string(pp->getSurvivals());
        case 4: return double2string(pp->getTimestamp());
        case 5: return bool2string(pp->getIsInit());
        default: return "";
    }
}

bool HBProbeDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    HBProbe *pp = (HBProbe *)object; (void)pp;
    switch (field) {
        case 0: pp->setDisplayString((value)); return true;
        case 1: pp->setSourceName((value)); return true;
        case 2: pp->setDestName((value)); return true;
        case 3: pp->setSurvivals((value)); return true;
        case 4: pp->setTimestamp(string2double(value)); return true;
        case 5: pp->setIsInit(string2bool(value)); return true;
        default: return false;
    }
}

const char *HBProbeDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<6) ? fieldStructNames[field] : NULL;
}

void *HBProbeDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    HBProbe *pp = (HBProbe *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


