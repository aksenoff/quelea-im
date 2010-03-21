
#ifndef _MyServer_h_
#define _MyServer_h_

#include <QWidget>
#include <QTime>

class QTcpServer;
class QTextEdit;
class QTcpSocket;
class QLabel;
class Client;

// ======================================================================
class Message;

class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    QLabel* info;
    quint16     m_nNextBlockSize;
    QVector<Client *> clients;
private:
    void sendToSocket(QTcpSocket* socket, Message* message);
    void sendToClient(Client* client, Message* message);

public:
    MyServer(QWidget* pwgt = 0);

public slots:
            void slotNewConnection();
            void slotReadClient   ();
};

class Client
{
public:
    Client(QString n):name(n){};
    bool operator==(const Client& c)const{
    return(name==c.name);};
public:
    QString name;
    QTcpSocket* socket;
public:
    //Client();
    void send(QByteArray ba);
};

class Message
{
public:
    unsigned char code;
    //QTime time;
    QString text;
    QString recip;
    QVector<QString> contacts;
public:
    operator int(){return code;};
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text<<m.contacts;};
    friend QDataStream& operator>>(QDataStream& in, Message*& m) {
        unsigned char code;
        QString text;
        QDataStream& ds = in >> code >> text;
        if(!m) m = new Message(code, text);
        return ds;};
    Message(unsigned char c, QString s=""):code(c),text(s){};
};

#endif  //_MyServer_h_
