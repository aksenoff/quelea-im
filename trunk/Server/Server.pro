TEMPLATE = app
SOURCES = main.cpp \
    MyServer.cpp
QT += network
HEADERS = MyServer.h \
    ../codes.h
win32:TARGET = ../Quelea-Server
RC_FILE = ../Client/myapp.rc
