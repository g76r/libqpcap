QT -= gui
TARGET = qpcap
TEMPLATE = lib

QMAKE_RPATHDIR = # no rpath in linking

DEFINES += LIBQPCAP_LIBRARY
win32:INCLUDEPATH += c:/WpdPack/Include
win32:LIBS += c:/WpdPack/Lib/wpcap.lib
!win32:LIBS += -lpcap

linux-g++ {
  UI_DIR = ../libqpcap-build-linux/ui
  UI_HEADERS_DIR = ../libqpcap-build-linux/ui/include
  UI_SOURCES_DIR = ../libqpcap-build-linux/ui/src
  OBJECTS_DIR = ../libqpcap-build-linux/obj
  RCC_DIR = ../libqpcap-build-linux/rcc
  MOC_DIR = ../libqpcap-build-linux/moc
}
macx {
  UI_DIR = ../libqpcap-build-macos/ui
  UI_HEADERS_DIR = ../libqpcap-build-macos/ui/include
  UI_SOURCES_DIR = ../libqpcap-build-macos/ui/src
  OBJECTS_DIR = ../libqpcap-build-macos/obj
  RCC_DIR = ../libqpcap-build-macos/rcc
  MOC_DIR = ../libqpcap-build-macos/moc
}

SOURCES += \
    qpcapthread.cpp \
    qpcaptcpstack.cpp \
    qpcaptcppacket.cpp \
    qpcaptcpconversation.cpp \
    qpcaplayer3packet.cpp \
    qpcaplayer2packet.cpp \
    qpcaplayer1packet.cpp \
    qpcapipv4stack.cpp \
    qpcapipv4packet.cpp \
    qpcapethernetstack.cpp \
    qpcapethernetpacket.cpp \
    qpcapengine.cpp \
    qpcaphttpstack.cpp \
    qpcaphttphit.cpp

HEADERS += qpcap_global.h \
    qpcapthread.h \
    qpcaptcpstack.h \
    qpcaptcppacket.h \
    qpcaptcpconversation.h \
    qpcaplayer3packet.h \
    qpcaplayer2packet.h \
    qpcaplayer1packet.h \
    qpcapipv4stack.h \
    qpcapipv4packet.h \
    qpcapethernetstack.h \
    qpcapethernetpacket.h \
    qpcapengine.h \
    qpcap_global.h \
    qpcaphttpstack.h \
    qpcaphttphit.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1C87483
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = libqpcap.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
