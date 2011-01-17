TEMPLATE = app
QT += network
SOURCES = main.cpp \
    QueleaClient.cpp \
    settingsDialog.cpp \
    systemTray.cpp \
    ../message.cpp
HEADERS = QueleaClient.h \
    settingsDialog.h \
    systemTray.h \
    ../codes.h \
    ../message.h
win32:TARGET = ../QueleaClient
RC_FILE = resource.rc
