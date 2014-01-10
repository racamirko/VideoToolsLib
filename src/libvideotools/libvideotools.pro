#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T11:12:24
#
#-------------------------------------------------

QT       -= core gui

TARGET = libvideotools
TEMPLATE = lib

DEFINES += LIBVIDEOTOOLS_LIBRARY

SOURCES += libvideotools.cpp \
    CDataUtils.cpp \
    CPcaCompression.cpp

INCLUDEPATH += /usr/local/include
LIBS += -lglog -L/usr/local/lib -lopencv_core

QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -DNDEBUG

HEADERS += \
    ../../include/libvideotools_global.h \
    ../../include/libvideotools.h \
    ../../include/CDataUtils.h \
    globalInclude.h \
    ../../include/CCompressorInterface.h \
    ../../include/CPcaCompression.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
