
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
                              QString::fromLocal8Bit("Ошибка сервера"),
                              QString::fromLocal8Bit("Невозможно запустить сервер:")
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
    pvbxLayout->addWidget(new QLabel("["+QTime::currentTime().toString()+"]"+" "+QString::fromLocal8Bit("Сервер запущен на ")+ipAddress));
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
	delete conMess;
    
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
        QTime   time =  QTime::currentTime();
        QString str;
        Message *mess=0; //!
        in >> mess;
        switch(*mess)
        {
        case AUTH_REQUEST:
            {
                Client* newclient = new Client(mess->gettext(), pClientSocket);
                clients.push_back(*newclient);
                m_ptxt->append(time.toString() + " "+QString::fromLocal8Bit("Подключен новый клиент с именем ") + mess->gettext());
                Message* auth_ok = new Message(AUTH_RESPONSE);
                sendToClient(newclient, auth_ok);
                delete auth_ok;
                break;
            }
        case CONTACTS_REQUEST:
            {

                QString contacts_string = "";
                for (int i=0; i<clients.size(); i++)
                    contacts_string.append(clients[i].getname()+";");
                Message* contacts_message = new Message(CONTACTS_RESPONSE, contacts_string);
                for (int i=0;i<clients.size();i++)
                    sendToClient(&clients[i], contacts_message);
                delete contacts_message;
                break;
            }
        case MESSAGE_TO_SERVER:
            {

                QVector<Client>::iterator from;
                for(from=clients.begin();from->getsocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");


                int i = clients.indexOf(Client(messtoserv[0],0));
                str=from->getname()+";"+messtoserv[1]+";"+"";

                Message* newmess = new Message(MESSAGE_TO_CLIENT,str);

                if (messtoserv[0]==QString::fromLocal8Bit(">Все собеседники"))
                    for (int u=0;u<clients.size();u++)
                        sendToClient(&clients[u], newmess);
                else
                sendToClient(&clients[i],newmess);
                break;
            }


        case MESSAGE_TO_CHAT:
            {
                QVector<Client>::iterator from;
                for(from=clients.begin();from->getsocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");



                if (messtoserv[0]==QString::fromLocal8Bit(">Все собеседники"))
                {
                    str=from->getname()+";"+messtoserv[1]+";"+"";
                  Message* newmess = new Message(MESSAGE_TO_CLIENT,str);

                  for (int u=0;u<clients.size();u++)
                        sendToClient(&clients[u], newmess);
                 }

                else
                {
                int i = clients.indexOf(Client(messtoserv[0],0));
                QString nstr = " -> "+clients[i].getname();
                str=from->getname()+";"+messtoserv[1]+";"+nstr;
                Message* newmess = new Message(MESSAGE_TO_CLIENT,str);

                for (int u=0;u<clients.size();u++)
                      sendToClient(&clients[u], newmess);
                 }

                break;



            }

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


    out << quint16(0)<< *message;

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


    out << quint16(0) << *message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    client->send(arrBlock);
}

void Client::send(QByteArray ba)
{
    socket->write(ba);
}
