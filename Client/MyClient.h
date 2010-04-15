
#ifndef _MyClient_h_
#define _MyClient_h_

#include <QWidget>
#include <QTcpSocket>
#include <QTime>
#include <Qtgui>

class QTextEdit;
class QLineEdit;
class Message;

// ======================================================================
class MyClient : public QWidget {
Q_OBJECT
private:
    QTcpSocket* TcpSocket;
    QTextEdit*  TextInfo;
    QLineEdit*  messInput;
    QComboBox*  clname;
    QListWidget* contlist;
    QInputDialog* ipselect;
    QPushButton* connbutton;
    QPushButton* pcmd;
    QPushButton* sendtochat;
    QPushButton* info;
    QSpacerItem* spacer1,*spacer2;
    quint16     NextBlockSize;
    void SendToServer(Message* message);

public:
    MyClient(QWidget* pwgt = 0) ;

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void conn(QString ipadr);
    void disconn();
    void sendmess();
    void sendchat();
    void opendial();
    void enableConnButton();
    void enableSendButton();
};

class Message
{
public:
    unsigned char code;

    QString text;
    QString recip;
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
