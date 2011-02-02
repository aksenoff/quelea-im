TEMPLATE = app
QT += network
SOURCES = main.cpp \
    QueleaClient.cpp \
    settingsDialog.cpp \
    systemTray.cpp \
    clientTab.cpp \
    ../message.cpp \
    QueleaClientUI.cpp \
    connectionStateMachine.cpp
HEADERS = QueleaClient.h \
    settingsDialog.h \
    systemTray.h \
    clientTab.h \
    ../codes.h \
    ../message.h \
    QueleaClientUI.h \
    connectionStateMachine.h
win32:TARGET = ../QueleaClient
RC_FILE = resource.rc

RESOURCES += \
    resources.qrc
