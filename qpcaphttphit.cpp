#include "qpcaphttphit.h"
#include <QDateTime>

QString QPcapHttpHit::english() const {
  return QString("QPcapHttpHit(%1 %2 %3 %4 %5 %6 %7)")
      .arg(methodAsString()).arg(protocol()).arg(host()).arg(path())
      .arg(requestTimestamp()).arg(usecToFirstByte()).arg(usecToLastByte());
}

qint64 QPcapHttpHit::writeCsv(QIODevice *output) const {
  QDateTime dt = QDateTime::fromMSecsSinceEpoch(requestTimestamp()/1000);
  QString host = d->_host;
  QString path = d->_path;
  host.replace(";", "|"); // LATER use better ; escape
  path.replace(";", "|");
  QString s;
  s.append(dt.toString("yyyy-MM-dd")).append(';')
      .append(dt.toString("hh:mm:ss")).append(';')
      .append(QString::number(requestTimestamp()%1000000)).append(';')
      .append(QString::number(requestTimestamp())).append(';')
      .append(d->_protocol).append(';').append(methodAsString()).append(';')
      .append(host).append(';').append(path).append(';')
      .append(d->_firstRequestPacket.ip().src()).append(';')
      .append(d->_firstRequestPacket.ip().dst()).append(';')
      .append(QString::number(d->_firstRequestPacket.srcPort())).append(';')
      .append(QString::number(d->_firstRequestPacket.dstPort())).append(';')
      .append(d->_firstRequestPacket.src()).append(';')
      .append(d->_firstRequestPacket.dst()).append(';')
      .append(QString::number(usecToFirstByte())).append(';')
      .append(QString::number(usecToLastByte())).append('\n');
  return output->write(s.toUtf8());
}

qint64 QPcapHttpHit::writeCsvHeader(QIODevice *output) const {
  return output->write("day;time;usec;usec_since_1970;protocol;method;host;"
                       "path;ip_src;ip_dst;tcp_src_port;tcp_dst_port;tcp_src;"
                       "tcp_src;ttfb;ttlb\n");
}
