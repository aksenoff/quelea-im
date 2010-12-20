
#ifndef _QueleaClient_h_
#define _QueleaClient_h_

#include <QWidget>
#include <QTcpSocket>
#include <QTime>
#include <QtGui>
#include <QStateMachine>

class QTextEdit;
class QLineEdit;
class Message;
class TabWt;

// ======================================================================
class QueleaClient : public QWidget {
Q_OBJECT
private:
    QTcpSocket* tcpSocket;
    QTextEdit*  textInfo;
    QLineEdit*  messInput;
    QListWidget* contlist;
    QInputDialog* ipselect;
    QPushButton* connbutton;
    QPushButton* sendButton;
    QPushButton* settingsButton;
    QPushButton* sendtochat;
    QPushButton* info;
    QSpacerItem* spacer1,*spacer2;
    QWidget *spacer3, *spacer4;
    QLabel* stateLabel;
    QLabel*  yourCompanionsLabel;
    TabWt* tabWidget;
    QString serverAdr;
    QString clientName;
    quint16     nextBlockSize;
    void SendToServer(Message* message);
    QStateMachine connectionStatus;
    QStateMachine sendButtonStatus;
    QShortcut* sendShortcut;
public:
    QueleaClient(QWidget* pwgt = 0) ;
    QString clientStatus;
signals:
    void startedConnect();
    void sendButtonChangeToChat();
    void sendButtonChangeToPrivate();
    void toDisconnStateBydisconn();
    void statusChanged(QString status);


private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void enableConnected();
    void enableDisconnected();
    void sendmess();
    void sendchat();
    void enableSendButton();
    void addTab(QListWidgetItem*);
    void normalizeTabColor(int tab);
    void closeTab(int index);
    void enableSendChat();
    void enableSendPrivate();
    void sendButtonFunc(int index);
    void changeStatus(QString status);

public slots:
     void openSettingDialog();
     void conn();
     void disconn();
};

class Message
{
public:
    unsigned char code;

    QString text;
    QVector<QString> contacts;
public:
    Message(unsigned char c, QString s=""):code(c),text(s){}
    operator int(){return code;}
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text;}
    friend QDataStream& operator>>(QDataStream& in, Message*& m) {
        unsigned char code;
        QString text;
        QDataStream& ds = in >> code >> text;
        if(!m) m = new Message(code, text);
        return ds;}
};

class TabWt : public QTabWidget
{
public:
    QTabBar * gettabbar()  const
    {
    return tabBar();
    }
};
#endif
