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
  inline bool operator ==(
      const QPcapIPv4PortConversationSignatureData &other) const {
    return _data == 0
        ? other._data == 0
        : other._data && _data[0] == other._data[0]
          && _data[1] == other._data[1];
  }
  inline bool operator <(
      const QPcapIPv4PortConversationSignatureData &other) const {
    return _data == 0
        ? other._data != 0
        : other._data != 0
        && (_data[0] < other._data[0] || _data[1] < other._data[1]);
  }
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
    return *d == *(other.d);
  }
  inline bool operator <(
      const QPcapIPv4PortConversationSignature &other) const {
    return *d < *(other.d);
  }
  uint hashCode() const;
};

inline uint qHash(const QPcapIPv4PortConversationSignature &o) {
  return o.hashCode();
}

#endif // QPCAPIPV4PORTCONVERSATIONSIGNATURE_H
