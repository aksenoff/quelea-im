TEMPLATE = app
SOURCES = main.cpp \
    QueleaServerUI.cpp \
    QueleaServer.cpp \
    client.cpp \
    ../message.cpp \
    database.cpp \
    settingsDialog.cpp \
    dbeditor.cpp \
    ldapauth.cpp
QT += network
QT += sql
QT += widgets
HEADERS = QueleaServer.h \
    QueleaServerUI.h \
    client.h \
    ../codes.h \
    ../message.h \
    versionInfo.h \
    database.h \
    settingsDialog.h \
    dbeditor.h \
    ldapauth.h
LIBS += -L"$$_PRO_FILE_PWD_/libs/" -lldap
win32:TARGET = ../QueleaServer
RC_FILE = resource.rc
INCLUDEPATH = include
RESOURCES += \
    resources.qrc

TRANSLATIONS += translations/QueleaServer_ru.ts
