#ifndef QPCAPENGINE_H
#define QPCAPENGINE_H

#if defined(QPCAPENGINE_CPP) || defined(lib_pcap_pcap_h)
#include <pcap/pcap.h>
#else
#define pcap_t void
#endif
#include <QString>
#include <QMutex>
#include "qpcaplayer1packet.h"
#include "qpcap_global.h"

class QThread;

class LIBQPCAPSHARED_EXPORT QPcapEngine : public QObject {
  Q_OBJECT
  friend class QPcapThread;
private:
  pcap_t *_pcap;
  QString _filename;
  unsigned long _packetsCount;
  mutable QMutex _mutex;

public:
  QPcapEngine();
  QPcapEngine(QString filename);
  ~QPcapEngine();
  void loadFile(QString filename);
  bool isRunning() const;

signals:
  // LATER propagate captureXXX() signals to all stacks
  void captureStarted();
  void captureFinished();
  void layer1PacketReceived(QPcapLayer1Packet packet);
  void packetsCountTick(unsigned long count);

private slots:
  void readNextPackets();

private:
  void init();
  void packetHandler(const struct pcap_pkthdr* pkthdr,
                     const u_char* packet);
  static void callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                       const u_char* packet);
  void finishing();
  Q_DISABLE_COPY(QPcapEngine)
};

#endif // QPCAPENGINE_H
