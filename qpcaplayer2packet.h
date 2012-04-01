#ifndef PCAPLAYER2PACKET_H
#define PCAPLAYER2PACKET_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QtDebug>
#include <QByteArray>

class LIBQPCAPSHARED_EXPORT QPcapLayer2PacketData : public QSharedData {
protected:
  quint16 _layer2Proto;
  quint16 _layer3Proto;
  QByteArray _payload;

public:
  inline explicit QPcapLayer2PacketData(
      quint16 layer2Proto = 0, quint16 layer3Proto = 0,
      QByteArray payload = QByteArray()) : _layer2Proto(layer2Proto),
    _layer3Proto(layer3Proto), _payload(payload) { }
  inline QPcapLayer2PacketData(const QPcapLayer2PacketData &other)
    : QSharedData(), _layer2Proto(other._layer2Proto),
      _layer3Proto(other._layer3Proto), _payload(other._payload) { }
  inline quint16 layer2Proto() const { return _layer2Proto; }
  inline quint16 layer3Proto() const { return _layer3Proto; }
  inline QByteArray payload() const { return _payload; }
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
  inline quint16 layer2Proto() const { return d->layer2Proto(); }
  inline quint16 layer3Proto() const { return d->layer3Proto(); }
  inline QByteArray payload() const { return d->payload(); }
  inline bool isNull() const { return payload().isNull(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const QPcapLayer2Packet &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPLAYER2PACKET_H
