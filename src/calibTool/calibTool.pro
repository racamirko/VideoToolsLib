TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include /usr/local/include
LIBS += -L/usr/lib -L/usr/local/lib -lglog -lopencv_core -lopencv_highgui -lboost_filesystem -lboost_program_options -lopencv_imgproc -lopencv_calib3d -lboost_system
QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -DNDEBUG
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp
