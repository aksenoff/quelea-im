
#ifndef _QueleaClient_h_
#define _QueleaClient_h_

#include <QWidget>
#include <QTcpSocket>
#include <QTime>
#include <QtGui>
#include <QStateMachine>
#include "../message.h"
#include "clientTab.h"

class QTextEdit;
class QLineEdit;
class TabWt;

// ======================================================================
class QueleaClient : public QWidget {
Q_OBJECT
private:
    QTcpSocket* tcpSocket;
    QTextEdit*  textInfo;
    QTextEdit*  messInput;
    QListWidget* contlist;
    QInputDialog* ipselect;
    QPushButton* connbutton;
    QPushButton* sendButton;
    QPushButton* settingsButton;
    QPushButton* sendtochat;
    QPushButton* aboutButton;
    QSpacerItem* spacer1,*spacer2,*spacer3, *spacer4, *spacer5;
    QLabel* stateLabel;
    QLabel*  yourCompanionsLabel;
    QString serverAdr;
    QString clientName;
    quint16     nextBlockSize;
    void SendToServer(Message* message);
    QStateMachine connectionStatus;
    QStateMachine sendButtonStatus;
    QShortcut* sendShortcut;
    bool enableSound;
public:
    QueleaClient(QWidget* pwgt = 0) ;
    QString clientStatus;
    ClientTab* tabWidget;
signals:
    void startedConnect();
    void sendButtonChangeToChat();
    void sendButtonChangeToPrivate();
    void toDisconnStateBydisconn();
    void statusChanged(QString status);
    void newMessage(QString receiver);


private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void enableConnected();
    void enableDisconnected();
    void sendmess();
    void sendchat();
    void enableSendButton();
    void addTab(QListWidgetItem*);
    void tabChanged(int tab);
    void closeTab(int index);
    void enableSendChat();
    void enableSendPrivate();
    void sendButtonFunc(int index);
    void changeStatus(QString status);
    void messageReceived(QString receiver);
    void playSound(QString reason);

public slots:
     void openSettingDialog();
     void conn();
     void disconn();
};
#endif
