
#include <QtNetwork>
#include <QtGui>
#include "MyServer.h"
#include "../codes.h"

// ----------------------------------------------------------------------


MyServer::MyServer(QWidget* pwgt /*=0*/) : QWidget(pwgt)
                                                    , NextBlockSize(0)
{


    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 49212)) {
        QMessageBox::critical(0, 
                              QString::fromLocal8Bit("Ошибка сервера"),
                              QString::fromLocal8Bit("Невозможно запустить сервер:")
                              + tcpServer->errorString()
                             );
        tcpServer->close();
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




    connect(tcpServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
           );

    textInfo = new QTextEdit;
    textInfo->setReadOnly(true);


    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;    
    pvbxLayout->addWidget(new QLabel("["+QTime::currentTime().toString()+"]"+" "+QString::fromLocal8Bit("Сервер запущен на ")+ipAddress));
    pvbxLayout->addWidget(textInfo);
    setLayout(pvbxLayout);


}

// ----------------------------------------------------------------------
void MyServer::slotNewConnection()
{

    QTcpSocket* pClientSocket = tcpServer->nextPendingConnection();

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

Client::Client(const QString &n, QTcpSocket *s):name(n),socket(s){
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(socketClosed()));

}

void Client::socketClosed()
{
    emit goodbye(socket);
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!NextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> NextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < NextBlockSize) {
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
                clients.push_back(newclient);
                connect(newclient,SIGNAL(goodbye(QTcpSocket*)),
                        this, SLOT(slotByeClient(QTcpSocket*)));
                textInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("Подключен новый клиент с именем ") + mess->gettext());
                Message* auth_ok = new Message(AUTH_RESPONSE);
                sendToClient(newclient, auth_ok);
                delete auth_ok;
                break;
            }
        case CONTACTS_REQUEST:
            {

                QString contacts_string = "";
                for (int i=0; i<clients.size(); i++)
                    contacts_string.append(clients[i]->getname()+";");
                Message* contacts_message = new Message(CONTACTS_RESPONSE, contacts_string);
                for (int i=0;i<clients.size();i++)
                    sendToClient(clients[i], contacts_message);
                delete contacts_message;
                break;
            }
        case MESSAGE_TO_SERVER:
            {

                QVector<Client*>::iterator from;
                for(from=clients.begin();(*from)->getsocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");


                int i = clients.indexOf(&Client(messtoserv[0],0)); // aksenoff: ok,'cos we deal with member, not the address
                str=(*from)->getname()+";"+messtoserv[1]+";"+"";      // (needs refactoring)

                Message* newmess = new Message(MESSAGE_TO_CLIENT,str); 
                sendToClient(clients[i],newmess);
                break;
            }


        case MESSAGE_TO_CHAT:
            {
                QVector<Client*>::iterator from;
                for(from=clients.begin();(*from)->getsocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");



                if (messtoserv[0]==QString::fromLocal8Bit(">Все собеседники"))
                {
                    str=(*from)->getname()+";"+messtoserv[1]+";"+"";
                  Message* newmess = new Message(MESSAGE_TO_CLIENT,str);

                  for (int u=0;u<clients.size();u++)
                        sendToClient(clients[u], newmess);
                }
                else
                {
                    QVector<Client*>::iterator i;
                    for(i=clients.begin();(*i)->getname()!=messtoserv[0];++i);
                    QString nstr = " -> "+(*i)->getname();
                    str=(*from)->getname()+";"+messtoserv[1]+";"+nstr;
                    Message* newmess = new Message(MESSAGE_TO_CLIENT,str);
                    for (int u=0;u<clients.size();u++)
                        sendToClient(clients[u], newmess);
                    delete newmess;
                }
                break;
            }

        }
        NextBlockSize = 0;

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

void MyServer::slotByeClient(QTcpSocket* s)
{
    QVector<Client*>::iterator dissock;
    for(dissock=clients.begin();(*dissock)->getsocket()!=s;dissock++);
    clients.erase(dissock);
    textInfo->append("["+QTime::currentTime().toString()+"]" + " "+QString::fromLocal8Bit("Клиент ") +  (*dissock)->getname()+QString::fromLocal8Bit(" отключен"));

    QString contacts_string = "";
    for (int i=0; i<clients.size(); i++)
        contacts_string.append(clients[i]->getname()+";");
    Message* contacts_message = new Message(CONTACTS_RESPONSE, contacts_string);
    for (int i=0;i<clients.size();i++)
        sendToClient(clients[i], contacts_message);
    delete contacts_message;


}
