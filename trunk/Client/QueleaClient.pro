TEMPLATE = app
QT += network
SOURCES = main.cpp \
    QueleaClient.cpp \
    settingsDialog.cpp \
    systemTray.cpp
HEADERS = QueleaClient.h \
    ../codes.h \
    settingsDialog.h \
    systemTray.h
win32:TARGET = ../QueleaClient
RC_FILE = resource.rc
