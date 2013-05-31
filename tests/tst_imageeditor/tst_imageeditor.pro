TARGET = tst_imageeditor

include(../test_common.pri)

CONFIG(use_quill) {
    CONFIG += link_pkgconfig
    PKGCONFIG += quill
} else {
    DEFINES += USE_QIMAGE
}

INCLUDEPATH += ../../src/
SOURCES += tst_imageeditor.cpp \
    ../../src/declarativeimageeditor.cpp \
    ../../src/declarativeimageeditor_p.cpp

HEADERS += ../../src/declarativeimageeditor.h \
    ../../src/declarativeimageeditor_p.h
