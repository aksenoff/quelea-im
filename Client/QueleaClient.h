
#ifndef _QueleaClient_h_
#define _QueleaClient_h_

#include <QWidget>
#include <QTcpSocket>
#include <QTime>
#include <Qtgui>
#include <QStateMachine>

class QTextEdit;
class QLineEdit;
class Message;

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
    QPushButton* pcmd;
    QPushButton* settingsButton;
    QPushButton* sendtochat;
    QPushButton* info;
    QSpacerItem* spacer1,*spacer2;
    QLabel* stateLabel;
    QTabWidget* tabWidget;
    QString serverAdr;
    QString clientName;
    quint16     nextBlockSize;
    void SendToServer(Message* message);
    QStateMachine connectionStatus;
public:
    QueleaClient(QWidget* pwgt = 0) ;
signals:
    void startedConnect();

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void conn();
    void enableConnected();
    void enableDisconnected();
    void disconn();
    void sendmess();
    void sendchat();
    void openSettingDialog();
    void enableSendButton();
    void addTab(QListWidgetItem*);
};

class Message
{
public:
    unsigned char code;

    QString text;
    QVector<QString> contacts;
public:
    Message(unsigned char c, QString s=""):code(c),text(s){};
    operator int(){return code;};
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text;};
    friend QDataStream& operator>>(QDataStream& in, Message*& m) {
        unsigned char code;
        QString text;
        QDataStream& ds = in >> code >> text;
        if(!m) m = new Message(code, text);
        return ds;};
};
#endif
