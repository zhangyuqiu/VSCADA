TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

LIBS += -l sqlite3

SOURCES += \
        main.cpp \
    db_engine.cpp \
    datamonitor.cpp \
    datacontrol.cpp \
    iocontrol.cpp \
    config.cpp \
    glv_thread.cpp \
    tsi_thread.cpp \
    tsv_thread.cpp \
    cool_thread.cpp

HEADERS += \
    db_engine.h \
    datamonitor.h \
    datacontrol.h \
    iocontrol.h \
    typedefs.h \
    config.h \
    glv_thread.h \
    tsi_thread.h \
    tsv_thread.h \
    cool_thread.h

QT += serialbus widgets
QT += core
QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CFLAGS += -std=gnu++0x -pthread
