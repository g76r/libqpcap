#ifndef PCAPTCPCONVERSATION_H
#define PCAPTCPCONVERSATION_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QtDebug>
#include "qpcaptcppacket.h"
#include <QList>

class QPcapTcpConversation;

class LIBQPCAPSHARED_EXPORT QPcapTcpConversationData : public QSharedData {
  friend class QPcapTcpConversation;
private:
  quint64 _id;
  QPcapTcpPacket _firstPacket;
  quint32 _nextUpstreamNumber;
  quint32 _nextDownstreamNumber;
  bool _upstreamNumbersInitialized, _downstreamNumbersInitialized;
  QList<QPcapTcpPacket> _packets;

public:
  QPcapTcpConversationData(QPcapTcpPacket firstPacket = QPcapTcpPacket());
  inline QPcapTcpConversationData(const QPcapTcpConversationData &other)
    : QSharedData(), _id(other._id), _firstPacket(other._firstPacket),
      _nextUpstreamNumber(other._nextUpstreamNumber),
      _nextDownstreamNumber(other._nextDownstreamNumber),
      _upstreamNumbersInitialized(other._upstreamNumbersInitialized),
      _downstreamNumbersInitialized(other._downstreamNumbersInitialized),
      _packets(other._packets) { }
};

class LIBQPCAPSHARED_EXPORT QPcapTcpConversation {
private:
  QExplicitlySharedDataPointer<QPcapTcpConversationData> d;

public:
  inline explicit QPcapTcpConversation(
      QPcapTcpPacket firstPacket = QPcapTcpPacket()) {
    d = new QPcapTcpConversationData(firstPacket); }
  inline QPcapTcpConversation(const QPcapTcpConversation &other) : d(other.d) { }
  inline quint64 id() const { return d->_id; }
  inline QPcapTcpPacket firstPacket() const { return d->_firstPacket; }
  inline QString toText() const {
    return QString("PcapTcpConversation(%1, %2, %3)").arg(d->_id)
        .arg(d->_firstPacket.src()).arg(d->_firstPacket.dst()); }
  inline bool matchesEitherStream(QPcapTcpPacket packet) {
    return (d->_firstPacket.srcPort() == packet.srcPort()
            && d->_firstPacket.dstPort() == packet.dstPort()
            && d->_firstPacket.ip().srcAsInt() == packet.ip().srcAsInt()
            && d->_firstPacket.ip().dstAsInt() == packet.ip().dstAsInt())
        || (d->_firstPacket.dstPort() == packet.srcPort()
            && d->_firstPacket.srcPort() == packet.dstPort()
            && d->_firstPacket.ip().srcAsInt() == packet.ip().dstAsInt()
            && d->_firstPacket.ip().dstAsInt() == packet.ip().srcAsInt()); }
  inline bool matchesSameStream(QPcapTcpPacket packet) {
    return d->_firstPacket.srcPort() == packet.srcPort()
        && d->_firstPacket.dstPort() == packet.dstPort()
        && d->_firstPacket.ip().srcAsInt() == packet.ip().srcAsInt()
        && d->_firstPacket.ip().dstAsInt() == packet.ip().dstAsInt(); }
  inline quint32 &nextUpstreamNumber() { return d->_nextUpstreamNumber; }
  inline quint32 &nextDownstreamNumber() { return d->_nextDownstreamNumber; }
  inline bool &upstreamNumbersInitialized() { return d->_upstreamNumbersInitialized; }
  inline bool &downstreamNumbersInitialized() { return d->_downstreamNumbersInitialized; }
  inline bool isNull() { return !id(); }
  inline QList<QPcapTcpPacket> &packets() { return d->_packets; }
  inline bool operator ==(const QPcapTcpConversation &other) const {
    return d == other.d; }
  inline bool operator <(const QPcapTcpConversation &other) const {
    return id() < other.id(); }
  static void resetConversationCounter();
};

inline QDebug operator<<(QDebug dbg, const QPcapTcpConversation &o) {
  dbg.nospace() << o.toText();
  return dbg.space();
}

inline uint qHash(const QPcapTcpConversation &o) {
  return o.id();
}

#endif // PCAPTCPCONVERSATION_H
