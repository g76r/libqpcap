#include "qpcaptcpconversation.h"
#include <QAtomicInt>

static QAtomicInt _conversationCounter = 1;

QPcapTcpConversationData::QPcapTcpConversationData(QPcapTcpPacket firstPacket)
  : QSharedData(),
    _id(firstPacket.isNull() ? 0 : _conversationCounter.fetchAndAddRelaxed(1)),
    _firstPacket(firstPacket), _nextUpstreamNumber(firstPacket.seqNumber()),
    _nextDownstreamNumber(0), _numbersInitialized(false) {
}
