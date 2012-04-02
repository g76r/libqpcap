#ifndef PCAPTCPPACKET_H
#define PCAPTCPPACKET_H

#include "qpcap_global.h"
#include <QtDebug>
#include "qpcapipv4packet.h"
#include <QByteArray>
#include <QSharedData>

class QPcapTcpPacket;

class LIBQPCAPSHARED_EXPORT QPcapTcpPacketData : public QSharedData {
  friend class QPcapTcpPacket;
protected:
  quint16 _srcPort;
  quint16 _dstPort;
  quint32 _seqNumber;
  quint32 _ackNumber; // should be ignored if !_ack
  quint8/*4*/ _headerSize; // in 32 bits words
  /*4*/
  quint8/*2*/ _ecn;
  bool _urg;
  bool _ack;
  bool _psh;
  bool _rst;
  bool _syn;
  bool _fin;
  quint16 _windowSize;
  quint16 _checksum;
  quint16 _urgentPointer; // should be ignored if !_urg
  QByteArray _payload;
  QPcapIPv4Packet _ip;

  inline void reset() {
    _srcPort = _dstPort = _seqNumber = _ackNumber = _headerSize = _ecn
        = _windowSize = _checksum = _urgentPointer = 0;
    _urg = _ack = _psh = _rst = _syn = _fin = false;
    _payload.clear();
  }

public:
  QPcapTcpPacketData(QPcapIPv4Packet packet = QPcapIPv4Packet());
  inline QPcapTcpPacketData(const QPcapTcpPacketData &other)
    : QSharedData(), _srcPort(other._srcPort), _dstPort(other._dstPort),
      _seqNumber(other._seqNumber), _ackNumber(other._ackNumber),
      _headerSize(other._headerSize), _ecn(other._ecn), _urg(other._urg),
      _ack(other._ack), _psh(other._psh), _rst(other._rst), _syn(other._syn),
      _fin(other._fin), _windowSize(other._windowSize),
      _checksum(other._checksum), _urgentPointer(other._urgentPointer),
      _payload(other._payload), _ip(other._ip) {
  }
  inline quint16 srcPort() const { return _srcPort; }
  inline quint16 dstPort() const { return _dstPort; }
  /** @return an adress:port string, e.g. 127.0.0.1:80
    */
  inline QString src() const {
    return QString("%1:%2").arg(_ip.src()).arg(_srcPort);
  }
  /** @return an adress:port string, e.g. 127.0.0.1:80
    */
  inline QString dst() const {
    return QString("%1:%2").arg(_ip.dst()).arg(_dstPort);
  }
  inline quint32 seqNumber() const { return _seqNumber; }
  inline quint32 ackNumber() const { return _ackNumber; }
  /** @return in bytes (= actual header field * 4)
    */
  inline quint32 headerSizeInBytes() const {
    return _headerSize*4;
  }
  /** @return in 4 bytes words count (= bytes / 4)
    */
  inline quint32 headerSize() const {
    return _headerSize;
  }
  inline quint8 ecn() const { return _ecn; }
  inline bool urg() const { return _urg; }
  inline bool ack() const { return _ack; }
  inline bool psh() const { return _psh; }
  inline bool rst() const { return _rst; }
  inline bool syn() const { return _syn; }
  inline bool fin() const { return _fin; }
  inline QByteArray payload() const { return _payload; }
  inline QPcapIPv4Packet ip() const { return _ip; }
  QString english() const;
};

class LIBQPCAPSHARED_EXPORT QPcapTcpPacket {
private:
  QExplicitlySharedDataPointer<QPcapTcpPacketData> d;

public:
  inline explicit QPcapTcpPacket(QPcapIPv4Packet packet = QPcapIPv4Packet()) {
    d = new QPcapTcpPacketData(packet);
  }
  inline QPcapTcpPacket(const QPcapTcpPacket &other) : d(other.d) { }
  inline quint16 srcPort() const { return d->srcPort(); }
  inline quint16 dstPort() const { return d->dstPort(); }
  inline QString src() const { return d->src(); }
  inline QString dst() const { return d->dst(); }
  inline quint32 seqNumber() const { return d->seqNumber(); }
  inline quint32 ackNumber() const { return d->ackNumber(); }
  /** @return in bytes (= actual header field * 4)
    */
  inline quint32 headerSizeInBytes() const { return d->headerSizeInBytes(); }
  /** @return in 4 bytes words count (= bytes / 4)
    */
  inline quint32 headerSize() const { return d->headerSize(); }
  inline quint8 ecn() const { return d->ecn(); }
  inline bool urg() const { return d->urg(); }
  inline bool ack() const { return d->ack(); }
  inline bool psh() const { return d->psh(); }
  inline bool rst() const { return d->rst(); }
  inline bool syn() const { return d->syn(); }
  inline bool fin() const { return d->fin(); }
  inline QByteArray payload() const { return d->payload(); }
  inline QPcapIPv4Packet ip() const { return d->ip(); }
  QString english() const { return d->english(); }
  inline bool isNull() const { return !d->srcPort(); }
  inline bool isEmpty() const { return d->payload().isEmpty(); }
};

inline QDebug operator<<(QDebug dbg, const QPcapTcpPacket &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPTCPPACKET_H
