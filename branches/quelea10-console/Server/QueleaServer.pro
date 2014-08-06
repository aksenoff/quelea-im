TEMPLATE = app
CONFIG += console
SOURCES = main.cpp \
    QueleaServer.cpp \
    client.cpp \
    ../message.cpp \
    QueleaServerControl.cpp
QT += network
QT -= gui
HEADERS = QueleaServer.h \
    client.h \
    ../codes.h \
    ../message.h \
    versionInfo.h \
    QueleaServerControl.h
win32:TARGET = ../QueleaServer
RC_FILE = resource.rc

RESOURCES += \
    resources.qrc

TRANSLATIONS += translations/QueleaServer_ru.ts
