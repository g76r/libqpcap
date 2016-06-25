#define QPCAPENGINE_CPP
#include "qpcapengine.h"
#include <QtDebug>
#include <QThread>
#include <QMetaType>
#include "qpcaplayer2packet.h"
#include "qpcaplayer3packet.h"
#include "qpcapipv4packet.h"
#include "qpcaptcpstack.h"
#include "qpcaphttphit.h"
#include <QMetaObject>

void QPcapEngine::init() {
  qRegisterMetaType<QPcapLayer1Packet>("QPcapLayer1Packet");
  qRegisterMetaType<QPcapLayer2Packet>("QPcapLayer2Packet");
  qRegisterMetaType<QPcapLayer3Packet>("QPcapLayer3Packet");
  qRegisterMetaType<QPcapIPv4Packet>("QPcapIPv4Packet");
  qRegisterMetaType<QPcapTcpPacket>("QPcapTcpPacket");
  qRegisterMetaType<QPcapTcpConversation>("QPcapTcpConversation");
  qRegisterMetaType<QPcapHttpHit>("QPcapHttpHit");
}

QPcapEngine::QPcapEngine() : _pcap(0), _packetsCount(0), _datalink(DLT_NULL),
  _preambleLength(0) {
  init();
}

QPcapEngine::QPcapEngine(QString filename) : _pcap(0), _packetsCount(0) {
  init();
  loadFile(filename);
}

QPcapEngine::~QPcapEngine() {
  if (_pcap)
    pcap_close(_pcap);
}

bool QPcapEngine::isRunning() const {
  QMutexLocker locker(&_mutex);
  return _pcap;
}

void QPcapEngine::loadFile(QString filename) {
  QMutexLocker locker(&_mutex);
  char errbuf[PCAP_ERRBUF_SIZE];
  qDebug() << "QPcapEngine::loadFile" << filename;
  if (_pcap) {
    qWarning() << "QPcapEngine::loadFile called while running (ignored)";
    return;
  }
  emit captureStarted();
  emit packetsCountTick(_packetsCount = 0);
  _pcap = pcap_open_offline(filename.toUtf8(), errbuf);
  if (_pcap) {
    _filename = filename;
    _datalink = pcap_datalink(_pcap);
    switch (_datalink) {
    case DLT_LINUX_SLL:
      // LATER handle several datalink for real rather than pretending that SLL
      // is EthernetII by just removing 2 bytes and put the 6 following ones in
      // destination address as if it were an address
      _preambleLength = 2;
      QMetaObject::invokeMethod(this, "readNextPackets", Qt::QueuedConnection);
      break;
    case DLT_EN10MB:
      _preambleLength = 0;
      QMetaObject::invokeMethod(this, "readNextPackets", Qt::QueuedConnection);
      break;
    default:
      qWarning() << "unsupported datalink format" << _datalink << "for file"
                 << filename;
      pcap_close(_pcap);
      _pcap = 0;
      emit captureFinished();
    }
  } else {
    qDebug() << "pcap_open_offline" << filename << "failed:" << errbuf;
    _filename = QString();
    emit captureFinished();
  }
}

void QPcapEngine::readNextPackets() {
  //qDebug() << "readNextPackets";
  int count = pcap_dispatch(_pcap, 128, this->callback,
                            reinterpret_cast<u_char*>(this));
  if (count > 0)
    QMetaObject::invokeMethod(this, "readNextPackets", Qt::QueuedConnection);
  else
    finishing();
}

void QPcapEngine::packetHandler(const struct pcap_pkthdr* pkthdr,
                               const u_char* packet) {
  //qDebug() << "PcapEngine::packetHandler" << (long)pkthdr << (long)packet;
  //QByteArray ba((const char *)packet, pkthdr->caplen);
  //qDebug() << "PcapEngine::packetHandler" //<< (long)pkthdr << (long)packet
  //         << ba.toHex();
  QPcapLayer1Packet pp(pkthdr, packet, _preambleLength);
  //qDebug() << pp;
  ++_packetsCount;
  emit layer1PacketReceived(pp);
  if (_packetsCount % 1000 == 0)
    emit packetsCountTick(_packetsCount);
}

void QPcapEngine::callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                          const u_char* packet) {
  //qDebug() << "PcapEngine::callback";
  QPcapEngine *that = reinterpret_cast<QPcapEngine*>(user);
  if (that)
    that->packetHandler(pkthdr, packet);
  else
    qDebug() << "QPcapEngine::callback with null object pointer";
}

void QPcapEngine::finishing() {
  QMutexLocker locker(&_mutex);
  qDebug() << "pcap capture finished" << _packetsCount;
  qDebug() << "QPcapEngine thread" << thread();
  emit packetsCountTick(_packetsCount);
  emit captureFinished();
  if (_pcap) {
    pcap_close(_pcap);
    _pcap = 0;
  }
}
