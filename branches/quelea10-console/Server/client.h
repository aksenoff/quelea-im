#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>

class Client : public QObject {
Q_OBJECT
private:
    QString name;
    QTcpSocket* socket;
signals:
    void goodbye(QTcpSocket*);
public slots:
    void socketClosed();
public:
    QTcpSocket* getSocket() const;
    QString getName() const;
    Client(const QString&, QTcpSocket*);
};

#endif
