#include "qpcapethernetstack.h"
#include <QtDebug>
#include "qpcapengine.h"

QPcapEthernetStack::QPcapEthernetStack(QObject *parent, QPcapEngine *engine)
  : QObject(parent) {
  connect(engine, SIGNAL(layer1PacketReceived(QPcapLayer1Packet)),
          this, SLOT(layer1PacketReceived(QPcapLayer1Packet)));
  connect(engine, SIGNAL(captureStarted()), this, SIGNAL(captureStarted()));
  connect(engine, SIGNAL(captureFinished()), this, SIGNAL(captureFinished()));
}

void QPcapEthernetStack::layer1PacketReceived(QPcapLayer1Packet packet) {
  QPcapEthernetPacket ether(packet);
  //qDebug() << "QPcapEthernetStack::layer1PacketReceived" << packet.timestamp();
  if (!ether.isNull())
    emit layer2PacketReceived(ether);
  //else
  //  qDebug() << "ignoring non-ethernet packet" << packet;
}
