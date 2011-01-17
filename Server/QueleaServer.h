
#ifndef QUELEASERVER_H
#define QUELEASERVER_H

#include "../message.h"
#include <QtNetwork>
#include "QueleaServerUI.h"
#include "client.h"

class QueleaServerUI;

class QueleaServer : public QTcpServer {
Q_OBJECT
private:
    quint16 nextBlockSize;
    QString ipAddress;
    quint16 port;
    QVector<Client*> clients;
    QueleaServerUI* ui;
public:
    QueleaServer(QueleaServerUI* userInterface = 0);

public slots:
    //void slotStartServer();
    //void slotStopServer();
    void slotNewConnection();
    void slotReadClient();
    void slotByeClient(QTcpSocket*);
};

#endif
