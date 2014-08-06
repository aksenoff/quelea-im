#ifndef QUELEACLIENT_H
#define QUELEACLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include "QueleaClientUI.h"
#include "../message.h"

class QueleaClientUI;

class QueleaClient : public QTcpSocket {
Q_OBJECT
private:
    QTcpSocket* serverSocket;
    QString serverAddress;
    QString clientName;
    QueleaClientUI* ui;
public:
    QueleaClient(QueleaClientUI*, const QString&, const QString&);
    void changeSettings(const QString&, const QString&);
private slots:
    void slotReadServer();
    void slotError(QAbstractSocket::SocketError);
public slots:
    void sendPrivateMessage(const QString&, const QString&) const;
    void sendChatMessage(const QString&, const QString&) const;
    void connectToServer();
    void disconnectFromServer();
signals:
    void socketError();
    void authOkSignal();
    void authErrorSignal();
};

#endif
