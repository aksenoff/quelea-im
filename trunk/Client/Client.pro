TEMPLATE = app
QT += network
SOURCES = main.cpp \
    MyClient.cpp
HEADERS = MyClient.h \
    ../codes.h
win32:TARGET = ../Quelea-Client
