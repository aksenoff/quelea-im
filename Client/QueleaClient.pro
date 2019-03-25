TEMPLATE = app
QT += network
QT += widgets
QT += multimedia
SOURCES = main.cpp \
    QueleaClient.cpp \
    settingsDialog.cpp \
    systemTray.cpp \
    clientTab.cpp \
    ../message.cpp \
    QueleaClientUI.cpp \
    connectionStateMachine.cpp \
    application.cpp
HEADERS = QueleaClient.h \
    settingsDialog.h \
    systemTray.h \
    clientTab.h \
    ../codes.h \
    ../message.h \
    QueleaClientUI.h \
    connectionStateMachine.h \
    versionInfo.h \
    application.h
win32:TARGET = ../QueleaClient
RC_FILE = resource.rc

RESOURCES += \
    resources.qrc
CONFIG += c++11
TRANSLATIONS += translations/QueleaClient_ru.ts
