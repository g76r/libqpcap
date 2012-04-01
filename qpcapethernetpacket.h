#ifndef PCAPETHERNETPACKET_H
#define PCAPETHERNETPACKET_H

#include "qpcap_global.h"
#include "qpcaplayer1packet.h"
#include "qpcaplayer2packet.h"

class LIBQPCAPSHARED_EXPORT QPcapEthernetPacketData : public QPcapLayer2PacketData {
protected:
  quint8 _dst[6];
  quint8 _src[6];

public:
  inline explicit QPcapEthernetPacketData(const QPcapLayer1Packet &packet)
    : QPcapLayer2PacketData(QPcapLayer2Packet::EthernetII) {
    int size = packet.payload().size();
    const quint8 *data = (const quint8 *)packet.payload().constData();
    if (size < 14) {
      ::memset(_dst, 0 , 6);
      ::memset(_src, 0 , 6);
    } else {
      ::memcpy(_dst, data, 6);
      ::memcpy(_src, data+6, 6);
      _layer3Proto = (data[12] << 8) + data[13];
      _payload = QByteArray((const char *)data+14, size-14);
      // LATER handle LLC frames
    }
  }
  inline QPcapEthernetPacketData(const QPcapEthernetPacketData &other)
    : QPcapLayer2PacketData(other.layer2Proto(), other.layer3Proto(),
                           other._payload) {
    ::memcpy(_dst, other._dst, 6);
    ::memcpy(_src, other._src, 6);
  }
  inline QString dst() const {
    return QString("%1:%2:%3:%4:%5:%6").arg(_dst[0], 2, 16, QLatin1Char('0'))
        .arg(_dst[1], 2, 16, QLatin1Char('0'))
        .arg(_dst[2], 2, 16, QLatin1Char('0'))
        .arg(_dst[3], 2, 16, QLatin1Char('0'))
        .arg(_dst[4], 2, 16, QLatin1Char('0'))
        .arg(_dst[5], 2, 16, QLatin1Char('0'));
  }
  inline QString src() const {
    return QString("%1:%2:%3:%4:%5:%6").arg(_src[0], 2, 16, QLatin1Char('0'))
        .arg(_src[1], 2, 16, QLatin1Char('0'))
        .arg(_src[2], 2, 16, QLatin1Char('0'))
        .arg(_src[3], 2, 16, QLatin1Char('0'))
        .arg(_src[4], 2, 16, QLatin1Char('0'))
        .arg(_src[5], 2, 16, QLatin1Char('0'));
  }
  QString english() const;
};

class LIBQPCAPSHARED_EXPORT QPcapEthernetPacket : public QPcapLayer2Packet {
public:
  inline explicit QPcapEthernetPacket(const QPcapLayer1Packet &packet)
    : QPcapLayer2Packet(new QPcapEthernetPacketData(packet)) { }
  inline QPcapEthernetPacket(const QPcapEthernetPacket &other)
    : QPcapLayer2Packet(other) { }
  inline QString dst() const { return static_cast<const QPcapEthernetPacketData*>(d.constData())->dst(); }
  inline QString src() const { return static_cast<const QPcapEthernetPacketData*>(d.constData())->src(); }
};

#endif // PCAPETHERNETPACKET_H
