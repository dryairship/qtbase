TARGET = cpdbprintersupport
MODULE = cpdbprintersupport

QT += core-private gui-private printsupport printsupport-private

# QMAKE_USE_PRIVATE += cpdb

INCLUDEPATH += ../../../printsupport/kernel

SOURCES += main.cpp \
    qcpdbprintdevice.cpp \
    qcpdbprintersupport.cpp \
    qcpdbprintengine.cpp

HEADERS += qcpdbprintersupport_p.h \
    qcpdbprintdevice.h \
    qcpdbprintengine_p.h

OTHER_FILES += cpdb.json

PLUGIN_TYPE = printsupport
PLUGIN_CLASS_NAME = QCpdbPrinterSupportPlugin
load(qt_plugin)
