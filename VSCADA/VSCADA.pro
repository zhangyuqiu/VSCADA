TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

TARGET = VSCADA
QT     += xml
QT     += core gui printsupport sql
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -l sqlite3
LIBS += -pthread
LIBS += -lltdl
LIBS += -lcrypt
LIBS += -lm
LIBS += -lltdl

SOURCES += \
        main.cpp \
    db_engine.cpp \
    datamonitor.cpp \
    datacontrol.cpp \
    iocontrol.cpp \
    config.cpp \
    mainwindow.cpp\
    qcustomplot.cpp \
    canbus_interface.cpp \
    subsystemthread.cpp

HEADERS += \
    db_engine.h \
    datamonitor.h \
    datacontrol.h \
    iocontrol.h \
    typedefs.h \
    config.h \
    mainwindow.h\
    qcustomplot.h\
    typedefs.h \
    canbus_interface.h \
    subsystemthread.h

FORMS += \
        mainwindow.ui

QT += serialbus widgets
QT += core
QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CFLAGS += -std=gnu++0x -pthread
LIBS += -lboost_system

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
