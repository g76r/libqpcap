#include "qpcapipv4stack.h"
#include "qpcapethernetstack.h"

QPcapIPv4Stack::QPcapIPv4Stack(QObject *parent, QPcapEthernetStack *stack)
  : QObject(parent) {
  connect(stack, SIGNAL(layer2PacketReceived(QPcapLayer2Packet)),
          this, SLOT(layer2PacketReceived(QPcapLayer2Packet)));
  connect(stack, SIGNAL(captureStarted()), this, SIGNAL(captureStarted()));
  connect(stack, SIGNAL(captureFinished()), this, SIGNAL(captureFinished()));
}

void QPcapIPv4Stack::layer2PacketReceived(QPcapLayer2Packet packet) {
  if (packet.layer3Proto() != 0x800) { // ignore non-IPv4 packets
    //qDebug() << "ignoring non-IPv4 packet with protocol #"
    //         << QString("0x%1")
    //            .arg(QString::number(packet.layer3Proto(), 16));
    return;
  }
  QPcapIPv4Packet ip(packet);
  //qDebug() << "received IPv4 packet" << packet;
  /*if (!ip.dontFragment() && ip.layer4Proto() == QPcapIPv4Packet::TCP)
    qDebug() << "found TCP packet without don't fragment' flag"
             << ip.src() << ip.dst() << ip.payload().size(); */
  if (ip.moreFragments() || ip.fragmentOffset()) {
    // TODO handle fragmented packets
    qDebug() << "ignoring fragmented IPv4 packet fragment"
             << (ip.fragmentOffset() == 0
                 ? "first" : (ip.moreFragments() ? "middle" : "last" ))
             << ip.fragmentOffset() << ip.moreFragments()
             << ip.src() << ip.dst() << ip.identification() << ip.layer4Proto();
  } else {
    emit layer3PacketReceived(ip);
    emit ipv4PacketReceived(ip);
  }
}
