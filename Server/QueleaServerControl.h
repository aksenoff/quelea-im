#ifndef QUELEASERVERUI_H
#define QUELEASERVERUI_H

#include "QueleaServer.h"

class QueleaServer;

class QueleaServerControl {
private:
    QueleaServer* server;
public:
    QueleaServerControl();
    void log(const QString&) const;
    ~QueleaServerControl();
    void startServer();
private slots:
    void stopServer();
};

#endif
