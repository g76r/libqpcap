#include "qpcapipv4packet.h"

QPcapIPv4PacketData::QPcapIPv4PacketData(const QPcapLayer2Packet &packet)
  : QPcapLayer3PacketData(packet.timestamp(), 0x800) {
  int layer2DataSize = packet.payload().size();
  const quint8 *layer2Data = (const quint8 *)packet.payload().constData();
  if (layer2DataSize < 20) {
    reset();
    return;
  }
  _version = layer2Data[0] >> 4;
  _headerSize = layer2Data[0] & 0xf;
  _tos = layer2Data[1];
  _totalSize = 0x100 * layer2Data[2] + layer2Data[3];
  _identification = 0x100 * layer2Data[4] + layer2Data[5];
  _unusedFlag = layer2Data[6] & 0x80;
  _dontFragment = layer2Data[6] & 0x40;
  _moreFragments = layer2Data[6] & 0x20;
  _fragmentOffset = 0x100 * (layer2Data[6] & 0x1f) + layer2Data[7];
  if (_fragmentOffset || _moreFragments) {
    qDebug() << "parsing fragmented IP packet" << layer2Data[6] << layer2Data[7]
             << _fragmentOffset << _moreFragments
             << packet.layer2Proto() << packet.toText();
  }
  _ttl = layer2Data[8];
  _layer4Proto = layer2Data[9];
  _headerChecksum = (layer2Data[10] << 8) + layer2Data[11];
  ::memcpy(_src, layer2Data+12, 4);
  ::memcpy(_dst, layer2Data+16, 4);
  //qDebug() << "QPcapIPv4Packet" << _totalSize << _headerSize
  //         << _version;
  if (layer2DataSize < _headerSize*4 || _headerSize*4 < 20) {
    reset();
    return;
  }
  _payload = QByteArray((const char *)layer2Data + _headerSize*4,
                        std::min(layer2DataSize, (int)_totalSize)
                        - _headerSize*4);
}

QString QPcapIPv4PacketData::toText() const {
  return QString("QPcapIPv4Packet(%1, %2, %3, %4, %5)")
      .arg(src()).arg(dst()).arg(layer4Proto())
      .arg(_payload.size()).arg(_payload.toHex().constData());
}

