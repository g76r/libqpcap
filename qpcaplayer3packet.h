#ifndef PCAPLAYER3PACKET_H
#define PCAPLAYER3PACKET_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QtDebug>
#include <QByteArray>

class LIBQPCAPSHARED_EXPORT QPcapLayer3PacketData : public QSharedData {
protected:
  quint16 _layer3Proto;
  QByteArray _payload;

public:
  inline explicit QPcapLayer3PacketData(
      quint16 proto = 0, QByteArray payload = QByteArray())
    : _layer3Proto(proto), _payload(payload) { }
  inline QPcapLayer3PacketData(const QPcapLayer3PacketData &other)
    : QSharedData(), _layer3Proto(other._layer3Proto),
      _payload(other._payload) { }
  inline quint16 layer3Proto() const { return _layer3Proto; }
  inline QByteArray payload() const { return _payload; }
  virtual QString english() const;
};

class LIBQPCAPSHARED_EXPORT QPcapLayer3Packet {
protected:
  QSharedDataPointer<QPcapLayer3PacketData> d;
  explicit inline QPcapLayer3Packet(QPcapLayer3PacketData *data) {
    d = data ? data : new QPcapLayer3PacketData(); }

public:
  enum Layer3Proto { Unknown = 0, IPv4 = 0x800, ARP = 0x806, AppleTalk = 0x809b,
                     AppleTalkARP = 0x80f3, VLAN = 0x8100, IPX = 0x8137,
                     IPv6 = 0x86dd };

  explicit inline QPcapLayer3Packet() : d(new QPcapLayer3PacketData()) { }
  inline QPcapLayer3Packet(const QPcapLayer3Packet &other) : d(other.d) { }
  inline quint16 layer3Proto() const { return d->layer3Proto(); }
  inline QByteArray payload() const { return d->payload(); }
  inline bool isNull() const { return payload().isNull(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const QPcapLayer3Packet &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPLAYER3PACKET_H
