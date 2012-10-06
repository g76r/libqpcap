#ifndef PCAPTCPSTACK_H
#define PCAPTCPSTACK_H

#include "qpcap_global.h"
#include <QObject>
#include <QByteArray>
#include "qpcaptcpconversation.h"
#include "qpcaptcppacket.h"
#include <QMultiMap>
#include "qpcapipv4portconversationsignature.h"

class QPcapIPv4Stack;

class LIBQPCAPSHARED_EXPORT QPcapTcpStack : public QObject {
  Q_OBJECT
private:
  QMap<QPcapIPv4PortConversationSignature,QPcapTcpConversation> _conversations;
  QMultiMap<QPcapTcpConversation, QPcapTcpPacket> _upstreamBuffer;
  QMultiMap<QPcapTcpConversation, QPcapTcpPacket> _downstreamBuffer;
  unsigned long _packetsCount;

public:
  explicit QPcapTcpStack(QObject *parent, QPcapIPv4Stack *stack);

signals:
  /** Emitting ordered data stream chunk.
    */
  void tcpPacket(QPcapTcpPacket packet, QPcapTcpConversation conversation);
  void conversationStarted(QPcapTcpConversation conversation);
  void conversationFinished(QPcapTcpConversation conversation);
  void packetsCountTick(unsigned long count);
  void captureStarted();
  void captureFinished();

public slots:
  /** Receiving IP packet, potentially retransmitted or in wrong order.
    */
  void ipPacketReceived(QPcapIPv4Packet packet);
  /** Discard any upstream buffered packet and reset upstream sequence numbers.
    */
  void discardUpstreamBuffer(QPcapTcpConversation conversation);
  /** Discard any downstream buffered packet and reset downstream sequence
    * numbers.
    */
  void discardDownstreamBuffer(QPcapTcpConversation conversation);
  /** Discard any conversation or buffered packet.
    */

private slots:
  void starting();
  void finishing();

private:
  void dispatchPacket(QPcapTcpPacket packet, QPcapTcpConversation conversation);
  inline static quint64 higherPortPeer(QPcapTcpPacket packet);
  inline static quint64 lowerPortPeer(QPcapTcpPacket packet);
  Q_DISABLE_COPY(QPcapTcpStack)
};

#endif // PCAPTCPSTACK_H
