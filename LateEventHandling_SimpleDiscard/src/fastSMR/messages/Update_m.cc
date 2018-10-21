//
// Generated file, do not edit! Created by opp_msgc 4.2 from fastSMR/messages/Update.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Update_m.h"

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




Register_Class(Update);

Update::Update(const char *name, int kind) : cPacket(name,kind)
{
    this->displayString_var = DISPLAY_STR_MSG_GROUP;
    this->senderId_var = 0;
    this->seq_var = 0;
    this->content_var = 0;
    this->sourceName_var = 0;
    this->destName_var = 0;
}

Update::Update(const Update& other) : cPacket(other)
{
    copy(other);
}

Update::~Update()
{
}

Update& Update::operator=(const Update& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void Update::copy(const Update& other)
{
    this->displayString_var = other.displayString_var;
    this->senderId_var = other.senderId_var;
    this->seq_var = other.seq_var;
    this->content_var = other.content_var;
    this->sourceName_var = other.sourceName_var;
    this->destName_var = other.destName_var;
}

void Update::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->displayString_var);
    doPacking(b,this->senderId_var);
    doPacking(b,this->seq_var);
    doPacking(b,this->content_var);
    doPacking(b,this->sourceName_var);
    doPacking(b,this->destName_var);
}

void Update::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->displayString_var);
    doUnpacking(b,this->senderId_var);
    doUnpacking(b,this->seq_var);
    doUnpacking(b,this->content_var);
    doUnpacking(b,this->sourceName_var);
    doUnpacking(b,this->destName_var);
}

const char * Update::getDisplayString() const
{
    return displayString_var.c_str();
}

void Update::setDisplayString(const char * displayString)
{
    this->displayString_var = displayString;
}

unsigned long Update::getSenderId() const
{
    return senderId_var;
}

void Update::setSenderId(unsigned long senderId)
{
    this->senderId_var = senderId;
}

int Update::getSeq() const
{
    return seq_var;
}

void Update::setSeq(int seq)
{
    this->seq_var = seq;
}

const char * Update::getContent() const
{
    return content_var.c_str();
}

void Update::setContent(const char * content)
{
    this->content_var = content;
}

const char * Update::getSourceName() const
{
    return sourceName_var.c_str();
}

void Update::setSourceName(const char * sourceName)
{
    this->sourceName_var = sourceName;
}

const char * Update::getDestName() const
{
    return destName_var.c_str();
}

void Update::setDestName(const char * destName)
{
    this->destName_var = destName;
}

class UpdateDescriptor : public cClassDescriptor
{
  public:
    UpdateDescriptor();
    virtual ~UpdateDescriptor();

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

Register_ClassDescriptor(UpdateDescriptor);

UpdateDescriptor::UpdateDescriptor() : cClassDescriptor("Update", "cPacket")
{
}

UpdateDescriptor::~UpdateDescriptor()
{
}

bool UpdateDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Update *>(obj)!=NULL;
}

const char *UpdateDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int UpdateDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int UpdateDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *UpdateDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "displayString",
        "senderId",
        "seq",
        "content",
        "sourceName",
        "destName",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int UpdateDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "displayString")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "seq")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "content")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceName")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "destName")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *UpdateDescriptor::getFieldTypeString(void *object, int field) const
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
        "int",
        "string",
        "string",
        "string",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *UpdateDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int UpdateDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Update *pp = (Update *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string UpdateDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Update *pp = (Update *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDisplayString());
        case 1: return ulong2string(pp->getSenderId());
        case 2: return long2string(pp->getSeq());
        case 3: return oppstring2string(pp->getContent());
        case 4: return oppstring2string(pp->getSourceName());
        case 5: return oppstring2string(pp->getDestName());
        default: return "";
    }
}

bool UpdateDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Update *pp = (Update *)object; (void)pp;
    switch (field) {
        case 0: pp->setDisplayString((value)); return true;
        case 1: pp->setSenderId(string2ulong(value)); return true;
        case 2: pp->setSeq(string2long(value)); return true;
        case 3: pp->setContent((value)); return true;
        case 4: pp->setSourceName((value)); return true;
        case 5: pp->setDestName((value)); return true;
        default: return false;
    }
}

const char *UpdateDescriptor::getFieldStructName(void *object, int field) const
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

void *UpdateDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Update *pp = (Update *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


