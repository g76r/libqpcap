#ifndef PCAPTCPSTACK_H
#define PCAPTCPSTACK_H

#include "qpcap_global.h"
#include <QObject>
#include <QByteArray>
#include "qpcaptcpconversation.h"
#include "qpcaptcppacket.h"
#include <QMultiMap>

class LIBQPCAPSHARED_EXPORT QPcapTcpStack : public QObject {
  Q_OBJECT
private:
  QSet<QPcapTcpConversation> _conversations;
  QMultiMap<QPcapTcpConversation, QPcapTcpPacket> _upstreamBuffer;
  QMultiMap<QPcapTcpConversation, QPcapTcpPacket> _downstreamBuffer;

public:
  explicit QPcapTcpStack(QObject *parent = 0) : QObject(parent) { }

signals:
  /** Emitting ordered data stream chunk, client to server.
    * Beware: if SYN/SYN-ACK exchange was previous capture start, client and
    * server are randomly chosen (actually: first seen packet source is client).
    */
  void tcpUpstreamPacket(QPcapTcpPacket packet,
                         QPcapTcpConversation conversation);
  /** Emitting ordered data stream chunk, server to client.
    * Beware: if SYN/SYN-ACK exchange was previous capture start, client and
    * server are randomly chosen (actually: first seen packet source is client).
    */
  void tcpDownstreamPacket(QPcapTcpPacket packet,
                           QPcapTcpConversation conversation);

  void conversationStarted(QPcapTcpConversation conversation);
  void conversationFinished(QPcapTcpConversation conversation);

public slots:
  /** Receiving IP packet, potentially retransmitted or in wrong order.
    */
  void ipPacketReceived(QPcapIPv4Packet packet);

private:
  void dispatchPacket(QPcapTcpPacket packet, QPcapTcpConversation conversation);

  Q_DISABLE_COPY(QPcapTcpStack)
};

#endif // PCAPTCPSTACK_H
