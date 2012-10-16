#include "qpcapipv4portconversationsignature.h"
#include "qpcaptcppacket.h"

QPcapIPv4PortConversationSignature::QPcapIPv4PortConversationSignature(
    const QPcapTcpPacket tcpPacket) {
  quint64 *data = new quint64[2];
  if (tcpPacket.srcPort() < tcpPacket.dstPort()) {
    data[0] = (tcpPacket.ip().srcAsInt() << 16) + tcpPacket.srcPort();
    data[1] = (tcpPacket.ip().dstAsInt() << 16) + tcpPacket.dstPort();
  } else {
    data[0] = (tcpPacket.ip().dstAsInt() << 16) + tcpPacket.dstPort();
    data[1] = (tcpPacket.ip().srcAsInt() << 16) + tcpPacket.srcPort();
  }
  d = QSharedPointer<QPcapIPv4PortConversationSignatureData>(
        new QPcapIPv4PortConversationSignatureData(data));
}

uint QPcapIPv4PortConversationSignature::hashCode() const {
  // TODO take address into account, not only ports
  return ((d->_data[0] & 0xffff) << 16) | (d->_data[1] & 0xffff);
}
