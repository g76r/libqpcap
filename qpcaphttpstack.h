#ifndef QPCAPHTTPSTACK_H
#define QPCAPHTTPSTACK_H

#include "qpcap_global.h"
#include <QObject>
#include "qpcaptcpconversation.h"
#include "qpcaptcppacket.h"
#include "qpcaphttphit.h"
#include <QHash>
#include <QRegExp>
#include "qpcaptcpstack.h"

class LIBQPCAPSHARED_EXPORT QPcapHttpStack : public QObject {
  Q_OBJECT
public:
  enum QPcapHttpDirection { Upstream, Downstream, Anystream };

private:
  enum QPcapHttpState { AwaitingRequest, InRequest, Awaiting100Continue,
                        InResponse, TruncatedResponseBeforeRequest, NonHttp };

  class QPcapHttpConversation {
    friend class QPcapHttpStack;
  private:
    QPcapTcpConversation _tcp;
    quint64 _remaining;
    QPcapHttpState _state;
    bool _switched; // true if client to server == downstream
    QByteArray _buf;
    QPcapHttpHit _hit;
    QPcapHttpConversation(QPcapTcpConversation tcp) : _tcp(tcp),
      _remaining(0), _state(AwaitingRequest), _switched(false) { }
  };

  class QPcapHttpFilter {
    friend class QPcapHttpStack;
  private:
    QRegExp _re;
    QPcapHttpDirection _direction;
    int _captureRank;
  public:
    inline QPcapHttpFilter(
        QString regex = QString(), QPcapHttpDirection direction = Anystream,
        int captureRank = 1) : _re(regex), _direction(direction),
      _captureRank(captureRank) { }
    inline QPcapHttpFilter(const QPcapHttpFilter &other) : _re(other._re),
      _direction(other._direction), _captureRank(other._captureRank) { }
  };

  QHash<quint64, QPcapHttpConversation*> _conversations;
  QRegExp _requestRE, _headerRE, _100ContinueRE, _responseRE;
  QList<QPcapHttpFilter> _filters;

public:
  explicit QPcapHttpStack(QObject *parent = 0);
  void connectToLowerStack(QPcapTcpStack &stack);
  inline void addFilter(QString regex, QPcapHttpDirection direction = Anystream,
                        int captureRank = 1) {
    _filters.append(QPcapHttpFilter(regex, direction, captureRank));
  }
  inline void clearFilters() { _filters.clear(); }

signals:
  /** Each time a hit is detected and fully qualified (i.e. request is
    * terminated therefore all timestamps are known).
    */
  void httpHit(QPcapHttpHit hit);
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
