
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


    Message *conMess = new Message(CONNECTED);

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
        case 2:
        {
            Client* newclient = new Client("");
            newclient->socket=pClientSocket;
            newclient->name=mess->text;
            clients.push_back(newclient);
            m_ptxt->append("new client!");
            Message* contactlist = new Message(5);

            for (int i=0;i<=clients.size();i++) // записываем имена клиентов в отдельный вектор для контакт-листа
            {
            contactlist->contacts[i]=clients[i]->name;
            }


            for (int i=0;i<=clients.size();i++) //отправляем всем клиентам контакт-лист(пока не работает)
            {
                sendToClient(clients[i],contactlist);
           }

        }
        case 7:{}break;//а вот тут должен быть IndexOf, которы у меня не получился. Я добавил в заголовочный  bool operator.
                       //Еще нужно в класс Message добавить свойство sender, а recip (получатель) я уже добавил. Как раз на предмет получателя нужно делать indexOf.
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


    out << quint16(0) <<QTime::currentTime()<< *message;

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


    out << quint16(0) << QTime::currentTime() << *message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    client->send(arrBlock);
}

void Client::send(QByteArray ba)
{
    socket->write(ba);
}
