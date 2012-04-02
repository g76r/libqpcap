#ifndef QPCAPHTTPHIT_H
#define QPCAPHTTPHIT_H

#include "qpcap_global.h"
#include <QSharedData>
#include <QString>
#include <QtDebug>

class QPcapHttpHit;

enum QPcapHttpMethod { UNKNOWN, GET, POST, HEAD };

class LIBQPCAPSHARED_EXPORT QPcapHttpHitData : public QSharedData {
  friend class QPcapHttpHit;
private:
  QPcapHttpMethod _method;
  QString _protocol, _host, _path;
  quint16 _returnCode;
  quint64 _requestTimestamp, _firstPacketTimestamp, _lastPacketTimestamp;

public:
  inline QPcapHttpHitData() : _method(UNKNOWN), _returnCode(0),
    _requestTimestamp(0), _firstPacketTimestamp(0), _lastPacketTimestamp(0) { }
  inline QPcapHttpHitData(QPcapHttpMethod method, QString protocol,
                          QString host, QString path, quint64 requestTimestamp)
    : _method(method), _protocol(protocol), _host(host), _path(path),
      _returnCode(0), _requestTimestamp(requestTimestamp),
      _firstPacketTimestamp(0), _lastPacketTimestamp(0) { }
  inline QPcapHttpHitData(const QPcapHttpHitData &other) : QSharedData(),
    _method(other._method), _protocol(other._protocol), _host(other._host),
    _path(other._path), _returnCode(other._returnCode),
    _requestTimestamp(other._requestTimestamp),
    _firstPacketTimestamp(other._firstPacketTimestamp),
    _lastPacketTimestamp(other._lastPacketTimestamp) { }
};

class LIBQPCAPSHARED_EXPORT QPcapHttpHit {
private:
  QExplicitlySharedDataPointer<QPcapHttpHitData> d;

public:
  QPcapHttpHit() : d(new QPcapHttpHitData()) { }
  QPcapHttpMethod method() const { return d->_method; }
  QPcapHttpMethod &method() { return d->_method; }
  QString protocol() const { return d->_protocol; }
  QString &protocol() { return d->_protocol; }
  QString host() const { return d->_host; }
  QString &host() { return d->_host; }
  QString path() const { return d->_path; }
  QString &path() { return d->_path; }
  quint16 returnCode() const { return d->_returnCode; }
  quint64 requestTimestamp() const { return d->_requestTimestamp; }
  quint64 &requestTimestamp() { return d->_requestTimestamp; }
  quint64 firstPacketTimestamp() const { return d->_firstPacketTimestamp; }
  quint64 &firstPacketTimestamp() { return d->_firstPacketTimestamp; }
  quint64 lastPacketTimestamp() const { return d->_lastPacketTimestamp; }
  quint64 &lastPacketTimestamp() { return d->_lastPacketTimestamp; }
  /** @return < 0 if not known */
  quint64 usecToFirstByte() const { return d->_firstPacketTimestamp-d->_requestTimestamp; }
  /** @return < 0 if not known */
  quint64 usecToLastByte() const { return d->_lastPacketTimestamp-d->_requestTimestamp; }
  QString english() const;
};

inline QDebug operator<<(QDebug dbg, const QPcapHttpHit &hit) {
  dbg.nospace() << hit.english();
  return dbg.space();
}

#endif // QPCAPHTTPHIT_H
