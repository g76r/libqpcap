#ifndef PCAPLAYER2PACKET_H
#define PCAPLAYER2PACKET_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QtDebug>
#include <QByteArray>

class QPcapLayer2Packet;

class LIBQPCAPSHARED_EXPORT QPcapLayer2PacketData : public QSharedData {
  friend class QPcapLayer2Packet;
protected:
  quint64 _timestamp; // in microseconds since 1970
  quint16 _layer2Proto;
  quint16 _layer3Proto;
  QByteArray _payload;

public:
  inline explicit QPcapLayer2PacketData(
      quint64 timestamp = 0, quint16 layer2Proto = 0, quint16 layer3Proto = 0,
      QByteArray payload = QByteArray()) : _timestamp(timestamp),
    _layer2Proto(layer2Proto), _layer3Proto(layer3Proto), _payload(payload) { }
  inline QPcapLayer2PacketData(const QPcapLayer2PacketData &other)
    : QSharedData(), _timestamp(other._timestamp),
      _layer2Proto(other._layer2Proto), _layer3Proto(other._layer3Proto),
      _payload(other._payload) { }
  virtual QString english() const;
};

class LIBQPCAPSHARED_EXPORT QPcapLayer2Packet {
protected:
  QSharedDataPointer<QPcapLayer2PacketData> d;
  explicit inline QPcapLayer2Packet(QPcapLayer2PacketData *data) {
    d = data ? data : new QPcapLayer2PacketData(); }

public:
  enum Layer2Proto { Unknown = 0, EthernetII };

  explicit inline QPcapLayer2Packet() : d(new QPcapLayer2PacketData()) { }
  inline QPcapLayer2Packet(const QPcapLayer2Packet &other) : d(other.d) { }
  inline quint64 timestamp() const { return d->_timestamp; }
  inline quint16 layer2Proto() const { return d->_layer2Proto; }
  inline quint16 layer3Proto() const { return d->_layer3Proto; }
  inline QByteArray payload() const { return d->_payload; }
  inline bool isNull() const { return payload().isNull(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const QPcapLayer2Packet &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPLAYER2PACKET_H
