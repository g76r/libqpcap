#define QPCAPENGINE_CPP
#include "qpcapengine.h"
#include <QtDebug>
#include <QThread>
#include "qpcapthread.h"
#include <QMetaType>
#include "qpcaplayer2packet.h"
#include "qpcaplayer3packet.h"
#include "qpcapipv4packet.h"
#include "qpcaptcpstack.h"
#include "qpcaphttphit.h"

void QPcapEngine::init() {
  _thread = new QPcapThread(this);
  qRegisterMetaType<QPcapLayer1Packet>("QPcapLayer1Packet");
  qRegisterMetaType<QPcapLayer2Packet>("QPcapLayer2Packet");
  qRegisterMetaType<QPcapLayer3Packet>("QPcapLayer3Packet");
  qRegisterMetaType<QPcapIPv4Packet>("QPcapIPv4Packet");
  qRegisterMetaType<QPcapTcpPacket>("QPcapTcpPacket");
  qRegisterMetaType<QPcapTcpConversation>("QPcapTcpConversation");
  qRegisterMetaType<QPcapHttpHit>("QPcapHttpHit");
  connect(_thread, SIGNAL(finished()), this, SLOT(finishing()));
}

QPcapEngine::QPcapEngine() : _pcap(0), _packetsCount(0) {
  init();
}

QPcapEngine::QPcapEngine(QString filename) : _pcap(0), _packetsCount(0) {
  init();
  loadFile(filename);
}

void QPcapEngine::start() {
  moveToThread(_thread);
  _thread->start();
}

bool QPcapEngine::isRunning() const {
  return _thread && _thread->isRunning();
}

void QPcapEngine::loadFile(QString filename) {
  char errbuf[PCAP_ERRBUF_SIZE];
  if (_pcap) {
    pcap_close(_pcap);
    _pcap = 0;
  }
  emit captureStarted();
  emit packetsCountTick(_packetsCount = 0);
  _pcap = pcap_open_offline(filename.toUtf8(), errbuf);
  if (_pcap)
    _filename = filename;
  else {
    qDebug() << "pcap_open_offline" << filename << "failed:" << errbuf;
    _filename = QString();
  }
}

void QPcapEngine::packetHandler(const struct pcap_pkthdr* pkthdr,
                               const u_char* packet) {
  //qDebug() << "PcapEngine::packetHandler" << (long)pkthdr << (long)packet;
  //QByteArray ba((const char *)packet, pkthdr->caplen);
  //qDebug() << "PcapEngine::packetHandler" //<< (long)pkthdr << (long)packet
  //         << ba.toHex();
  QPcapLayer1Packet pp(pkthdr, packet);
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
  //qDebug() << "pcap capture finished";
  emit packetsCountTick(_packetsCount);
  emit captureFinished();
  if (_pcap) {
    pcap_close(_pcap);
    _pcap = 0;
  }
}
