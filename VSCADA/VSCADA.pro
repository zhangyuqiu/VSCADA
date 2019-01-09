TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

TARGET = VSCADA
QT     += xml
QT     += core gui printsupport sql
DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += $$_PRO_FILE_PWD_

DEFINES += MAIN_QML=\\\"Basic.qml\\\"

LIBS += -l sqlite3
LIBS += -pthread
LIBS += -lusb-1.0
#LIBS += -l:libudev.so.1
#LIBS += -l:libc.so.6
#LIBS += -l:libusb-1.0.so.0
#LIBS += -l:ld-linux.so.3
#LIBS += -lstdc++

SOURCES += \
        main.cpp \
    db_engine.cpp \
    datacontrol.cpp \
    config.cpp \
    mainwindow.cpp\
    qcustomplot.cpp \
    canbus_interface.cpp \
    subsystemthread.cpp \
    gpio_interface.cpp \
    detailpage.cpp \
    dashboard.cpp \
    qcgaugewidget.cpp \
    libusb_interface/pmd.c \
    libusb_interface/test-usb7204.c \
    libusb_interface/usb-7204.c \
    usb7402_interface.cpp \
    libusb_interface/hidapi.c \
    postprocess.cpp \
    QKeyPushButton.cpp \
    widgetKeyBoard.cpp \
    traffictest.cpp \
    examplemyfocus.cpp

HEADERS += \
    db_engine.h \
    datacontrol.h \
    typedefs.h \
    config.h \
    mainwindow.h\
    qcustomplot.h\
    typedefs.h \
    canbus_interface.h \
    subsystemthread.h \
    gpio_interface.h \
    detailpage.h \
    dashboard.h \
    qcgaugewidget.h \
    libusb_interface/pmd.h \
    libusb_interface/usb-7204.h \
    libusb_interface/test-usb7204.h \
    usb7402_interface.h \
    libusb_interface/hidapi.h \
    postprocess.h \
    QKeyPushButton.h \
    widgetKeyBoard.h \
    traffictest.h \
    examplemyfocus.h

FORMS += \
        mainwindow.ui \
    detailpage.ui \
    dashboard.ui \
    postprocess.ui

QT += serialbus widgets
QT += core
QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CFLAGS += -std=gnu++0x -pthread

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    0
