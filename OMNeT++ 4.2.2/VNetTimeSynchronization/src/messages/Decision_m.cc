//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/Decision.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Decision_m.h"

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




Register_Class(Decision);

Decision::Decision(const char *name, int kind) : cPacket(name,kind)
{
    this->displayString_var = DISPLAY_STR_MSG_GROUP_EH;
    this->senderName_var = 0;
    this->round_var = 0;
    this->epoch_var = 0;
    this->groupId_var = 0;
    this->decision_var = 0;
}

Decision::Decision(const Decision& other) : cPacket(other)
{
    copy(other);
}

Decision::~Decision()
{
}

Decision& Decision::operator=(const Decision& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void Decision::copy(const Decision& other)
{
    this->displayString_var = other.displayString_var;
    this->senderName_var = other.senderName_var;
    this->round_var = other.round_var;
    this->epoch_var = other.epoch_var;
    this->groupId_var = other.groupId_var;
    this->decision_var = other.decision_var;
}

void Decision::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->displayString_var);
    doPacking(b,this->senderName_var);
    doPacking(b,this->round_var);
    doPacking(b,this->epoch_var);
    doPacking(b,this->groupId_var);
    doPacking(b,this->decision_var);
}

void Decision::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->displayString_var);
    doUnpacking(b,this->senderName_var);
    doUnpacking(b,this->round_var);
    doUnpacking(b,this->epoch_var);
    doUnpacking(b,this->groupId_var);
    doUnpacking(b,this->decision_var);
}

const char * Decision::getDisplayString() const
{
    return displayString_var.c_str();
}

void Decision::setDisplayString(const char * displayString)
{
    this->displayString_var = displayString;
}

const char * Decision::getSenderName() const
{
    return senderName_var.c_str();
}

void Decision::setSenderName(const char * senderName)
{
    this->senderName_var = senderName;
}

int Decision::getRound() const
{
    return round_var;
}

void Decision::setRound(int round)
{
    this->round_var = round;
}

int Decision::getEpoch() const
{
    return epoch_var;
}

void Decision::setEpoch(int epoch)
{
    this->epoch_var = epoch;
}

int Decision::getGroupId() const
{
    return groupId_var;
}

void Decision::setGroupId(int groupId)
{
    this->groupId_var = groupId;
}

const char * Decision::getDecision() const
{
    return decision_var.c_str();
}

void Decision::setDecision(const char * decision)
{
    this->decision_var = decision;
}

class DecisionDescriptor : public cClassDescriptor
{
  public:
    DecisionDescriptor();
    virtual ~DecisionDescriptor();

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

Register_ClassDescriptor(DecisionDescriptor);

DecisionDescriptor::DecisionDescriptor() : cClassDescriptor("Decision", "cPacket")
{
}

DecisionDescriptor::~DecisionDescriptor()
{
}

bool DecisionDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Decision *>(obj)!=NULL;
}

const char *DecisionDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int DecisionDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int DecisionDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *DecisionDescriptor::getFieldName(void *object, int field) const
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
        "round",
        "epoch",
        "groupId",
        "decision",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int DecisionDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "displayString")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderName")==0) return base+1;
    if (fieldName[0]=='r' && strcmp(fieldName, "round")==0) return base+2;
    if (fieldName[0]=='e' && strcmp(fieldName, "epoch")==0) return base+3;
    if (fieldName[0]=='g' && strcmp(fieldName, "groupId")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "decision")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *DecisionDescriptor::getFieldTypeString(void *object, int field) const
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
        "int",
        "string",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *DecisionDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int DecisionDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Decision *pp = (Decision *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string DecisionDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Decision *pp = (Decision *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDisplayString());
        case 1: return oppstring2string(pp->getSenderName());
        case 2: return long2string(pp->getRound());
        case 3: return long2string(pp->getEpoch());
        case 4: return long2string(pp->getGroupId());
        case 5: return oppstring2string(pp->getDecision());
        default: return "";
    }
}

bool DecisionDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Decision *pp = (Decision *)object; (void)pp;
    switch (field) {
        case 0: pp->setDisplayString((value)); return true;
        case 1: pp->setSenderName((value)); return true;
        case 2: pp->setRound(string2long(value)); return true;
        case 3: pp->setEpoch(string2long(value)); return true;
        case 4: pp->setGroupId(string2long(value)); return true;
        case 5: pp->setDecision((value)); return true;
        default: return false;
    }
}

const char *DecisionDescriptor::getFieldStructName(void *object, int field) const
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

void *DecisionDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Decision *pp = (Decision *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


