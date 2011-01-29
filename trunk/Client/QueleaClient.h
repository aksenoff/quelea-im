
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
    QString serverAdr;
    QString clientName;
    QString clientStatus;
    QueleaClientUI* ui;
    quint16     nextBlockSize;
public:
    QueleaClient(QueleaClientUI*);
    void setSettings(QString name, QString server);
    QString getStatus();

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void changeStatus(QString status);

public slots:
     void conn();
     void disconn();
     void sendmess(QString receiver, QString messageText);
     void sendchat(QString receiver, QString messageText);

signals:
    void startedConnect();
    void sendButtonChangeToChat();
    void sendButtonChangeToPrivate();
    void disconnectSignal();
    void statusChanged(QString status);
    void socketError();
};
#endif
