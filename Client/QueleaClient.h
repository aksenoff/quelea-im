
#ifndef _QueleaClient_h_
#define _QueleaClient_h_

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
    QString clientStatus;
    QueleaClientUI* ui;
public:
    QueleaClient(QueleaClientUI*, const QString&, const QString&);
    QString getStatus() const; //?
    void changeSettings(const QString&, const QString&);
private slots:
    void slotReadServer();
    void slotError(QAbstractSocket::SocketError);
    void changeStatus(const QString&);
public slots:
    void sendPrivateMessage(const QString&, const QString&) const;
    void sendChatMessage(const QString&, const QString&) const;
    void connectToServer();
    void disconnectFromServer();;
signals:
    void startedConnect();
    void sendButtonChangeToChat();
    void sendButtonChangeToPrivate();
    void disconnectSignal();
    void statusChanged(const QString&);
    void socketError();
    void authOkSignal();
    void authErrorSignal();
};
#endif
