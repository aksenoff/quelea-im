
#ifndef _MyClient_h_
#define _MyClient_h_

#include <QWidget>
#include <QTcpSocket>
#include <QTime>

class QTextEdit;
class QLineEdit;

// ======================================================================
class MyClient : public QWidget {
Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit*  m_ptxtInfo;
    QLineEdit*  m_ptxtInput;
    QLineEdit*  clname;
    QLineEdit*  ipadr;
    quint16     m_nNextBlockSize;

public:
    MyClient(QWidget* pwgt = 0) ;

private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer(                            );
    void slotConnected   (                            );
    void conn();
};

class Message
{
public:
    unsigned char code;
    QTime time;
    QString text;
public:
    friend QDataStream& operator<<(QDataStream& out, const Message& m) {return out << m.code << m.text;};
    friend QDataStream& operator>>(QDataStream& in, Message& m) {return in >> m.code >> m.text;};
};
#endif  //_MyClient_h_
