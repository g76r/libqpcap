#define QPCAPLAYER1PACKET_CPP
#include "qpcaplayer1packet.h"

QPcapLayer1PacketData::QPcapLayer1PacketData(const struct pcap_pkthdr* pkthdr,
                                             const u_char* packet,
                                             const int preambleLength)
  : _timestamp(pkthdr->ts.tv_usec + pkthdr->ts.tv_sec*(1000000LL)),
    _wirelen(pkthdr->len), _payload(((const char*)packet)+preambleLength,
                                    pkthdr->caplen-preambleLength) {
}
