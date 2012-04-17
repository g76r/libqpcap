#include "qpcapethernetpacket.h"

QString QPcapEthernetPacketData::toText() const {
  return QString("PcapEthernetPacket(%1, %2, 0x%3, %4, %5)").arg(dst()).arg(src())
      .arg(QString::number(_layer3Proto, 16))
      .arg(_payload.size()).arg(_payload.toHex().constData());
}

