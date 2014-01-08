#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T11:12:24
#
#-------------------------------------------------

QT       -= core gui

TARGET = libvideotools
TEMPLATE = lib

DEFINES += LIBVIDEOTOOLS_LIBRARY

SOURCES += libvideotools.cpp

HEADERS += libvideotools.h\
        libvideotools_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
