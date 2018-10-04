TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

TARGET = VSCADA
QT       += core gui printsupport sql
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
    glv_thread.cpp \
    tsi_thread.cpp \
    tsv_thread.cpp \
    cool_thread.cpp \
    mainwindow.cpp\
    qcustomplot.cpp \
    canbus_interface.cpp

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
    cool_thread.h \
    mainwindow.h\
    qcustomplot.h\
    typedefs.h \
    canbus_interface.h

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
