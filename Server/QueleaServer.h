
#ifndef QUELEASERVER_H
#define QUELEASERVER_H

#include "../message.h"
#include <QtNetwork>
#include "QueleaServerControl.h"
#include "client.h"

class QueleaServerControl;

class QueleaServer : public QTcpServer {
Q_OBJECT
private:
    QHostAddress ipAddress;
    quint16 port;
    QVector<Client*> clients;
    QueleaServerControl* control;
public:
    QueleaServer(const QHostAddress&, QueleaServerControl*);
    ~QueleaServer();
public slots:
    void slotNewConnection();
    void slotReadClient();
    void slotByeClient(QTcpSocket*);
};

#endif
