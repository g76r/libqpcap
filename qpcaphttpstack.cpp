#include "qpcaphttpstack.h"
#include <QtDebug>

QPcapHttpStack::QPcapHttpStack(QObject *parent) : QObject(parent),
  _requestRE("\\s*(GET|POST|HEAD)\\s+(\\S+)\\s+HTTP/1.[01]\\s*\r?"),
  _headerRE("(\\w+):\\s+(\\S*)\\s*\r?") {
}

void QPcapHttpStack::conversationStarted(QPcapTcpConversation conversation) {
  _conversations.insert(conversation.id(),
                        new QPcapHttpConversation(conversation.id()));
}

void QPcapHttpStack::tcpUpstreamPacket(QPcapTcpPacket packet,
                                       QPcapTcpConversation conversation) {
  if (packet.isEmpty())
    return;
  QPcapHttpConversation *c = _conversations.value(conversation.id());
  if (!c) {
    qWarning() << "QPcapHttpStack::tcpUpstreamPacket for unknown conversation";
    return;
  }
  switch(c->_state) {
  case Ready:
    c->_switched = false;
    c->_expect100Continue = false;
    c->_buf.append(packet.payload());
    hasRequestPacket(packet, c);
    break;
  case InRequest:
    if (c->_switched) {
      if (!c->_expect100Continue) {
        qDebug() << c->_id << "sw>";
        c->_buf.clear();
        c->_state = InResponse;
        hasResponsePacket(packet, c);
      } else {
        qDebug() << c->_id << "ig>";
        c->_expect100Continue = false;
      }
    } else {
      c->_buf.append(packet.payload());
      hasRequestPacket(packet, c);
    }
    break;
  case InResponse:
    if (c->_switched) {
      c->_buf.append(packet.payload());
      hasResponsePacket(packet, c);
    } else {
      emit httpHit(conversation, c->_hit);
      c->_buf.clear();
      c->_buf.append(packet.payload());
      c->_state = Ready;
      c->_expect100Continue = false;
      c->_hit = QPcapHttpHit();
      hasRequestPacket(packet, c);
    }
    break;
  case NonHttp:
    return;
  }
}

void QPcapHttpStack::tcpDownstreamPacket(QPcapTcpPacket packet,
                                         QPcapTcpConversation conversation) {
  if (packet.isEmpty())
    return;
  QPcapHttpConversation *c = _conversations.value(conversation.id());
  if (!c) {
    qWarning() << "QPcapHttpStack::tcpDownstreamPacket for unknown "
                  "conversation";
    return;
  }
  switch(c->_state) {
  case Ready:
    c->_switched = true;
    c->_expect100Continue = false;
    c->_buf.append(packet.payload());
    hasRequestPacket(packet, c);
    break;
  case InRequest:
    if (c->_switched) {
      c->_buf.append(packet.payload());
      hasRequestPacket(packet, c);
    } else {
      if (!c->_expect100Continue) {
        qDebug() << c->_id << "<sw";
        c->_buf.clear();
        c->_state = InResponse;
        hasResponsePacket(packet, c);
      } else {
        qDebug() << c->_id << "<ig";
        c->_expect100Continue = false;
      }
    }
    break;
  case InResponse:
    if (c->_switched) {
      emit httpHit(conversation, c->_hit);
      c->_buf.clear();
      c->_buf.append(packet.payload());
      c->_state = Ready;
      c->_expect100Continue = false;
      c->_hit = QPcapHttpHit();
      hasRequestPacket(packet, c);
    } else {
      c->_buf.append(packet.payload());
      hasResponsePacket(packet, c);
    }
    break;
  case NonHttp:
    return;
  }
}

void QPcapHttpStack::hasRequestPacket(QPcapTcpPacket packet,
                                      QPcapHttpConversation *c) {
  if (c->_hit.firstRequestPacket().isNull())
    c->_hit.firstRequestPacket() = packet;
  forever {
    QString s(c->_buf.constData());
    int p = s.indexOf('\n');
    if (p < 0)
      break;
    if (c->_state == Ready) {
      if (_requestRE.exactMatch(s.left(p))) {
        QString method = _requestRE.cap(1);
        // LATER support other methods than GET/HEAD/POST
        if (method == "GET") {
          c->_hit.method() = GET;
        } else if (method == "POST") {
          c->_hit.method() = POST;
        } else if (method == "HEAD") {
          c->_hit.method() = HEAD;
        }
        QString url = _requestRE.cap(2).trimmed();
        c->_hit.path() = url; // LATER support proxy hits with host in url
        c->_hit.protocol() = "http";
        c->_state = InRequest;
        c->_buf.remove(0, p+1);
      } else {
        if (!c->_expect100Continue) {
          // FIXME this may detect as non http conversations that begin before start of capture
          qDebug() << "HTTP inconsistency detected (case 5)" << c->_id
                   << s.left(p);
          c->_state = NonHttp;
          c->_buf.clear();
          return;
        } else
          c->_expect100Continue = false;
      }
    } else if (c->_state == InRequest) {
      if (_headerRE.exactMatch(s.left(p))) {
        QString key = _headerRE.cap(1);
        if (c->_hit.host().isEmpty()
            && key.compare("Host", Qt::CaseInsensitive) == 0) {
          c->_hit.host() = _headerRE.cap(2);
        } else if (key.compare("Expect", Qt::CaseInsensitive) == 0) {
          if (_headerRE.cap(2).contains("100-continue", Qt::CaseInsensitive)) {
            qDebug() << c->_id << "... Expect: 100-continue detected";
            c->_expect100Continue = true;
          }
        }
      }
      c->_buf.remove(0, p+1);
    } else {
      qWarning() << "QPcapHttpStack::hasRequestPacket in unexpected state";
    }
  }
  if (c->_state == Ready && c->_buf.size() >= 2048) {
    // this should be main case were a non http conversation is detected
    qDebug() << c->_id << "ooo HTTP inconsistency detected (case 6)"
             << QString(c->_buf.constData()).left(32);
    c->_state = NonHttp;
    c->_buf.clear();
  }
  // do nothing, request may span accross several packets
}

void QPcapHttpStack::hasResponsePacket(QPcapTcpPacket packet,
                                       QPcapHttpConversation *c) {
  if (!c->_hit.firstResponseTimestamp())
    c->_hit.firstResponseTimestamp() = packet.ip().timestamp();
  c->_hit.lastResponseTimestamp() = packet.ip().timestamp();
}

void QPcapHttpStack::conversationFinished(QPcapTcpConversation conversation) {
  QPcapHttpConversation *c = _conversations.value(conversation.id());
  if (!c)
    return;
  if (c->_hit.lastResponseTimestamp()) {
    //qDebug() << c->_id << "xxx hit detected on conversation end";
    emit httpHit(conversation, c->_hit);
  }
  _conversations.remove(conversation.id());
  delete c;
}
