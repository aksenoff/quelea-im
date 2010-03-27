
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
    QVector<Client> clients;
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
    QString name;
    QTcpSocket* socket;
public:
    QTcpSocket* getsocket(){return socket;};
    QString getname(){return name;};
    Client(const QString& n, QTcpSocket* s):name(n),socket(s){};
    bool operator==(const Client& c)const{
    return(name==c.name);};
    Client(){};
    void send(QByteArray ba);
};

class Message
{
    unsigned char code;
    //QTime time;
    QString text;
public:
    operator int(){return code;};
    QString gettext(){return text;};
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text;};
    friend QDataStream& operator>>(QDataStream& in, Message*& m) {
        unsigned char code;
        QString text;
        QDataStream& ds = in >> code >> text;
        if(!m) m = new Message(code, text);
        return ds;};
    Message(unsigned char c, QString s=""):code(c),text(s){};
};

#endif  //_MyServer_h_
