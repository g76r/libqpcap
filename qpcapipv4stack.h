#ifndef PCAPIPV4STACK_H
#define PCAPIPV4STACK_H

#include "qpcap_global.h"
#include <QObject>
#include "qpcapipv4packet.h"
#include "qpcaplayer2packet.h"

class QPcapEthernetStack;

class LIBQPCAPSHARED_EXPORT QPcapIPv4Stack : public QObject {
  Q_OBJECT
public:
  explicit QPcapIPv4Stack(QObject *parent, QPcapEthernetStack *stack);

signals:
  /** Emititng generic layer 3 defragmented packet.
    */
  void layer3PacketReceived(QPcapLayer3Packet packet);
  /** Emititng IPv4 defragmented packet.
    */
  void ipv4PacketReceived(QPcapIPv4Packet packet);
  void captureStarted();
  void captureFinished();

public slots:
  /** Receiving layer 2 packet, potentially fragmented.
    */
  void layer2PacketReceived(QPcapLayer2Packet packet);

private:
  Q_DISABLE_COPY(QPcapIPv4Stack)
};

#endif // PCAPIPV4STACK_H
