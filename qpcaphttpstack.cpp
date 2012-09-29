#include "qpcaphttpstack.h"
#include <QtDebug>

QPcapHttpStack::QPcapHttpStack(QObject *parent, QPcapTcpStack *stack,
                               int maxDataKept)
  : QObject(parent),
    _requestRE("\\s*(GET|POST|HEAD)\\s+(\\S+)\\s+HTTP/1.[01]\\s*\r?"),
    _headerRE("(\\w+):\\s+(\\S*)\\s*\r?"),
    _100ContinueRE(".*HTTP/\\d\\.\\d\\s+100.*", Qt::CaseInsensitive),
    _responseRE("HTTP/\\d\\.\\d\\s+(\\d\\d\\d)(\\s+(.*))"), _hitsCount(0),
    _maxDataKept(maxDataKept) {
  connect(stack, SIGNAL(conversationStarted(QPcapTcpConversation)),
          this, SLOT(conversationStarted(QPcapTcpConversation)));
  connect(stack, SIGNAL(conversationFinished(QPcapTcpConversation)),
          this, SLOT(conversationFinished(QPcapTcpConversation)));
  connect(stack, SIGNAL(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          this, SLOT(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(stack, SIGNAL(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          this, SLOT(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(this, SIGNAL(discardUpstreamBuffer(QPcapTcpConversation)),
          stack, SLOT(discardUpstreamBuffer(QPcapTcpConversation)));
  connect(this, SIGNAL(discardDownstreamBuffer(QPcapTcpConversation)),
          stack, SLOT(discardDownstreamBuffer(QPcapTcpConversation)));
  connect(stack, SIGNAL(captureFinished()), this, SLOT(finishing()));
  connect(stack, SIGNAL(captureStarted()), this, SLOT(starting()));
}

void QPcapHttpStack::conversationStarted(QPcapTcpConversation conversation) {
  _conversations.insert(conversation.id(),
                        new QPcapHttpConversation(conversation));
}

void QPcapHttpStack::tcpUpstreamPacket(QPcapTcpPacket packet,
                                       QPcapTcpConversation conversation) {
  hasTcpPacket(true, packet, conversation);
}

void QPcapHttpStack::tcpDownstreamPacket(QPcapTcpPacket packet,
                                         QPcapTcpConversation conversation) {
  hasTcpPacket(false, packet, conversation);
}


void QPcapHttpStack::conversationFinished(QPcapTcpConversation conversation) {
  QPcapHttpConversation *c = _conversations.value(conversation.id());
  if (!c)
    return;
  if (c->_hit.isValid()) {
    // last response packet received has never been reported before
    emitHit(c);
  }
  _conversations.remove(conversation.id());
  delete c;
}

void QPcapHttpStack::hasTcpPacket(bool isUpstream, QPcapTcpPacket packet,
                                  QPcapTcpConversation conversation) {
  //qDebug() << "receiving HTTP packet" << conversation.id() << packet;
  if (packet.isEmpty())
    return;
  QPcapHttpConversation *c = _conversations.value(conversation.id());
  if (!c) {
    qWarning() << "QPcapHttpStack::tcpPacket for unknown conversation"
               << (isUpstream ? "(upstream)" : "(downstream)");
    return;
  }
  //if (c->_state != NonHttp)
  //  qDebug() << conversation.id() << "   " << c->_state;
  switch(c->_state) {
  case AwaitingRequest:
    // no packet yet seen, this should be the first packet of a request
    hasRequestPacket(packet, c);
    break;
  case InRequest:
    if (isUpstream != c->_switched) { // client to server
      // regular request packet
      hasRequestPacket(packet, c);
    } else { // server to client
      if (_100ContinueRE.exactMatch(packet.payload().constData())) {
        // this is a late 100 continue packet (comming after the request
        // continuation) so we ignore it
        //qDebug() << c->_tcp.id()
        //         << "... late 100-continue found and ignored" << packet;
        break;
      }
      // request is terminated since the new response arrives
      c->_state = InResponse;
      c->_buf.clear();
      // discarding unmatched tcp packet on the other stream should not be
      // necessary... but if there has been some sequence number
      // desynchronization due to lost packets during capture
      if (isUpstream)
        emit discardDownstreamBuffer(c->_tcp);
      else
        emit discardUpstreamBuffer(c->_tcp);
      // anyway, this is a response packet and should be processed
      hasResponsePacket(packet, c);
    }
    break;
  case Awaiting100Continue:
    // a request packet containing "Expect: 100-continue" has just been seen
    if (isUpstream != c->_switched) { // client to server
      // 100-continue is asked for but not waited for (or packet has been lost,
      // or packet is not in the same order in capture file and reality)
      // therefore we decide to forget about 100-continue
      //qDebug() << c->_tcp.id()
      //         << "... 100-continue expected but not found (case 2)" << packet;
      c->_state = InRequest;
      // process as regular request packet
      hasRequestPacket(packet, c);
    } else { // server to client
      has100ContinueResponsePacket(packet, c);
    }
    break;
  case InResponse:
    if (isUpstream != c->_switched) { // client to server
      // response is terminated since a new request arrives
      emitHit(c);
      c->_buf.clear();
      c->_hit = QPcapHttpHit();
      c->_state = AwaitingRequest;
      // discarding unmatched tcp packet on the other stream should not be
      // necessary... but if there has been some sequence number
      // desynchronization due to lost packets during capture
      if (isUpstream)
        emit discardDownstreamBuffer(c->_tcp);
      else
        emit discardUpstreamBuffer(c->_tcp);
      // anyway, this is a request packet and should be processed
      hasRequestPacket(packet, c);
    } else { // server to client
      // regular response packet
      hasResponsePacket(packet, c);
    }
    break;
  case TruncatedResponseBeforeRequest:
    if (isUpstream) { // server to client
      // ignore packet since it's before first request
    } else { // client to server
      // truncated response is terminated since a new request arrives
      c->_buf.clear();
      c->_hit = QPcapHttpHit();
      c->_state = AwaitingRequest;
      // discarding unmatched tcp packet on the other stream should not be
      // necessary... but if there has been some sequence number
      // desynchronization due to lost packets during capture
      emit discardUpstreamBuffer(c->_tcp);
      // process packet as the first packet of a request
      hasRequestPacket(packet, c);
    }
    break;
  case NonHttp:
    return;
  }
}

void QPcapHttpStack::hasRequestPacket(QPcapTcpPacket packet,
                                      QPcapHttpConversation *c) {
  c->_buf.append(packet.payload());
  if (c->_hit.firstRequestPacket().isNull()) {
    c->_hit.conversation() = c->_tcp;
    c->_hit.firstRequestPacket() = packet;
  }
  if (c->_upstreamData.size() < _maxDataKept)
    c->_upstreamData.append(packet.payload());
  forever {
    QString s(c->_buf.constData());
    int p = s.indexOf('\n');
    if (p < 0) {
      if (c->_state == AwaitingRequest && c->_buf.size() >= 2048) {
        // don't have yet seen a \n within first bytes of a request buffer
        // this should be main case were a non http conversation is detected
        //qDebug() << c->_tcp.id() << "oo6 HTTP inconsistency detected, probably "
        //            "not actual HTTP (case 6)"
        //         << QString(c->_buf.constData()).left(32);
        c->_buf.clear();
        c->_state = NonHttp;
      } else {
        // do nothing maybe (even not likely) the request first line take more
        // than one tcp packet
      }
      return;
    }
    switch (c->_state) {
    case AwaitingRequest:
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
        c->_buf.remove(0, p+1);
        c->_state = InRequest;
      } else if (!c->_switched) {
        // the request does not begin with a request string, it is either
        // non-HTTP or the connection began before capture start
        c->_state = TruncatedResponseBeforeRequest;
        c->_switched = true;
        return;
      } else {
        // won't switch again, this is definitely non-HTTP
        //qDebug() << c->_tcp.id() << "oo8 HTTP inconsistency detected, probably "
        //            "not actual HTTP (case 8)"
        //         << QString(c->_buf.constData()).left(32);
        c->_buf.clear();
        c->_state = NonHttp;
        return;
      }
      break;
    case InRequest:
    case Awaiting100Continue: // it can be set during a previous loop iteration
      if (_headerRE.exactMatch(s.left(p))) {
        QString key = _headerRE.cap(1);
        if (c->_hit.host().isEmpty()
            && key.compare("Host", Qt::CaseInsensitive) == 0) {
          c->_hit.host() = _headerRE.cap(2);
        } else if (key.compare("Expect", Qt::CaseInsensitive) == 0) {
          if (_headerRE.cap(2).contains("100-continue", Qt::CaseInsensitive)) {
            //qDebug() << c->_tcp.id() << "... Expect: 100-continue detected" << packet;
            c->_state = Awaiting100Continue;
          }
        }
      }
      c->_buf.remove(0, p+1);
      break;
    case InResponse:
    case NonHttp:
    case TruncatedResponseBeforeRequest:
      qWarning() << "QPcapHttpStack::hasRequestPacket in unexpected state";
    }
  }
}

void QPcapHttpStack::hasResponsePacket(QPcapTcpPacket packet,
                                       QPcapHttpConversation *c) {
  c->_buf.append(packet.payload());
  if (c->_downstreamData.size() < _maxDataKept)
    c->_downstreamData.append(packet.payload());
  if (!c->_hit.firstResponseTimestamp()) {
    //qDebug() << c->_tcp.id() << "=== setting timestamp" << packet;
    c->_hit.firstResponseTimestamp() = packet.ip().timestamp();
  } else {
    //qDebug() << c->_tcp.id() << "    not setting timestamp" << packet;
  }
  c->_hit.lastResponseTimestamp() = packet.ip().timestamp();
  if (!c->_hit.returnCode()) {
    QString s(c->_buf.constData());
    int p = s.indexOf('\n');
    if (p && _responseRE.exactMatch(s.left(p))) {
      quint16 rc = _responseRE.cap(1).toUShort();
      c->_hit.returnCode() = rc;
      //qDebug() << "found HTTP return code" << rc;
    } else if(c->_buf.size() > 256) {
      // no longer scan for return code (return code packetd lost or non-HTTP)
      c->_hit.returnCode() = -1;
    }
  }
}

void QPcapHttpStack::has100ContinueResponsePacket(QPcapTcpPacket packet,
                                                  QPcapHttpConversation *c) {
  if (_100ContinueRE.exactMatch(packet.payload().constData())) {
    // ignore packet and get back in regular InRequest state
    //qDebug() << c->_tcp.id()
    //         << "... 100-continue found" << packet;
    c->_state = InRequest;
  } else {
    // no 100-continue has been seen, this is a regular response packet
    //qDebug() << c->_tcp.id()
    //         << "... 100-continue expected but not found (case 1)" << packet;
    c->_state = InResponse;
    hasResponsePacket(packet, c);
  }
}

void QPcapHttpStack::starting() {
  qDeleteAll(_conversations.values());
  _conversations.clear();
  emit captureStarted();
  emit hitsCountTick(_hitsCount = 0);
}

void QPcapHttpStack::finishing() {
  emit hitsCountTick(_hitsCount);
  emit captureFinished();
  qDeleteAll(_conversations.values());
  _conversations.clear();
}

void QPcapHttpStack::emitHit(QPcapHttpConversation *c) {
  //qDebug() << "about to emit hit" << c->_hit;
  //qDebug() << "packets:" << c->_hit.conversation().packets().size()
  //         << "data:" << c->_buf.size() << c->_upstreamData.size()
  //         << c->_downstreamData.size();
  c->_upstreamData.truncate(_maxDataKept);
  c->_downstreamData.truncate(_maxDataKept);
  emit httpHit(c->_hit, c->_upstreamData, c->_downstreamData);
  ++_hitsCount;
  if (_hitsCount % 20 == 0)
    emit hitsCountTick(_hitsCount);
}
