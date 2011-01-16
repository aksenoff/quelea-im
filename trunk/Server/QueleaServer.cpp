#include "QueleaServer.h"
#include <QMessageBox>
#include "../codes.h"

// ----------------------------------------------------------------------

QueleaServer::QueleaServer(QueleaServerUI* userInterface)
    : nextBlockSize(0), ipAddress(""), port(49212), ui(userInterface)
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // searching for first non-localhost ip
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // not found? using localhost
    if (ipAddress == "")
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    // starting server
    if (!listen(static_cast<QHostAddress>(ipAddress), port))
    {
        QMessageBox::critical(0, tr("Ошибка сервера"), tr("Невозможно запустить сервер:") + errorString());
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
    QTcpSocket* clientSocket = nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()),
            clientSocket, SLOT(deleteLater())
           );
    connect(clientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );
    Message connected(CONNECTED);
    sendToSocket(clientSocket, &connected);
}

// ----------------------------------------------------------------------

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
                   if(clients[i]->getName()==mess->gettext())
                    {
                        contCollState=true;                   
                        break;
                    }

                if (contCollState==true || mess->gettext()=="all"){
                    Message auth_error(AUTH_RESPONSE,"auth_error");
                    sendToSocket(newclient->getSocket(), &auth_error);
                }


                if(contCollState==false) {

                clients.push_back(newclient);
                connect(newclient,SIGNAL(goodbye(QTcpSocket*)),
                        this, SLOT(slotByeClient(QTcpSocket*)));
                ui->log("["+time.toString()+"]" + " "+tr("Подключен новый клиент ") + mess->gettext());
                Message auth_ok(AUTH_RESPONSE,"auth_ok");
                sendToSocket(newclient->getSocket(), &auth_ok);
            }
                break;


            }
        case CONTACTS_REQUEST:
            {

                QString contacts_string("");
                for (int i=0; i<clients.size(); i++)
                    contacts_string.append(clients[i]->getName()+";");
                Message contacts_list(CONTACTS_RESPONSE, contacts_string);
                for (int i=0;i<clients.size();i++)
                    sendToSocket(clients[i]->getSocket(), &contacts_list);
                break;
            }
        case MESSAGE_TO_SERVER:
            {

                QVector<Client*>::iterator from;
                for(from=clients.begin();(*from)->getSocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");

                QVector<Client*>::iterator i;
                for(i=clients.begin();(*i)->getName()!=messtoserv[0];++i);
                str=(*from)->getName()+";"+messtoserv[1]+";"+""; // (*from)->getname()=от кого, messtoserv[1]=текст

                Message newmess(MESSAGE_TO_CLIENT, str);
                sendToSocket((*i)->getSocket(), &newmess);
                break;
            }


        case MESSAGE_TO_CHAT:
            {
                QVector<Client*>::iterator from;
                for(from=clients.begin();(*from)->getSocket()!=pClientSocket;from++);

                QStringList messtoserv = mess->gettext().split(";");

                str=(*from)->getName()+";"+messtoserv[0]+";"+messtoserv[1]; // *from)->getname()=от кого, messtoserv[0]=кому, messtoserv[1]=текст сообщения
                Message newmess(MESSAGE_TO_CHAT, str);
                for (int u=0;u<clients.size();u++)
                     sendToSocket(clients[u]->getSocket(), &newmess);
                break;
            }

        }
        nextBlockSize = 0;

    }
}

//-------------------------------------------------------------------------
void QueleaServer::sendToSocket(QTcpSocket* const socket, const Message* const message) const
{

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);


    out << quint16(0)<< *message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);

}

void QueleaServer::slotByeClient(QTcpSocket* s)
{
    QVector<Client*>::iterator dissock;
    for(dissock=clients.begin();(*dissock)->getSocket()!=s;dissock++);
    ui->log("["+QTime::currentTime().toString()+"]" + " "+tr("Клиент ") +  (*dissock)->getName()+tr(" отключен"));
    delete (*dissock);
    clients.erase(dissock);

    QString contacts_string = "";
    for (int i=0; i<clients.size(); i++)
        contacts_string.append(clients[i]->getName()+";");
    Message contacts_list(CONTACTS_RESPONSE, contacts_string);
    for (int i=0;i<clients.size();i++)
        sendToSocket(clients[i]->getSocket(), &contacts_list);
}
