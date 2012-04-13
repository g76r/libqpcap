#include "qpcapthread.h"
#include <QtDebug>
#include <pcap/pcap.h>
#include "qpcapengine.h"
#include <QCoreApplication>

QPcapThread::QPcapThread(QPcapEngine *engine, QObject *parent)
  : QThread(parent), _engine(engine) {
}

void QPcapThread::run() {
  forever {
    if (!_engine->_pcap) {
      qDebug() << "QPcapThread::run() with null pcap pointer";
      break;
    }
    int rc = pcap_dispatch(_engine->_pcap, 32, _engine->callback,
                           reinterpret_cast<u_char*>(_engine));
    QCoreApplication::processEvents();
    if (rc <= 0)
      break;
  }
  _engine->moveToThread(QCoreApplication::instance()->thread());
}
