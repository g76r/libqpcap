#include "qpcapethernetstack.h"
#include <QtDebug>

void QPcapEthernetStack::layer1PacketReceived(QPcapLayer1Packet packet) {
  QPcapEthernetPacket ether(packet);
  if (ether.isNull())
    qDebug() << "ignoring non-ethernet packet" << packet;
  else
    emit layer2PacketReceived(ether);
}
