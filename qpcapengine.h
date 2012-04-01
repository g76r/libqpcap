#ifndef QPCAPENGINE_H
#define QPCAPENGINE_H

#if defined(QPCAPENGINE_CPP) || defined(lib_pcap_pcap_h)
#include <pcap/pcap.h>
#else
#define pcap_t void
#endif
#include <QString>
#include <QObject>
#include "qpcaplayer1packet.h"
#include "qpcap_global.h"

class QPcapThread;

class LIBQPCAPSHARED_EXPORT QPcapEngine : public QObject {
  Q_OBJECT
  friend class QPcapThread;
private:
  pcap_t *_pcap;
  QString _filename;
  QPcapThread *_thread;

public:
  QPcapEngine();
  QPcapEngine(QString filename);
  void loadFile(QString filename);
  void start();

signals:
  void captureTerminated();
  void layer1PacketReceived(QPcapLayer1Packet packet);

private:
  void init();
  void packetHandler(const struct pcap_pkthdr* pkthdr,
                     const u_char* packet);
  static void callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                       const u_char* packet);

  Q_DISABLE_COPY(QPcapEngine)
};

#endif // QPCAPENGINE_H
