TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

TARGET = VSCADA
QT       += core gui printsupport sql
DEFINES += QT_DEPRECATED_WARNINGS

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
    cool_thread.cpp \
    mainwindow.cpp\
    qcustomplot.cpp \
    dashboard.cpp

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
    dashboard.h

FORMS += \
        mainwindow.ui \
    dashboard.ui

QT += serialbus widgets
QT += core
QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CFLAGS += -std=gnu++0x -pthread

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
