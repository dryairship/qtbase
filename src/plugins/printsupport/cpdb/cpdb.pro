TARGET = cpdbprintersupport
MODULE = cpdbprintersupport

QT += core-private gui-private printsupport printsupport-private

INCLUDEPATH += ../../../printsupport/kernel

CONFIG += link_pkgconfig
PKGCONFIG += gio-unix-2.0 glib-2.0 gobject-2.0 cpdb-libs-frontend

SOURCES += main.cpp \
    qcpdbprintdevice.cpp \
    qcpdbprintersupport.cpp \
    qcpdbprintengine.cpp \
    ../../../printsupport/kernel/qcpdb.cpp

HEADERS += qcpdbprintersupport_p.h \
    qcpdbprintdevice.h \
    qcpdbprintengine_p.h \
    ../../../printsupport/kernel/qcpdb_p.h

OTHER_FILES += cpdb.json

PLUGIN_TYPE = printsupport
PLUGIN_CLASS_NAME = QCpdbPrinterSupportPlugin
load(qt_plugin)
