//
// Generated file, do not edit! Created by nedtool 5.4 from messages/UDPControlInfo.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __UDPCONTROLINFO_M_H
#define __UDPCONTROLINFO_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "../objects/IPvXAddress.h"
// }}

/**
 * Enum generated from <tt>messages/UDPControlInfo.msg:34</tt> by nedtool.
 * <pre>
 * //
 * // \UDP command codes, sent by the application to UDP. These constants
 * // should be set as message kind on a message sent to the UDP entity.
 * //
 * // \@see UDPControlInfo, UDPStatusInd, UDP
 * //
 * enum UDPCommandCode
 * {
 *     UDP_C_DATA = 0;    // datagram to send (must carry UDPControlInfo)
 *     UDP_C_BIND = 1;    // fill srcPort and srcAddr in UDPControlInfo
 *     UDP_C_CONNECT = 2; // fill destAddr and destPort in UDPControlInfo
 *     UDP_C_UNBIND = 3;  // fill sockId in UDPControlInfo
 * }
 * </pre>
 */
enum UDPCommandCode {
    UDP_C_DATA = 0,
    UDP_C_BIND = 1,
    UDP_C_CONNECT = 2,
    UDP_C_UNBIND = 3
};

/**
 * Enum generated from <tt>messages/UDPControlInfo.msg:49</tt> by nedtool.
 * <pre>
 * //
 * // \UDP indications, sent by UDP to the application. UDP will set these
 * // constants as message kind on messages it sends to the application.
 * //
 * // \@see UDPControlInfo, UDPCommandCode, UDP
 * //
 * enum UDPStatusInd
 * {
 *     UDP_I_DATA = 0;  // data packet (set on data packet)
 *     UDP_I_ERROR = 1; // ICMP error received on a sent datagram
 * }
 * </pre>
 */
enum UDPStatusInd {
    UDP_I_DATA = 0,
    UDP_I_ERROR = 1
};

/**
 * Class generated from <tt>messages/UDPControlInfo.msg:70</tt> by nedtool.
 * <pre>
 * //
 * // Control info for sending/receiving data via \UDP. Travels between
 * // application and the UDP module.
 * //
 * // The most convenient way to handle UDP is the UDPSocket class, which hides
 * // UDPControlInfo completely from you. But even if you manage talking
 * // to UDP yourself without help from UDPSocket, sockId must be obtained
 * // from UDPSocket::generateSocketId().
 * //
 * //# TODO explain userId, sockId, etc.
 * //
 * // \@see UDPCommandCode
 * //
 * class UDPControlInfo
 * {
 *     int sockId = -1;   // uniquely identifies the \UDP socket
 *     int userId = -1;   // id than can be freely used by the app
 *     IPvXAddress srcAddr;  // source \IP or \IPv6 address, or local address with BIND
 *     IPvXAddress destAddr; // destination \IP or \IPv6 address
 *     int srcPort;   // \UDP source port in packet, or local port with BIND
 *     int destPort;  // \UDP destination port in packet
 *     int interfaceId = -1; // interface on which pk was received/should be sent (see InterfaceTable)
 * }
 * </pre>
 */
class UDPControlInfo : public ::omnetpp::cObject
{
  protected:
    int sockId;
    int userId;
    IPvXAddress srcAddr;
    IPvXAddress destAddr;
    int srcPort;
    int destPort;
    int interfaceId;

  private:
    void copy(const UDPControlInfo& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const UDPControlInfo&);

  public:
    UDPControlInfo();
    UDPControlInfo(const UDPControlInfo& other);
    virtual ~UDPControlInfo();
    UDPControlInfo& operator=(const UDPControlInfo& other);
    virtual UDPControlInfo *dup() const override {return new UDPControlInfo(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSockId() const;
    virtual void setSockId(int sockId);
    virtual int getUserId() const;
    virtual void setUserId(int userId);
    virtual IPvXAddress& getSrcAddr();
    virtual const IPvXAddress& getSrcAddr() const {return const_cast<UDPControlInfo*>(this)->getSrcAddr();}
    virtual void setSrcAddr(const IPvXAddress& srcAddr);
    virtual IPvXAddress& getDestAddr();
    virtual const IPvXAddress& getDestAddr() const {return const_cast<UDPControlInfo*>(this)->getDestAddr();}
    virtual void setDestAddr(const IPvXAddress& destAddr);
    virtual int getSrcPort() const;
    virtual void setSrcPort(int srcPort);
    virtual int getDestPort() const;
    virtual void setDestPort(int destPort);
    virtual int getInterfaceId() const;
    virtual void setInterfaceId(int interfaceId);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const UDPControlInfo& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, UDPControlInfo& obj) {obj.parsimUnpack(b);}


#endif // ifndef __UDPCONTROLINFO_M_H

