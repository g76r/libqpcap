#include "qpcapethernetstack.h"
#include <QtDebug>

void QPcapEthernetStack::layer1PacketReceived(QPcapLayer1Packet packet) {
  QPcapEthernetPacket ether(packet);
  if (!ether.isNull())
    emit layer2PacketReceived(ether);
  //else
  //  qDebug() << "ignoring non-ethernet packet" << packet;
}
