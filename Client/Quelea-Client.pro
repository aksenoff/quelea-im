TEMPLATE = app
QT += network
SOURCES = main.cpp \
    QueleaClient.cpp
HEADERS = QueleaClient.h \
    ../codes.h
win32:TARGET = ../QueleaClient
RC_FILE = resource.rc
