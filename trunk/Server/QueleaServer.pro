TEMPLATE = app
SOURCES = main.cpp \
    QueleaServerUI.cpp \
    QueleaServer.cpp \
    client.cpp \
    ../message.cpp
QT += network
HEADERS = QueleaServer.h \
    QueleaServerUI.h \
    client.h \
    ../codes.h \
    ../message.h
win32:TARGET = ../QueleaServer
RC_FILE = resource.rc

RESOURCES += \
    resources.qrc
