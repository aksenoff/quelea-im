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
    enum {GUEST_AUTH, DB_AUTH, LDAP_AUTH};
    QTcpSocket* serverSocket;
    QString serverAddress;
    QString clientName;
    QString* uName;
    QString* uPassword;
    QueleaClientUI* ui;
    int authType;
public:
    QueleaClient(QueleaClientUI*);
    void changeSettings(const int&, const QString&, const QString&);
    void changeSettings(const int&, const QString&, const QString&, const QString&);
private slots:
    void slotReadServer();
    void slotError(QAbstractSocket::SocketError);
public slots:
    void sendPrivateMessage(const QString&, const QString&) const;
    void sendChatMessage(const QString&, const QString&) const;
    void connectToServer();
    void disconnectFromServer();
    void sendFile(QString filename);
    void receiveFile(){}
signals:
    void socketError();
    void authOkSignal();
    void authErrorSignal();
};

#endif
