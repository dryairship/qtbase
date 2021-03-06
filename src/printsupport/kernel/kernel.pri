HEADERS += \
        $$PWD/qpaintengine_alpha_p.h \
        $$PWD/qprint_p.h \
        $$PWD/qprintdevice_p.h \
        $$PWD/qprintengine.h \
        $$PWD/qprinter.h \
        $$PWD/qprinter_p.h \
        $$PWD/qprinterinfo.h \
        $$PWD/qprinterinfo_p.h \
        $$PWD/qplatformprintdevice.h \
        $$PWD/qplatformprintplugin.h \
        $$PWD/qplatformprintersupport.h \
        $$PWD/qtprintsupportglobal_p.h \
        $$PWD/qtprintsupportglobal.h

SOURCES += \
        $$PWD/qpaintengine_alpha.cpp \
        $$PWD/qprintdevice.cpp \
        $$PWD/qprintengine_pdf.cpp \
        $$PWD/qprinter.cpp \
        $$PWD/qprinterinfo.cpp \
        $$PWD/qplatformprintdevice.cpp \
        $$PWD/qplatformprintplugin.cpp \
        $$PWD/qplatformprintersupport.cpp

qtConfig(printpreviewwidget) {
    HEADERS += $$PWD/qpaintengine_preview_p.h
    SOURCES += $$PWD/qpaintengine_preview.cpp
}

win32 {
        HEADERS += \
                $$PWD/qprintengine_win_p.h
        SOURCES += \
                $$PWD/qprintengine_win.cpp
        !winrt {
            LIBS_PRIVATE += -lwinspool -lcomdlg32
            QMAKE_USE_PRIVATE += user32 gdi32
        }
}

unix:!darwin:qtConfig(cups) {
        INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/printsupport/cpdb
        SOURCES += $$PWD/qcups.cpp
        HEADERS += $$PWD/qcups_p.h
}

unix:!darwin:qtConfig(cpdb){
        CONFIG += link_pkgconfig
        PKGCONFIG += gio-unix-2.0 glib-2.0 gobject-2.0 cpdb-libs-frontend
        SOURCES += $$PWD/qcpdb.cpp
        HEADERS += $$PWD/qcpdb_p.h
}
