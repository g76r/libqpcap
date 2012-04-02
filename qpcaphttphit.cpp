#include "qpcaphttphit.h"

QString QPcapHttpHit::english() const {
  return QString("QPcapHttpHit(%1 %2 %3 %4 %5 %6 %7)")
      .arg(method()).arg(protocol()).arg(host()).arg(path())
      .arg(requestTimestamp()).arg(usecToFirstByte()).arg(usecToLastByte());
}
