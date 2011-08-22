TEMPLATE = app
SOURCES = main.cpp \
    QueleaServerUI.cpp \
    QueleaServer.cpp \
    client.cpp \
    ../message.cpp \
    database.cpp \
    settingsDialog.cpp \
    dbeditor.cpp
QT += network
QT += sql
HEADERS = QueleaServer.h \
    QueleaServerUI.h \
    client.h \
    ../codes.h \
    ../message.h \
    versionInfo.h \
    database.h \
    settingsDialog.h \
    dbeditor.h
win32:TARGET = ../QueleaServer
RC_FILE = resource.rc

RESOURCES += \
    resources.qrc

TRANSLATIONS += translations/QueleaServer_ru.ts
