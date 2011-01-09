TEMPLATE = app
SOURCES = main.cpp \
    QueleaServerUI.cpp \
    QueleaServer.cpp \
    ../message.cpp
QT += network
HEADERS = QueleaServer.h \
    QueleaServerUI.h \
    ../codes.h \
    ../message.h
win32:TARGET = ../QueleaServer
RC_FILE = resource.rc
