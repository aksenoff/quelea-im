/* ======================================================================
**  MyServer.h
** ======================================================================
**
** ======================================================================
**  Copyright (c) 2007 by Max Schlee
** ======================================================================
*/
#ifndef _MyServer_h_
#define _MyServer_h_

#include <QWidget>

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

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str, const QString& clname);

public:
    MyServer(QWidget* pwgt = 0);

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
};

class Client
{
private:
    QString name;
    QTcpSocket* socket;
};

#endif  //_MyServer_h_
