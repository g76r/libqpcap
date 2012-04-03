#ifndef QPCAPHTTPSTACK_H
#define QPCAPHTTPSTACK_H

#include "qpcap_global.h"
#include <QObject>
#include "qpcaptcpconversation.h"
#include "qpcaptcppacket.h"
#include "qpcaphttphit.h"
#include <QHash>
#include <QRegExp>

class LIBQPCAPSHARED_EXPORT QPcapHttpStack : public QObject {
  Q_OBJECT
private:
  enum QPcapHttpState { AwaitingRequest, InRequest, Awaiting100Continue,
                        InResponse, NonHttp };

  class QPcapHttpConversation {
    friend class QPcapHttpStack;
  private:
    QPcapTcpConversation _tcp;
    quint64 _remaining;
    QPcapHttpState _state;
    bool _switched; // true if client to server = downstream
    QByteArray _buf;
    QPcapHttpHit _hit;
    QPcapHttpConversation(QPcapTcpConversation tcp) : _tcp(tcp),
      _remaining(0), _state(AwaitingRequest), _switched(false) { }
  };

  QHash<quint64, QPcapHttpConversation*> _conversations;
  QRegExp _requestRE, _headerRE, _100ContinueRE, _responseRE;

public:
  explicit QPcapHttpStack(QObject *parent = 0);

signals:
  /** Each time a hit is detected and fully qualified (i.e. request is
    * terminated therefore all timestamps are known).
    */
  void httpHit(QPcapTcpConversation conversation, QPcapHttpHit hit);
  /** Should be connected to same name slot of QPcapTcpStack to recover
    * from some case of corrupted data in upstream flow.
    */
  void discardUpstreamBuffer(QPcapTcpConversation conversation);
  /** Should be connected to same name slot of QPcapTcpStack to recover
    * from some case of corrupted data in downstream flow.
    */
  void discardDownstreamBuffer(QPcapTcpConversation conversation);

public slots:
  void conversationStarted(QPcapTcpConversation conversation);
  void tcpUpstreamPacket(QPcapTcpPacket packet,
                         QPcapTcpConversation conversation);
  void tcpDownstreamPacket(QPcapTcpPacket packet,
                           QPcapTcpConversation conversation);
  void conversationFinished(QPcapTcpConversation conversation);

private:
  void hasTcpPacket(bool isUpstream, QPcapTcpPacket packet,
                    QPcapTcpConversation conversation);
  // following methods are called by tcpPacket() depending
  // on the conversation being swithed or not (i.e. client to server is
  // upstream or not).
  void hasRequestPacket(QPcapTcpPacket packet, QPcapHttpConversation *c);
  void hasResponsePacket(QPcapTcpPacket packet, QPcapHttpConversation *c);
  void has100ContinueResponsePacket(QPcapTcpPacket packet, QPcapHttpConversation *c);
};

#endif // QPCAPHTTPSTACK_H
