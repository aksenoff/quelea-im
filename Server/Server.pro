TEMPLATE = app
SOURCES = main.cpp \
    MyServer.cpp
QT += network
HEADERS = MyServer.h \
    ../codes.h
win32:TARGET = ../MyServer
