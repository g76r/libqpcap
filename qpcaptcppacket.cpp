#include "qpcaptcppacket.h"

QPcapTcpPacketData::QPcapTcpPacketData(QPcapIPv4Packet packet) : _ip(packet) {
  if (packet.layer4Proto() != QPcapIPv4Packet::TCP) { // ignore non-TCP packets
    reset();
    return;
  }
  int size = packet.payload().size();
  const quint8 *data = (const quint8 *)packet.payload().constData();
  //qDebug() << "  QPcapTcpPacketData" << size << packet;
  if (size < 20) {
    qDebug() << "tcp packet too short" << packet;
    reset();
    return;
  }
  _srcPort = 0x100*data[0] + data[1];
  _dstPort = 0x100*data[2] + data[3];
  _seqNumber = 0x1000000*data[4] + 0x10000*data[5] + 0x100*data[6] + data[7];
  _ackNumber = 0x1000000*data[8] + 0x10000*data[9] + 0x100*data[10] + data[11];
  _headerSize = data[12] >> 4;
  // ignoring 4 reserved bits
  _ecn = data[13] >> 6;
  _urg = data[13] & 0x20;
  _ack = data[13] & 0x10;
  _psh = data[13] & 0x8;
  _rst = data[13] & 0x4;
  _syn = data[13] & 0x2;
  _fin = data[13] & 0x1;
  _windowSize = 0x100*data[14] + data[15];
  _checksum = 0x100*data[16] + data[17];
  _urgentPointer = 0x100*data[18] + data[19];
  _payload = QByteArray((const char *)data+_headerSize*4, size-_headerSize*4);
}


QString QPcapTcpPacketData::english() const {
  return QString("QPcapTcpPacket(%1, %2, %3, %4, %5)").arg(src()).arg(dst())
      .arg(_seqNumber).arg(syn() || rst() || fin()).arg(payload().size());
}
