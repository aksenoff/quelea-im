
#include <QtNetwork>
#include <QtGui>
#include "MyServer.h"
#include "../codes.h"

// ----------------------------------------------------------------------


MyServer::MyServer(QWidget* pwgt /*=0*/) : QWidget(pwgt)
                                                    , m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this); 
    if (!m_ptcpServer->listen(QHostAddress::Any, 49212)) {
        QMessageBox::critical(0, 
                              "Server Error", 
                              "Unable to start the server:" 
                              + m_ptcpServer->errorString()
                             );
        m_ptcpServer->close();
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }




    connect(m_ptcpServer, SIGNAL(newConnection()), 
            this,         SLOT(slotNewConnection())
           );

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);


    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;    
    pvbxLayout->addWidget(new QLabel("Server is running on "+ipAddress));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);
}

// ----------------------------------------------------------------------
void MyServer::slotNewConnection()
{

    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()), 
            this,          SLOT(slotReadClient())
           );


    Message *conMess = new Message(1);

    this->sendToSocket(pClientSocket,conMess);
    
}

// ----------------------------------------------------------------------
void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        Message *mess=0; //!
        in >> time >> mess;
        switch(int(*mess))
        {
        case 2: //А если здесь поставить 1, то заработает. Поэтому при передачи CONNECTED клиенту проблему не было, так как код 1.
            m_ptxt->append("test!"); //И это не выполняется
            break;
        }



        m_nNextBlockSize = 0;

    }
}

//-------------------------------------------------------------------------
void MyServer::sendToSocket(QTcpSocket* socket, Message* message)
{

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);


    out << quint16(0) << QTime::currentTime() << message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);



}




// ----------------------------------------------------------------------
void MyServer::sendToClient(Client* client, Message* message)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);


    out << quint16(0) << QTime::currentTime() << message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    client->send(arrBlock);
}

void Client::send(QByteArray ba)
{
    socket->write(ba);
}
