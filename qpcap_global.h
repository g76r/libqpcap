#ifndef LIBQPCAP_GLOBAL_H
#define LIBQPCAP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBQPCAP_LIBRARY)
#  define LIBQPCAPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBQPCAPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBQPCAP_GLOBAL_H
