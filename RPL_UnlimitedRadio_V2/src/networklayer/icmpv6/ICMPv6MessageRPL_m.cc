//
// Generated file, do not edit! Created by nedtool 5.2 from src/networklayer/icmpv6/ICMPv6MessageRPL.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "ICMPv6MessageRPL_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::ICMPv6TypeRPL");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::ICMPv6TypeRPL"));
    e->insert(ICMPv6_RPL_CONTROL_MESSAGE, "ICMPv6_RPL_CONTROL_MESSAGE");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::ICMPv6_RPL_CONTROL_MSG");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::ICMPv6_RPL_CONTROL_MSG"));
    e->insert(DIS, "DIS");
    e->insert(DIO, "DIO");
    e->insert(DAO, "DAO");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::RPL_OPTIONS");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::RPL_OPTIONS"));
    e->insert(PAD1, "PAD1");
    e->insert(PADN, "PADN");
    e->insert(Solicited_Information, "Solicited_Information");
    e->insert(DAG_Metric_Container, "DAG_Metric_Container");
    e->insert(Routing_Information, "Routing_Information");
    e->insert(DODAG_Configuration, "DODAG_Configuration");
    e->insert(Prefix_Information, "Prefix_Information");
    e->insert(RPL_Target, "RPL_Target");
    e->insert(Transit_Information, "Transit_Information");
    e->insert(RPL_Target_Descriptor, "RPL_Target_Descriptor");
)

Register_Class(ICMPv6DISMsg)

ICMPv6DISMsg::ICMPv6DISMsg(const char *name, short kind) : ::inet::ICMPv6Message(name,kind)
{
    this->code = 0;
    this->flags = 0;
    this->reserved = 0;
    this->RPLInstanceID = 0;
    this->VersionNumber = 0;
    this->V = 0;
    this->I = 0;
    this->D = 0;
    this->Flag = 0;
    this->options = 0;
}

ICMPv6DISMsg::ICMPv6DISMsg(const ICMPv6DISMsg& other) : ::inet::ICMPv6Message(other)
{
    copy(other);
}

ICMPv6DISMsg::~ICMPv6DISMsg()
{
}

ICMPv6DISMsg& ICMPv6DISMsg::operator=(const ICMPv6DISMsg& other)
{
    if (this==&other) return *this;
    ::inet::ICMPv6Message::operator=(other);
    copy(other);
    return *this;
}

void ICMPv6DISMsg::copy(const ICMPv6DISMsg& other)
{
    this->code = other.code;
    this->flags = other.flags;
    this->reserved = other.reserved;
    this->RPLInstanceID = other.RPLInstanceID;
    this->VersionNumber = other.VersionNumber;
    this->V = other.V;
    this->I = other.I;
    this->D = other.D;
    this->Flag = other.Flag;
    this->DODAGID = other.DODAGID;
    this->options = other.options;
}

void ICMPv6DISMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::ICMPv6Message::parsimPack(b);
    doParsimPacking(b,this->code);
    doParsimPacking(b,this->flags);
    doParsimPacking(b,this->reserved);
    doParsimPacking(b,this->RPLInstanceID);
    doParsimPacking(b,this->VersionNumber);
    doParsimPacking(b,this->V);
    doParsimPacking(b,this->I);
    doParsimPacking(b,this->D);
    doParsimPacking(b,this->Flag);
    doParsimPacking(b,this->DODAGID);
    doParsimPacking(b,this->options);
}

void ICMPv6DISMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::ICMPv6Message::parsimUnpack(b);
    doParsimUnpacking(b,this->code);
    doParsimUnpacking(b,this->flags);
    doParsimUnpacking(b,this->reserved);
    doParsimUnpacking(b,this->RPLInstanceID);
    doParsimUnpacking(b,this->VersionNumber);
    doParsimUnpacking(b,this->V);
    doParsimUnpacking(b,this->I);
    doParsimUnpacking(b,this->D);
    doParsimUnpacking(b,this->Flag);
    doParsimUnpacking(b,this->DODAGID);
    doParsimUnpacking(b,this->options);
}

int ICMPv6DISMsg::getCode() const
{
    return this->code;
}

void ICMPv6DISMsg::setCode(int code)
{
    this->code = code;
}

int ICMPv6DISMsg::getFlags() const
{
    return this->flags;
}

void ICMPv6DISMsg::setFlags(int flags)
{
    this->flags = flags;
}

int ICMPv6DISMsg::getReserved() const
{
    return this->reserved;
}

void ICMPv6DISMsg::setReserved(int reserved)
{
    this->reserved = reserved;
}

int ICMPv6DISMsg::getRPLInstanceID() const
{
    return this->RPLInstanceID;
}

void ICMPv6DISMsg::setRPLInstanceID(int RPLInstanceID)
{
    this->RPLInstanceID = RPLInstanceID;
}

int ICMPv6DISMsg::getVersionNumber() const
{
    return this->VersionNumber;
}

void ICMPv6DISMsg::setVersionNumber(int VersionNumber)
{
    this->VersionNumber = VersionNumber;
}

int ICMPv6DISMsg::getV() const
{
    return this->V;
}

void ICMPv6DISMsg::setV(int V)
{
    this->V = V;
}

int ICMPv6DISMsg::getI() const
{
    return this->I;
}

void ICMPv6DISMsg::setI(int I)
{
    this->I = I;
}

int ICMPv6DISMsg::getD() const
{
    return this->D;
}

void ICMPv6DISMsg::setD(int D)
{
    this->D = D;
}

int ICMPv6DISMsg::getFlag() const
{
    return this->Flag;
}

void ICMPv6DISMsg::setFlag(int Flag)
{
    this->Flag = Flag;
}

IPv6Address& ICMPv6DISMsg::getDODAGID()
{
    return this->DODAGID;
}

void ICMPv6DISMsg::setDODAGID(const IPv6Address& DODAGID)
{
    this->DODAGID = DODAGID;
}

int ICMPv6DISMsg::getOptions() const
{
    return this->options;
}

void ICMPv6DISMsg::setOptions(int options)
{
    this->options = options;
}

class ICMPv6DISMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ICMPv6DISMsgDescriptor();
    virtual ~ICMPv6DISMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ICMPv6DISMsgDescriptor)

ICMPv6DISMsgDescriptor::ICMPv6DISMsgDescriptor() : omnetpp::cClassDescriptor("inet::ICMPv6DISMsg", "inet::ICMPv6Message")
{
    propertynames = nullptr;
}

ICMPv6DISMsgDescriptor::~ICMPv6DISMsgDescriptor()
{
    delete[] propertynames;
}

bool ICMPv6DISMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ICMPv6DISMsg *>(obj)!=nullptr;
}

const char **ICMPv6DISMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ICMPv6DISMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ICMPv6DISMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 11+basedesc->getFieldCount() : 11;
}

unsigned int ICMPv6DISMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<11) ? fieldTypeFlags[field] : 0;
}

const char *ICMPv6DISMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "code",
        "flags",
        "reserved",
        "RPLInstanceID",
        "VersionNumber",
        "V",
        "I",
        "D",
        "Flag",
        "DODAGID",
        "options",
    };
    return (field>=0 && field<11) ? fieldNames[field] : nullptr;
}

int ICMPv6DISMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "code")==0) return base+0;
    if (fieldName[0]=='f' && strcmp(fieldName, "flags")==0) return base+1;
    if (fieldName[0]=='r' && strcmp(fieldName, "reserved")==0) return base+2;
    if (fieldName[0]=='R' && strcmp(fieldName, "RPLInstanceID")==0) return base+3;
    if (fieldName[0]=='V' && strcmp(fieldName, "VersionNumber")==0) return base+4;
    if (fieldName[0]=='V' && strcmp(fieldName, "V")==0) return base+5;
    if (fieldName[0]=='I' && strcmp(fieldName, "I")==0) return base+6;
    if (fieldName[0]=='D' && strcmp(fieldName, "D")==0) return base+7;
    if (fieldName[0]=='F' && strcmp(fieldName, "Flag")==0) return base+8;
    if (fieldName[0]=='D' && strcmp(fieldName, "DODAGID")==0) return base+9;
    if (fieldName[0]=='o' && strcmp(fieldName, "options")==0) return base+10;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ICMPv6DISMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "IPv6Address",
        "int",
    };
    return (field>=0 && field<11) ? fieldTypeStrings[field] : nullptr;
}

const char **ICMPv6DISMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case 10: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *ICMPv6DISMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "inet::ICMPv6_RPL_CONTROL_MSG";
            return nullptr;
        case 10:
            if (!strcmp(propertyname,"enum")) return "inet::RPL_OPTIONS";
            return nullptr;
        default: return nullptr;
    }
}

int ICMPv6DISMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DISMsg *pp = (ICMPv6DISMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ICMPv6DISMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DISMsg *pp = (ICMPv6DISMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ICMPv6DISMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DISMsg *pp = (ICMPv6DISMsg *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getCode(), "inet::ICMPv6_RPL_CONTROL_MSG");
        case 1: return long2string(pp->getFlags());
        case 2: return long2string(pp->getReserved());
        case 3: return long2string(pp->getRPLInstanceID());
        case 4: return long2string(pp->getVersionNumber());
        case 5: return long2string(pp->getV());
        case 6: return long2string(pp->getI());
        case 7: return long2string(pp->getD());
        case 8: return long2string(pp->getFlag());
        case 9: {std::stringstream out; out << pp->getDODAGID(); return out.str();}
        case 10: return enum2string(pp->getOptions(), "inet::RPL_OPTIONS");
        default: return "";
    }
}

bool ICMPv6DISMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DISMsg *pp = (ICMPv6DISMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setCode((inet::ICMPv6_RPL_CONTROL_MSG)string2enum(value, "inet::ICMPv6_RPL_CONTROL_MSG")); return true;
        case 1: pp->setFlags(string2long(value)); return true;
        case 2: pp->setReserved(string2long(value)); return true;
        case 3: pp->setRPLInstanceID(string2long(value)); return true;
        case 4: pp->setVersionNumber(string2long(value)); return true;
        case 5: pp->setV(string2long(value)); return true;
        case 6: pp->setI(string2long(value)); return true;
        case 7: pp->setD(string2long(value)); return true;
        case 8: pp->setFlag(string2long(value)); return true;
        case 10: pp->setOptions((inet::RPL_OPTIONS)string2enum(value, "inet::RPL_OPTIONS")); return true;
        default: return false;
    }
}

const char *ICMPv6DISMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 9: return omnetpp::opp_typename(typeid(IPv6Address));
        default: return nullptr;
    };
}

void *ICMPv6DISMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DISMsg *pp = (ICMPv6DISMsg *)object; (void)pp;
    switch (field) {
        case 9: return (void *)(&pp->getDODAGID()); break;
        default: return nullptr;
    }
}

Register_Class(ICMPv6DIOMsg)

ICMPv6DIOMsg::ICMPv6DIOMsg(const char *name, short kind) : ::inet::ICMPv6Message(name,kind)
{
    this->code = 0;
    this->versionNumber = 0;
    this->rank = 0;
    this->grounded = 0;
    this->dtsn = 0;
    this->IMin = 0;
    this->NofDoub = 0;
    this->k = 0;
    this->options = 0;
}

ICMPv6DIOMsg::ICMPv6DIOMsg(const ICMPv6DIOMsg& other) : ::inet::ICMPv6Message(other)
{
    copy(other);
}

ICMPv6DIOMsg::~ICMPv6DIOMsg()
{
}

ICMPv6DIOMsg& ICMPv6DIOMsg::operator=(const ICMPv6DIOMsg& other)
{
    if (this==&other) return *this;
    ::inet::ICMPv6Message::operator=(other);
    copy(other);
    return *this;
}

void ICMPv6DIOMsg::copy(const ICMPv6DIOMsg& other)
{
    this->code = other.code;
    this->versionNumber = other.versionNumber;
    this->rank = other.rank;
    this->grounded = other.grounded;
    this->dtsn = other.dtsn;
    this->IMin = other.IMin;
    this->NofDoub = other.NofDoub;
    this->k = other.k;
    this->DODAGID = other.DODAGID;
    this->options = other.options;
}

void ICMPv6DIOMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::ICMPv6Message::parsimPack(b);
    doParsimPacking(b,this->code);
    doParsimPacking(b,this->versionNumber);
    doParsimPacking(b,this->rank);
    doParsimPacking(b,this->grounded);
    doParsimPacking(b,this->dtsn);
    doParsimPacking(b,this->IMin);
    doParsimPacking(b,this->NofDoub);
    doParsimPacking(b,this->k);
    doParsimPacking(b,this->DODAGID);
    doParsimPacking(b,this->options);
}

void ICMPv6DIOMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::ICMPv6Message::parsimUnpack(b);
    doParsimUnpacking(b,this->code);
    doParsimUnpacking(b,this->versionNumber);
    doParsimUnpacking(b,this->rank);
    doParsimUnpacking(b,this->grounded);
    doParsimUnpacking(b,this->dtsn);
    doParsimUnpacking(b,this->IMin);
    doParsimUnpacking(b,this->NofDoub);
    doParsimUnpacking(b,this->k);
    doParsimUnpacking(b,this->DODAGID);
    doParsimUnpacking(b,this->options);
}

int ICMPv6DIOMsg::getCode() const
{
    return this->code;
}

void ICMPv6DIOMsg::setCode(int code)
{
    this->code = code;
}

int ICMPv6DIOMsg::getVersionNumber() const
{
    return this->versionNumber;
}

void ICMPv6DIOMsg::setVersionNumber(int versionNumber)
{
    this->versionNumber = versionNumber;
}

int ICMPv6DIOMsg::getRank() const
{
    return this->rank;
}

void ICMPv6DIOMsg::setRank(int rank)
{
    this->rank = rank;
}

int ICMPv6DIOMsg::getGrounded() const
{
    return this->grounded;
}

void ICMPv6DIOMsg::setGrounded(int grounded)
{
    this->grounded = grounded;
}

int ICMPv6DIOMsg::getDtsn() const
{
    return this->dtsn;
}

void ICMPv6DIOMsg::setDtsn(int dtsn)
{
    this->dtsn = dtsn;
}

double ICMPv6DIOMsg::getIMin() const
{
    return this->IMin;
}

void ICMPv6DIOMsg::setIMin(double IMin)
{
    this->IMin = IMin;
}

int ICMPv6DIOMsg::getNofDoub() const
{
    return this->NofDoub;
}

void ICMPv6DIOMsg::setNofDoub(int NofDoub)
{
    this->NofDoub = NofDoub;
}

int ICMPv6DIOMsg::getK() const
{
    return this->k;
}

void ICMPv6DIOMsg::setK(int k)
{
    this->k = k;
}

IPv6Address& ICMPv6DIOMsg::getDODAGID()
{
    return this->DODAGID;
}

void ICMPv6DIOMsg::setDODAGID(const IPv6Address& DODAGID)
{
    this->DODAGID = DODAGID;
}

int ICMPv6DIOMsg::getOptions() const
{
    return this->options;
}

void ICMPv6DIOMsg::setOptions(int options)
{
    this->options = options;
}

class ICMPv6DIOMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ICMPv6DIOMsgDescriptor();
    virtual ~ICMPv6DIOMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ICMPv6DIOMsgDescriptor)

ICMPv6DIOMsgDescriptor::ICMPv6DIOMsgDescriptor() : omnetpp::cClassDescriptor("inet::ICMPv6DIOMsg", "inet::ICMPv6Message")
{
    propertynames = nullptr;
}

ICMPv6DIOMsgDescriptor::~ICMPv6DIOMsgDescriptor()
{
    delete[] propertynames;
}

bool ICMPv6DIOMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ICMPv6DIOMsg *>(obj)!=nullptr;
}

const char **ICMPv6DIOMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ICMPv6DIOMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ICMPv6DIOMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 10+basedesc->getFieldCount() : 10;
}

unsigned int ICMPv6DIOMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<10) ? fieldTypeFlags[field] : 0;
}

const char *ICMPv6DIOMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "code",
        "versionNumber",
        "rank",
        "grounded",
        "dtsn",
        "IMin",
        "NofDoub",
        "k",
        "DODAGID",
        "options",
    };
    return (field>=0 && field<10) ? fieldNames[field] : nullptr;
}

int ICMPv6DIOMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "code")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "versionNumber")==0) return base+1;
    if (fieldName[0]=='r' && strcmp(fieldName, "rank")==0) return base+2;
    if (fieldName[0]=='g' && strcmp(fieldName, "grounded")==0) return base+3;
    if (fieldName[0]=='d' && strcmp(fieldName, "dtsn")==0) return base+4;
    if (fieldName[0]=='I' && strcmp(fieldName, "IMin")==0) return base+5;
    if (fieldName[0]=='N' && strcmp(fieldName, "NofDoub")==0) return base+6;
    if (fieldName[0]=='k' && strcmp(fieldName, "k")==0) return base+7;
    if (fieldName[0]=='D' && strcmp(fieldName, "DODAGID")==0) return base+8;
    if (fieldName[0]=='o' && strcmp(fieldName, "options")==0) return base+9;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ICMPv6DIOMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "double",
        "int",
        "int",
        "IPv6Address",
        "int",
    };
    return (field>=0 && field<10) ? fieldTypeStrings[field] : nullptr;
}

const char **ICMPv6DIOMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case 9: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *ICMPv6DIOMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "inet::ICMPv6_RPL_CONTROL_MSG";
            return nullptr;
        case 9:
            if (!strcmp(propertyname,"enum")) return "inet::RPL_OPTIONS";
            return nullptr;
        default: return nullptr;
    }
}

int ICMPv6DIOMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DIOMsg *pp = (ICMPv6DIOMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ICMPv6DIOMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DIOMsg *pp = (ICMPv6DIOMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ICMPv6DIOMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DIOMsg *pp = (ICMPv6DIOMsg *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getCode(), "inet::ICMPv6_RPL_CONTROL_MSG");
        case 1: return long2string(pp->getVersionNumber());
        case 2: return long2string(pp->getRank());
        case 3: return long2string(pp->getGrounded());
        case 4: return long2string(pp->getDtsn());
        case 5: return double2string(pp->getIMin());
        case 6: return long2string(pp->getNofDoub());
        case 7: return long2string(pp->getK());
        case 8: {std::stringstream out; out << pp->getDODAGID(); return out.str();}
        case 9: return enum2string(pp->getOptions(), "inet::RPL_OPTIONS");
        default: return "";
    }
}

bool ICMPv6DIOMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DIOMsg *pp = (ICMPv6DIOMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setCode((inet::ICMPv6_RPL_CONTROL_MSG)string2enum(value, "inet::ICMPv6_RPL_CONTROL_MSG")); return true;
        case 1: pp->setVersionNumber(string2long(value)); return true;
        case 2: pp->setRank(string2long(value)); return true;
        case 3: pp->setGrounded(string2long(value)); return true;
        case 4: pp->setDtsn(string2long(value)); return true;
        case 5: pp->setIMin(string2double(value)); return true;
        case 6: pp->setNofDoub(string2long(value)); return true;
        case 7: pp->setK(string2long(value)); return true;
        case 9: pp->setOptions((inet::RPL_OPTIONS)string2enum(value, "inet::RPL_OPTIONS")); return true;
        default: return false;
    }
}

const char *ICMPv6DIOMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 8: return omnetpp::opp_typename(typeid(IPv6Address));
        default: return nullptr;
    };
}

void *ICMPv6DIOMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DIOMsg *pp = (ICMPv6DIOMsg *)object; (void)pp;
    switch (field) {
        case 8: return (void *)(&pp->getDODAGID()); break;
        default: return nullptr;
    }
}

Register_Class(ICMPv6DAOMsg)

ICMPv6DAOMsg::ICMPv6DAOMsg(const char *name, short kind) : ::inet::ICMPv6Message(name,kind)
{
    this->code = 0;
    this->d = 0;
    this->options = 0;
}

ICMPv6DAOMsg::ICMPv6DAOMsg(const ICMPv6DAOMsg& other) : ::inet::ICMPv6Message(other)
{
    copy(other);
}

ICMPv6DAOMsg::~ICMPv6DAOMsg()
{
}

ICMPv6DAOMsg& ICMPv6DAOMsg::operator=(const ICMPv6DAOMsg& other)
{
    if (this==&other) return *this;
    ::inet::ICMPv6Message::operator=(other);
    copy(other);
    return *this;
}

void ICMPv6DAOMsg::copy(const ICMPv6DAOMsg& other)
{
    this->code = other.code;
    this->d = other.d;
    this->DODAGID = other.DODAGID;
    this->options = other.options;
}

void ICMPv6DAOMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::ICMPv6Message::parsimPack(b);
    doParsimPacking(b,this->code);
    doParsimPacking(b,this->d);
    doParsimPacking(b,this->DODAGID);
    doParsimPacking(b,this->options);
}

void ICMPv6DAOMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::ICMPv6Message::parsimUnpack(b);
    doParsimUnpacking(b,this->code);
    doParsimUnpacking(b,this->d);
    doParsimUnpacking(b,this->DODAGID);
    doParsimUnpacking(b,this->options);
}

int ICMPv6DAOMsg::getCode() const
{
    return this->code;
}

void ICMPv6DAOMsg::setCode(int code)
{
    this->code = code;
}

int ICMPv6DAOMsg::getD() const
{
    return this->d;
}

void ICMPv6DAOMsg::setD(int d)
{
    this->d = d;
}

IPv6Address& ICMPv6DAOMsg::getDODAGID()
{
    return this->DODAGID;
}

void ICMPv6DAOMsg::setDODAGID(const IPv6Address& DODAGID)
{
    this->DODAGID = DODAGID;
}

int ICMPv6DAOMsg::getOptions() const
{
    return this->options;
}

void ICMPv6DAOMsg::setOptions(int options)
{
    this->options = options;
}

class ICMPv6DAOMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ICMPv6DAOMsgDescriptor();
    virtual ~ICMPv6DAOMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ICMPv6DAOMsgDescriptor)

ICMPv6DAOMsgDescriptor::ICMPv6DAOMsgDescriptor() : omnetpp::cClassDescriptor("inet::ICMPv6DAOMsg", "inet::ICMPv6Message")
{
    propertynames = nullptr;
}

ICMPv6DAOMsgDescriptor::~ICMPv6DAOMsgDescriptor()
{
    delete[] propertynames;
}

bool ICMPv6DAOMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ICMPv6DAOMsg *>(obj)!=nullptr;
}

const char **ICMPv6DAOMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ICMPv6DAOMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ICMPv6DAOMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount() : 4;
}

unsigned int ICMPv6DAOMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *ICMPv6DAOMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "code",
        "d",
        "DODAGID",
        "options",
    };
    return (field>=0 && field<4) ? fieldNames[field] : nullptr;
}

int ICMPv6DAOMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "code")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "d")==0) return base+1;
    if (fieldName[0]=='D' && strcmp(fieldName, "DODAGID")==0) return base+2;
    if (fieldName[0]=='o' && strcmp(fieldName, "options")==0) return base+3;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ICMPv6DAOMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "IPv6Address",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : nullptr;
}

const char **ICMPv6DAOMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case 3: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *ICMPv6DAOMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "inet::ICMPv6_RPL_CONTROL_MSG";
            return nullptr;
        case 3:
            if (!strcmp(propertyname,"enum")) return "inet::RPL_OPTIONS";
            return nullptr;
        default: return nullptr;
    }
}

int ICMPv6DAOMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DAOMsg *pp = (ICMPv6DAOMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ICMPv6DAOMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DAOMsg *pp = (ICMPv6DAOMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ICMPv6DAOMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DAOMsg *pp = (ICMPv6DAOMsg *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getCode(), "inet::ICMPv6_RPL_CONTROL_MSG");
        case 1: return long2string(pp->getD());
        case 2: {std::stringstream out; out << pp->getDODAGID(); return out.str();}
        case 3: return enum2string(pp->getOptions(), "inet::RPL_OPTIONS");
        default: return "";
    }
}

bool ICMPv6DAOMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DAOMsg *pp = (ICMPv6DAOMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setCode((inet::ICMPv6_RPL_CONTROL_MSG)string2enum(value, "inet::ICMPv6_RPL_CONTROL_MSG")); return true;
        case 1: pp->setD(string2long(value)); return true;
        case 3: pp->setOptions((inet::RPL_OPTIONS)string2enum(value, "inet::RPL_OPTIONS")); return true;
        default: return false;
    }
}

const char *ICMPv6DAOMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 2: return omnetpp::opp_typename(typeid(IPv6Address));
        default: return nullptr;
    };
}

void *ICMPv6DAOMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ICMPv6DAOMsg *pp = (ICMPv6DAOMsg *)object; (void)pp;
    switch (field) {
        case 2: return (void *)(&pp->getDODAGID()); break;
        default: return nullptr;
    }
}

} // namespace inet

