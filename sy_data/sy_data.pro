#-------------------------------------------------
#
# Project created by QtCreator 2023-08-10 T 14:08:04
#
#-------------------------------------------------

QT       += core gui sql network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS += -source-charset:utf-8

SOURCES += \
    database.cpp \
    devices.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    database.h \
    devices.h \
    mainwindow.h

FORMS += \
    database.ui \
    devices.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../img/picture.qrc

DISTFILES += \
    ../img/app.png \
    ../img/auto.png \
    ../img/bg.jpg \
    ../img/big.png \
    ../img/btncheckoff1.png \
    ../img/btncheckon1.png \
    ../img/clear.png \
    ../img/clearS.png \
    ../img/clearS_a.png \
    ../img/clear_a.png \
    ../img/clear_b.png \
    ../img/clear_yu.png \
    ../img/client.png \
    ../img/close.png \
    ../img/connect.png \
    ../img/ctrl.ico \
    ../img/ctrl.png \
    ../img/data.png \
    ../img/data_nie.png \
    ../img/device_close.png \
    ../img/device_close_1.png \
    ../img/device_open.png \
    ../img/discon.png \
    ../img/duan.png \
    ../img/exit.png \
    ../img/hand.png \
    ../img/humi.png \
    ../img/humi_soil.png \
    ../img/ip.png \
    ../img/led_off.png \
    ../img/led_on.png \
    ../img/light.png \
    ../img/mq2.png \
    ../img/net_off.png \
    ../img/net_on.png \
    ../img/open.png \
    ../img/rain.png \
    ../img/relay_off.png \
    ../img/relay_on.png \
    ../img/rest.png \
    ../img/send.png \
    ../img/send_a.png \
    ../img/server.png \
    ../img/set.png \
    ../img/set_li.png \
    ../img/small.png \
    ../img/switch.png \
    ../img/temp.png \
    ../img/wifi_off.png \
    ../img/wifi_on.png

RC_ICONS = icon.ico

win32: LIBS += -L$$PWD/../../../qmqtt-master/build-qmqtt-Desktop_Qt_5_9_9_MinGW_32bit-Release/lib/ -lQt5Qmqtt

INCLUDEPATH += $$PWD/../../../qmqtt-master/qmqtt-master/src/mqtt
DEPENDPATH += $$PWD/../../../qmqtt-master/qmqtt-master/src/mqtt
