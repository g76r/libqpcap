#define QPCAPENGINE_CPP
#include "qpcapengine.h"
#include <QtDebug>
#include <QThread>
#include "qpcapthread.h"
#include <QMetaType>
#include "qpcaplayer2packet.h"

void QPcapEngine::init() {
  _thread = new QPcapThread(this);
  moveToThread(_thread);
  qRegisterMetaType<QPcapLayer1Packet>("QPcapLayer1Packet");
  qRegisterMetaType<QPcapLayer2Packet>("QPcapLayer2Packet");
  connect(_thread, SIGNAL(finished()), this, SIGNAL(captureTerminated()));
}

QPcapEngine::QPcapEngine() : _pcap(0) {
  init();
}

QPcapEngine::QPcapEngine(QString filename) : _pcap(0) {
  init();
  loadFile(filename);
}

void QPcapEngine::start() {
  _thread->start();
}

void QPcapEngine::loadFile(QString filename) {
  char errbuf[PCAP_ERRBUF_SIZE];
  if (_pcap) {
    pcap_close(_pcap);
    _pcap = 0;
  }
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
  emit layer1PacketReceived(pp);
}

void QPcapEngine::callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                          const u_char* packet) {
  //qDebug() << "PcapEngine::callback";
  reinterpret_cast<QPcapEngine*>(user)->packetHandler(pkthdr, packet);
}
