# app.pro
QT += core gui network qml quick positioning location widgets charts
CONFIG += c++17

TARGET = stacje_pomiarowe

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += \
    qml.qrc

DISTFILES += \
    StationMeasure.qml \
    main.qml \
    StationList.qml \
    StationInfo.qml \
    StationMeasure.qml
