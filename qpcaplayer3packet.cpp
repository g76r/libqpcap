#include "qpcaplayer3packet.h"

QPcapLayer3PacketData::~QPcapLayer3PacketData() {
}

QString QPcapLayer3PacketData::toText() const {
  return QString("PcapLayer3Packet()");
}
