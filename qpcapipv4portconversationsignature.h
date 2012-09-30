#ifndef QPCAPIPV4PORTCONVERSATIONSIGNATURE_H
#define QPCAPIPV4PORTCONVERSATIONSIGNATURE_H

#include <QSharedData>
#include <QSharedPointer>

class QPcapTcpPacket;

class QPcapIPv4PortConversationSignatureData : public QSharedData {
  friend class QPcapIPv4PortConversationSignature;
  const quint64 *_data;
  inline QPcapIPv4PortConversationSignatureData(const quint64 *data)
    : _data(data) { }
public:
  inline QPcapIPv4PortConversationSignatureData() : _data(0) { }
  inline QPcapIPv4PortConversationSignatureData(
      const QPcapIPv4PortConversationSignatureData &other)
    : QSharedData(), _data(other._data) { }
  inline ~QPcapIPv4PortConversationSignatureData() { if (_data) delete _data; }
};

class QPcapIPv4PortConversationSignature {
  QSharedPointer<QPcapIPv4PortConversationSignatureData> d;
public:
  inline QPcapIPv4PortConversationSignature()
    : d(new QPcapIPv4PortConversationSignatureData()) { }
  explicit QPcapIPv4PortConversationSignature(const QPcapTcpPacket tcpPacket);
  inline QPcapIPv4PortConversationSignature(
      const QPcapIPv4PortConversationSignature &other) : d(other.d) { }
  inline bool operator ==(
      const QPcapIPv4PortConversationSignature &other) const {
    return d->_data && other.d->_data && d->_data[0] == other.d->_data[0]
        && d->_data[1] == other.d->_data[1];
  }
  inline bool operator <(
      const QPcapIPv4PortConversationSignature &other) const {
    return d->_data && other.d->_data
        && (d->_data[0] < other.d->_data[0] || d->_data[1] < other.d->_data[1]);
  }
};

#endif // QPCAPIPV4PORTCONVERSATIONSIGNATURE_H
