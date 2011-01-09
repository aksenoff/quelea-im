
#ifndef QUELEASERVER_H
#define QUELEASERVER_H

#include "../message.h"
#include <QtNetwork>
#include "QueleaServerUI.h"

class QueleaServerUI;
class Client;

// ======================================================================

class QueleaServer : public QTcpServer {
Q_OBJECT
private:
    quint16 nextBlockSize;
    QString ipAddress;
    quint16 port;
    QVector<Client*> clients;
    QueleaServerUI* ui;
    void sendToSocket(QTcpSocket* socket, Message* message);
    void sendToClient(Client* client, Message* message);
public:
    QueleaServer(QueleaServerUI* userInterface = 0);

public slots:
    //void slotStartServer();
    //void slotStopServer();
    void slotNewConnection();
    void slotReadClient();
    void slotByeClient(QTcpSocket*);
};

class Client: public QWidget {
Q_OBJECT
private:
    QString name;
    QTcpSocket* socket;
signals:
    void goodbye(QTcpSocket*);
public slots:
    void socketClosed();
public:
    QTcpSocket* getsocket(){return socket;}
    QString getname(){return name;}
    Client(const QString&, QTcpSocket*);
    Client(){}
    void send(QByteArray ba);
};

#endif
