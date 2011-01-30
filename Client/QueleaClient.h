
#ifndef _QueleaClient_h_
#define _QueleaClient_h_

#include <QWidget>
#include <QTcpSocket>
#include "QueleaClientUI.h"
#include "../message.h"

class QueleaClientUI;

// ======================================================================
class QueleaClient : public QTcpSocket {
Q_OBJECT
private:
    QTcpSocket* tcpSocket;
    QString serverAddress;
    QString clientName;
    QString clientStatus;
    QueleaClientUI* ui;
public:
    QueleaClient(QueleaClientUI*, QString, QString);
    QString getStatus();
    void changeSettings(QString, QString);
    void conn();
    void disconn();
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void changeStatus(QString status);
public slots:
     void sendmess(QString& receiver, QString& messageText);
     void sendchat(QString& receiver, QString& messageText);
signals:
    void startedConnect();
    void sendButtonChangeToChat();
    void sendButtonChangeToPrivate();
    void disconnectSignal();
    void statusChanged(QString status);
    void socketError();
    void authOkSignal();
    void authErrorSignal();
};
#endif
