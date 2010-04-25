TEMPLATE = app
SOURCES = main.cpp \
    QueleaServer.cpp
QT += network
HEADERS = QueleaServer.h \
    ../codes.h
win32:TARGET = ../QueleaServer
RC_FILE = resource.rc
