#ifndef QPCAPHTTPHIT_H
#define QPCAPHTTPHIT_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QString>
#include <QtDebug>
#include <QIODevice>
#include "qpcaptcppacket.h"
#include "qpcaptcpconversation.h"

class QPcapHttpHit;

enum QPcapHttpMethod { UNKNOWN, GET, POST, HEAD };

class LIBQPCAPSHARED_EXPORT QPcapHttpHitData : public QSharedData {
  friend class QPcapHttpHit;
private:
  QPcapHttpMethod _method;
  QString _protocol, _host, _path;
  quint16 _returnCode;
  QPcapTcpPacket _firstRequestPacket;
  quint64 _firstResponseTimestamp, _lastResponseTimestamp;
  QPcapTcpConversation _conversation;

public:
  inline QPcapHttpHitData(
      QPcapTcpConversation conversation = QPcapTcpConversation())
    : _method(UNKNOWN), _returnCode(0), _firstResponseTimestamp(0),
      _lastResponseTimestamp(0), _conversation(conversation) { }
  /*inline QPcapHttpHitData(QPcapHttpMethod method, QString protocol,
                          QString host, QString path,
                          QPcapTcpPacket firstRequestPacket,
                          QPcapTcpConversation conversation)
    : _method(method), _protocol(protocol), _host(host), _path(path),
      _returnCode(0), _firstRequestPacket(firstRequestPacket),
      _firstResponseTimestamp(0), _lastResponseTimestamp(0),
      _conversation(conversation) { }*/
  inline QPcapHttpHitData(const QPcapHttpHitData &other) : QSharedData(),
    _method(other._method), _protocol(other._protocol), _host(other._host),
    _path(other._path), _returnCode(other._returnCode),
    _firstRequestPacket(other._firstRequestPacket),
    _firstResponseTimestamp(other._firstResponseTimestamp),
    _lastResponseTimestamp(other._lastResponseTimestamp),
    _conversation(other._conversation) { }
};

class LIBQPCAPSHARED_EXPORT QPcapHttpHit {
private:
  QExplicitlySharedDataPointer<QPcapHttpHitData> d;

public:
  QPcapHttpHit(QPcapTcpConversation conversation = QPcapTcpConversation())
    : d(new QPcapHttpHitData(conversation)) { }
  QPcapHttpMethod method() const { return d->_method; }
  QString methodAsString() const {
    switch(d->_method) {
    case GET:
      return QString("GET");
    case POST:
      return QString("POST");
    case HEAD:
      return QString("HEAD");
    case UNKNOWN:
      ;
    }
    return QObject::tr("unknown");
  }
  QPcapHttpMethod &method() { return d->_method; }
  QString protocol() const { return d->_protocol; }
  QString &protocol() { return d->_protocol; }
  QString host() const { return d->_host; }
  QString &host() { return d->_host; }
  QString path() const { return d->_path; }
  QString &path() { return d->_path; }
  quint16 returnCode() const { return d->_returnCode; }
  quint64 requestTimestamp() const { return d->_firstRequestPacket.ip().timestamp(); }
  QPcapTcpPacket &firstRequestPacket() const { return d->_firstRequestPacket; }
  quint64 firstResponseTimestamp() const { return d->_firstResponseTimestamp; }
  quint64 &firstResponseTimestamp() { return d->_firstResponseTimestamp; }
  quint64 lastResponseTimestamp() const { return d->_lastResponseTimestamp; }
  quint64 &lastResponseTimestamp() { return d->_lastResponseTimestamp; }
  quint64 usecToFirstByte() const { return d->_firstResponseTimestamp ? d->_firstResponseTimestamp-requestTimestamp() : 0; }
  quint64 usecToLastByte() const { return d->_lastResponseTimestamp ? d->_lastResponseTimestamp-requestTimestamp() : 0; }
  QPcapTcpConversation &conversation() const { return d->_conversation; }
  QString english() const;
  qint64 writeCsv(QIODevice *output) const;
  qint64 writeCsvHeader(QIODevice *output) const;
  bool isValid() const { return d->_method != UNKNOWN && d->_firstRequestPacket.ip().timestamp(); }
};

inline QDebug operator<<(QDebug dbg, const QPcapHttpHit &hit) {
  dbg.nospace() << hit.english();
  return dbg.space();
}

#endif // QPCAPHTTPHIT_H
