#ifndef PCAPETHERNETSTACK_H
#define PCAPETHERNETSTACK_H

#include "qpcap_global.h"
#include <QObject>
#include "qpcapethernetpacket.h"
#include "qpcaplayer1packet.h"

class QPcapEngine;

class LIBQPCAPSHARED_EXPORT QPcapEthernetStack : public QObject {
  Q_OBJECT
public:
  explicit QPcapEthernetStack(QObject *parent, QPcapEngine *engine);

signals:
  /** Emititng layer 2 packet.
    */
  void layer2PacketReceived(QPcapLayer2Packet packet);
  //void ethernetPacketReceived(PcapEthernetPacket packet);
  void captureStarted();
  void captureFinished();

public slots:
  /** Receiving layer 1 packet, potentially not ethernet.
    */
  void layer1PacketReceived(QPcapLayer1Packet packet);

private:
  Q_DISABLE_COPY(QPcapEthernetStack)
 };

#endif // PCAPETHERNETSTACK_H
