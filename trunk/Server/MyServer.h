
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
class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    QLabel* info;
    quint16     m_nNextBlockSize;
    QList<Client> clients;
private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    MyServer(QWidget* pwgt = 0);

public slots:
            void slotNewConnection();
            void slotReadClient   ();
};

class Client
{
private:
    QString name;
    QTcpSocket* socket;
public:
    //Client();
};

class Message
{
private:
    unsigned char code;
    QTime time;
    QString text;
public:
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text;};
    friend QDataStream& operator>>(QDataStream& in, Message& m) {return in >> m.code >> m.text;};
};

#endif  //_MyServer_h_
