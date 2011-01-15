#ifndef CLIENT_H
#define CLIENT_H

#include <QtGui>
#include <QtNetwork>

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
    QTcpSocket* getSocket();
    QString getName();
    Client(const QString&, QTcpSocket*);
    void send(QByteArray);
};

#endif
