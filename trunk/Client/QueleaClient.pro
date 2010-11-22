TEMPLATE = app
QT += network
SOURCES = main.cpp \
    QueleaClient.cpp \
    settingsDialog.cpp
HEADERS = QueleaClient.h \
    ../codes.h \
    settingsDialog.h
win32:TARGET = ../QueleaClient
RC_FILE = resource.rc
