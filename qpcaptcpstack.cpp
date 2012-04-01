#include "qpcaptcpstack.h"

void QPcapTcpStack::ipPacketReceived(QPcapIPv4Packet packet) {
  if (packet.layer4Proto() != QPcapIPv4Packet::TCP) {
    return; // ignoring non-tcp packets
  }
  QPcapTcpPacket tcp(packet);
  if (tcp.isNull()) {
    qDebug() << "   ignoring malformed tcp packet";
    return;
  }
  foreach (QPcapTcpConversation c, _conversations) {
    if (c.matchesEitherStream(tcp)) {
      //qDebug() << c.id() << "   received established tcp packet" << tcp << c.nextUpstreamNumber() << c.nextDownstreamNumber();
      dispatchPacket(tcp, c);
      return;
    }
  }
  if (tcp.fin() || tcp.rst()) {
    // this avoids creating conversation for trailing fin-ack packets when the
    // fin packet has been seen and the conversatino closed
    // this is a hack and hides those packets, however this has no impact
    // on upper layers
    //qDebug() << "   ignoring fin or rst new tcp packet" << tcp << tcp.fin() << tcp.rst();
    return;
  }
  QPcapTcpConversation c(tcp);
  //qDebug() << c.id() << "   received new tcp packet" << tcp;
  _conversations.insert(c);
  emit conversationStarted(c);
  dispatchPacket(tcp, c);
}

void QPcapTcpStack::dispatchPacket(QPcapTcpPacket packet,
                                  QPcapTcpConversation conversation) {
  if (conversation.matchesSameStream(packet)) {
    // upstream packet (client to server)
    //qDebug() << "  upstream" << conversation.id() << packet.seqNumber() << conversation.nextUpstreamNumber() << packet.payload().size();
    if (packet.seqNumber() == conversation.nextUpstreamNumber()) {
      emit tcpUpstreamPacket(packet, conversation);
      conversation.nextUpstreamNumber() +=
          conversation.numbersInitialized() || !packet.syn() || packet.ack()
          ? packet.payload().size() : 1;
      QPcapTcpPacket packet2;
      foreach (QPcapTcpPacket p, _upstreamBuffer.values(conversation))
        if (p.seqNumber() == conversation.nextUpstreamNumber()) {
          packet2 = p;
          break;
        }
      if (!packet2.isNull()) {
        //qDebug() << "  found upstream buffered packet" << packet2;
        dispatchPacket(packet2, conversation); // this is a recursive call
        //qDebug() << "  removing buffered packet" << packet2;
        _upstreamBuffer.remove(packet2);
      }
    } else {
      if ((qint32)(packet.seqNumber()-conversation.nextUpstreamNumber()) < 0) {
        // retransmission of already treated packet: nothing to do
        if (conversation.nextUpstreamNumber()-(qint32)(packet.seqNumber() == 1)) {
            //qDebug() << conversation.id() << "KK>" << packet; // probable keepalive
        } else {
            //qDebug() << conversation.id() << "RR>" << packet;
        }
      } else {
        //qDebug() << conversation.id() << "-->" << packet;
        //qDebug() << "  inserting upstream buffered packet" << packet;
        _upstreamBuffer.insertMulti(conversation, packet);
      }
    }
  } else {
    // downstream packet (server to client)
    if (!conversation.numbersInitialized()) {
      conversation.nextDownstreamNumber() = packet.seqNumber();
    }
    //qDebug() << "  downstream" << conversation.id() << packet.seqNumber() << conversation.nextDownstreamNumber() << packet.payload().size();
    if (packet.seqNumber() == conversation.nextDownstreamNumber()) {
      emit tcpDownstreamPacket(packet, conversation);
      conversation.nextDownstreamNumber() +=
          conversation.numbersInitialized() || !packet.syn() || !packet.ack()
          ? packet.payload().size() : 1;
      QPcapTcpPacket packet2;
      foreach (QPcapTcpPacket p, _downstreamBuffer.values(conversation))
        if (p.seqNumber() == conversation.nextDownstreamNumber()) {
          packet2 = p;
          break;
        }
      if (!packet2.isNull()) {
        //qDebug() << "  found downstream buffered packet" << packet2;
        dispatchPacket(packet2, conversation); // this is a recursive call
        //qDebug() << "  removing buffered packet" << packet2;
        _downstreamBuffer.remove(packet2);
      }
    } else {
      if ((qint32)(packet.seqNumber()-conversation.nextDownstreamNumber()) < 0){
        // retransmission of already treated packet: nothing to do
        if (conversation.nextDownstreamNumber()-(qint32)(packet.seqNumber()==1)) {
          //qDebug() << conversation.id() << "KK>" << packet; // probable keepalive
        } else {
          //qDebug() << conversation.id() << "<RR" << packet;
        }
      } else {
        //qDebug() << conversation.id() << "<--" << packet;
        //qDebug() << "  inserting downstream buffered packet" << packet;
        _downstreamBuffer.insertMulti(conversation, packet);
      }
    }
    if (!conversation.numbersInitialized()) {
      conversation.numbersInitialized() = true;
    }
  }
  if (packet.rst() || packet.fin()) {
    //qDebug() << conversation.id() << "XXX";
    emit conversationFinished(conversation);
    _conversations.remove(conversation);
    if (_upstreamBuffer.values(conversation).size() != 0) { // should be useless
      qDebug() << conversation.id() << "  remaining upstream buffered packets"
               << _upstreamBuffer.size();
      _upstreamBuffer.remove(conversation);
    }
    if (_downstreamBuffer.values(conversation).size() != 0) { // should be useless
      qDebug() << conversation.id() << "  remaining downstream buffered packets"
               << _downstreamBuffer.size();
      _downstreamBuffer.remove(conversation);
    }
  }
}
