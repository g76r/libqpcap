#ifndef PCAPLAYER3PACKET_H
#define PCAPLAYER3PACKET_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QtDebug>
#include <QByteArray>

class QPcapLayer3Packet;

class LIBQPCAPSHARED_EXPORT QPcapLayer3PacketData : public QSharedData {
  friend class QPcapLayer3Packet;
protected:
  quint16 _layer3Proto;
  QByteArray _payload;
  quint64 _timestamp; // in microseconds since 1970

public:
  inline explicit QPcapLayer3PacketData(
      quint64 timestamp = 0,  quint16 proto = 0,
      QByteArray payload = QByteArray())
    : _layer3Proto(proto), _payload(payload), _timestamp(timestamp) { }
  inline QPcapLayer3PacketData(const QPcapLayer3PacketData &other)
    : QSharedData(), _layer3Proto(other._layer3Proto),
      _payload(other._payload), _timestamp(other._timestamp) { }
  virtual ~QPcapLayer3PacketData();
  virtual QString toText() const;
};

class LIBQPCAPSHARED_EXPORT QPcapLayer3Packet {
protected:
  QExplicitlySharedDataPointer<QPcapLayer3PacketData> d;

  explicit inline QPcapLayer3Packet(QPcapLayer3PacketData *data) {
    d = data ? data : new QPcapLayer3PacketData(); }

public:
  enum Layer3Proto { Unknown = 0, IPv4 = 0x800, ARP = 0x806, AppleTalk = 0x809b,
                     AppleTalkARP = 0x80f3, VLAN = 0x8100, IPX = 0x8137,
                     IPv6 = 0x86dd };

  explicit inline QPcapLayer3Packet() : d(new QPcapLayer3PacketData()) { }
  inline QPcapLayer3Packet(const QPcapLayer3Packet &other) : d(other.d) { }
  inline quint16 layer3Proto() const { return d->_layer3Proto; }
  inline QByteArray payload() const { return d->_payload; }
  inline bool isNull() const { return payload().isNull(); }
  inline QString toText() const { return d->toText(); }
  inline quint64 timestamp() const { return d->_timestamp; }
};

inline QDebug operator<<(QDebug dbg, const QPcapLayer3Packet &pp) {
  dbg.nospace() << pp.toText();
  return dbg.space();
}

#endif // PCAPLAYER3PACKET_H
