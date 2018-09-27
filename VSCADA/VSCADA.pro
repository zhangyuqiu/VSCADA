TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

SOURCES += \
        main.cpp \
    db_engine.cpp \
    datamonitor.cpp \
    datacontrol.cpp \
    iocontrol.cpp \
    config.cpp

HEADERS += \
    db_engine.h \
    datamonitor.h \
    datacontrol.h \
    iocontrol.h \
    typedefs.h \
    config.h

QT += serialbus widgets
