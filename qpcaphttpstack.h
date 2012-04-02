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
  enum QPcapHttpState { Ready, InRequest, InResponse, NonHttp };

  class QPcapHttpConversation {
    friend class QPcapHttpStack;
  private:
    quint64 _id, _remaining;
    QPcapHttpState _state;
    bool _switched, _expect100Continue;
    QByteArray _buf;
    QPcapHttpHit _hit;
    QPcapHttpConversation(quint64 id) : _id(id), _remaining(0), _state(Ready),
      _switched(false), _expect100Continue(false) { }
  };

  QHash<quint64, QPcapHttpConversation*> _conversations;
  QRegExp _requestRE, _headerRE;

public:
  explicit QPcapHttpStack(QObject *parent = 0);

signals:
  void httpHit(QPcapTcpConversation conversation, QPcapHttpHit hit);
  
public slots:
  void conversationStarted(QPcapTcpConversation conversation);
  void tcpUpstreamPacket(QPcapTcpPacket packet,
                         QPcapTcpConversation conversation);
  void tcpDownstreamPacket(QPcapTcpPacket packet,
                           QPcapTcpConversation conversation);
  void conversationFinished(QPcapTcpConversation conversation);

private:
  void hasRequestPacket(QPcapTcpPacket packet, QPcapHttpConversation *c);
  void hasResponsePacket(QPcapTcpPacket packet, QPcapHttpConversation *c);
};

#endif // QPCAPHTTPSTACK_H
