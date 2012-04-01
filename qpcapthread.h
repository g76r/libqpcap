#ifndef PCAPTHREAD_H
#define PCAPTHREAD_H

#include "qpcap_global.h"
#include <QThread>

class QPcapEngine;

class LIBQPCAPSHARED_EXPORT QPcapThread : public QThread {
  Q_OBJECT
private:
  QPcapEngine *_engine;

public:
  explicit QPcapThread(QPcapEngine *engine, QObject *parent = 0);
  void run();

private:
  Q_DISABLE_COPY(QPcapThread)
};

#endif // PCAPTHREAD_H
