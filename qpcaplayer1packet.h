#ifndef QPCAPLAYER1PACKET_H
#define QPCAPLAYER1PACKET_H

#if defined(QPCAPLAYER1PACKET_CPP) || defined(lib_pcap_pcap_h)
#include <pcap/pcap.h>
#else
#define pcap_t void
typedef unsigned char u_char;
struct pcap_pkthdr {
};
#endif
#include "qpcap_global.h"
#include <QSharedData>
#include <QtDebug>
#include <QByteArray>
#include <QDateTime>

class QPcapEngine;

class LIBQPCAPSHARED_EXPORT QPcapLayer1PacketData : public QSharedData {
private:
  quint64 _timestamp; // in microseconds since 1970
  quint32 _wirelen;
  QByteArray _payload;

public:
  inline QPcapLayer1PacketData() : _timestamp(0), _wirelen(0) { }
  inline QPcapLayer1PacketData(const QPcapLayer1PacketData &other)
    : QSharedData(), _timestamp(other._timestamp), _wirelen(other._wirelen),
      _payload(other._payload) {
  }
  QPcapLayer1PacketData(const struct pcap_pkthdr* pkthdr,
                        const u_char* packet);
  inline quint64 usecSince1970() const { return _timestamp; }
  inline QDateTime timestamp() const {
    return QDateTime::fromMSecsSinceEpoch(_timestamp/1000);
  }
  inline quint32 wirelen() const { return _wirelen; }
  inline QByteArray payload() const { return _payload; }
  inline QString english() const {
    return QString("PcapPacket(%1, %2, %3, %4)")
        .arg(timestamp().time().toString("HH:mm:ss,zzz")).arg(_wirelen)
        .arg(_payload.size()).arg(_payload.toHex().constData());
  }
};

class LIBQPCAPSHARED_EXPORT QPcapLayer1Packet {
  friend class QPcapEngine;
private:
  QSharedDataPointer<QPcapLayer1PacketData> d;

  inline QPcapLayer1Packet(const struct pcap_pkthdr* pkthdr,
                           const u_char* packet) {
    d = new QPcapLayer1PacketData(pkthdr, packet);
  }

public:
  inline QPcapLayer1Packet() { d = new QPcapLayer1PacketData(); }
  inline QPcapLayer1Packet(const QPcapLayer1Packet &other) : d(other.d) { }
  inline QDateTime timestamp() const { return d->timestamp(); }
  inline quint64 usecSince1970() const { return d->usecSince1970(); }
  inline quint32 wirelen() const { return d->wirelen(); }
  inline QByteArray payload() const { return d->payload(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const QPcapLayer1Packet &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // QPCAPLAYER1PACKET_H
