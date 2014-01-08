#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T11:16:56
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_vtltestapptest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_vtltestapptest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../build/build-libvideotools-Qt_4_8_4_System-Debug/release/ -llibvideotools
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../build/build-libvideotools-Qt_4_8_4_System-Debug/debug/ -llibvideotools
else:unix: LIBS += -L$$PWD/../../../../../build/build-libvideotools-Qt_4_8_4_System-Debug/ -llibvideotools

INCLUDEPATH += $$PWD/../../include /usr/local/include
DEPENDPATH += $$PWD/../../include

LIBS += -lglog -L/usr/local/lib -lopencv_core
