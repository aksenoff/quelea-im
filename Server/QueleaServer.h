
#ifndef _QueleaServer_h_
#define _QueleaServer_h_

#include <QWidget>
#include <QTime>

class QTcpServer;
class QTextEdit;
class QTcpSocket;
class QLabel;
class Client;

// ======================================================================
class Message;

class QueleaServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* tcpServer;
    QTextEdit*  textInfo;
    quint16     nextBlockSize;
    QVector<Client*> clients;
    void sendToSocket(QTcpSocket* socket, Message* message);
    void sendToClient(Client* client, Message* message);
public:
    QueleaServer(QWidget* pwgt = 0);

public slots:
    void slotNewConnection();
    void slotReadClient();
    void slotByeClient(QTcpSocket*);
};

class Client: public QWidget {
Q_OBJECT
private:
    QString name;
    QTcpSocket* socket;
signals:
    void goodbye(QTcpSocket*);
public slots:
    void socketClosed();
public:
    QTcpSocket* getsocket(){return socket;}
    QString getname(){return name;}
    Client(const QString&, QTcpSocket*);
    Client(){}
    void send(QByteArray ba);
};

class Message
{
    unsigned char code;
    QString text;
public:
    operator int(){return code;}
    QString gettext(){return text;}
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text;}
    friend QDataStream& operator>>(QDataStream& in, Message*& m) {
        unsigned char code;
        QString text;
        QDataStream& ds = in >> code >> text;
        if(!m) m = new Message(code, text);
        return ds;}
    Message(unsigned char c, QString s=""):code(c),text(s){}
};

#endif
