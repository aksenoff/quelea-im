#include "QueleaServer.h"
#include <QMessageBox>
#include "../codes.h"

// ----------------------------------------------------------------------


QueleaServer::QueleaServer(QueleaServerUI* userInterface)
    : nextBlockSize(0), ipAddress(""), port(49212), ui(userInterface)
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // ищем IP-адрес интерфейса, не являющегося localhost
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // если не нашли, используем localhost
    if (ipAddress == "")
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    // стартуем
    if (!listen(static_cast<QHostAddress>(ipAddress), port)) {
        QMessageBox::critical(0, 
                              tr("Ошибка сервера"),
                              tr("Невозможно запустить сервер:")
                              + errorString()
                             );
        close();
        return;
    }
    ui->log("<b>["+QTime::currentTime().toString()+"]"+" "+tr("Сервер запущен на ")+ipAddress+":"+QString::number(port)+"</b>");
    connect(this, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
           );



}

// ----------------------------------------------------------------------
void QueleaServer::slotNewConnection()
{

    QTcpSocket* pClientSocket = nextPendingConnection();

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

void QueleaServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!nextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < nextBlockSize) {
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
                bool contCollState=false;
                for(int i=0; i<clients.size(); i++)
                   if(clients[i]->getname()==mess->gettext())
                    {
                        contCollState=true;                   
                        break;
                    }

                if (contCollState==true || mess->gettext()=="all"){
                    Message* auth_error = new Message(AUTH_RESPONSE,"auth_error");
                    sendToClient(newclient, auth_error);
                    delete auth_error;
                }


                if(contCollState==false) {

                clients.push_back(newclient);
                connect(newclient,SIGNAL(goodbye(QTcpSocket*)),
                        this, SLOT(slotByeClient(QTcpSocket*)));
                ui->log("["+time.toString()+"]" + " "+tr("Подключен новый клиент ") + mess->gettext());
                Message* auth_ok = new Message(AUTH_RESPONSE,"auth_ok");
                sendToClient(newclient, auth_ok);
                delete auth_ok;
            }
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

                QVector<Client*>::iterator i;
                for(i=clients.begin();(*i)->getname()!=messtoserv[0];++i);
                str=(*from)->getname()+";"+messtoserv[1]+";"+""; // (*from)->getname()=от кого, messtoserv[1]=текст

                Message* newmess = new Message(MESSAGE_TO_CLIENT,str); 
                sendToClient(*i,newmess);
                delete newmess;
                break;
            }


        case MESSAGE_TO_CHAT:
            {
                QVector<Client*>::iterator from;
                for(from=clients.begin();(*from)->getsocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");

                str=(*from)->getname()+";"+messtoserv[0]+";"+messtoserv[1]; // *from)->getname()=от кого, messtoserv[0]=кому, messtoserv[1]=текст сообщения
                Message* newmess = new Message(MESSAGE_TO_CHAT,str);
                for (int u=0;u<clients.size();u++)
                     sendToClient(clients[u], newmess);
                delete newmess;

                break;
            }

        }
        nextBlockSize = 0;

    }
}

//-------------------------------------------------------------------------
void QueleaServer::sendToSocket(QTcpSocket* socket, Message* message)
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
void QueleaServer::sendToClient(Client* client, Message* message)
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

void QueleaServer::slotByeClient(QTcpSocket* s)
{
    QVector<Client*>::iterator dissock;
    for(dissock=clients.begin();(*dissock)->getsocket()!=s;dissock++);
    ui->log("["+QTime::currentTime().toString()+"]" + " "+tr("Клиент ") +  (*dissock)->getname()+tr(" отключен"));
    delete (*dissock);
    clients.erase(dissock);

    QString contacts_string = "";
    for (int i=0; i<clients.size(); i++)
        contacts_string.append(clients[i]->getname()+";");
    Message* contacts_message = new Message(CONTACTS_RESPONSE, contacts_string);
    for (int i=0;i<clients.size();i++)
        sendToClient(clients[i], contacts_message);
    delete contacts_message;


}
