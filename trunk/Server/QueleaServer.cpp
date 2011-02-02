#include "QueleaServer.h"
#include <QMessageBox>
#include "../codes.h"

QueleaServer::QueleaServer(QueleaServerUI* userInterface)
    : ipAddress(""), port(49212), ui(userInterface)
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // searching for first non-localhost ip
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address())
        {
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
    ui->log("<b>[" + QTime::currentTime().toString() + "]" + " "
            + tr("Сервер запущен на") + " " + ipAddress + ":" + QString::number(port) + "</b>");
    connect(this, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection())
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
    connected.send(clientSocket);
}

// ----------------------------------------------------------------------

void QueleaServer::slotReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    Message* incomingMessage = new Message(clientSocket);
    switch(*incomingMessage)
    {
    case AUTH_REQUEST:
        {
            Client* newClient = new Client(incomingMessage->getText(), clientSocket);
            bool contCollState = false;
            // finding out if chosen name is already used by another client
            for(int i = 0; i < clients.size(); ++i)
                if(clients[i]->getName() == incomingMessage->getText())
                {
                    contCollState = true;
                    break;
                }
            // if yes then sending an error message
            if (contCollState == true || incomingMessage->getText() == "all")
            {
                Message auth_error(AUTH_RESPONSE, "auth_error");
                auth_error.send(newClient->getSocket());
                delete newClient;
            }
            else
            {
                // adding new client to vector
                clients.push_back(newClient);
                connect(newClient, SIGNAL(goodbye(QTcpSocket*)),
                        this, SLOT(slotByeClient(QTcpSocket*)));
                ui->log("[" + QTime::currentTime().toString() + "]" + " "
                        + tr("Подключен новый клиент ") + incomingMessage->getText());
                // sending authorization confirmation
                Message auth_ok(AUTH_RESPONSE, "auth_ok");
                auth_ok.send(newClient->getSocket());
            }
            break;
        }
    case CONTACTS_REQUEST:
        {
            QString contacts_string("");
            // constructing string of client names
            for (int i = 0; i < clients.size(); ++i)
                contacts_string.append(clients[i]->getName()+";");
            Message contacts_list(CONTACTS_RESPONSE, contacts_string);
            // broadcasting
            for (int i = 0; i < clients.size(); ++i)
                contacts_list.send(clients[i]->getSocket());
            break;
        }
    case MESSAGE_TO_SERVER:
        {
            QVector<Client*>::iterator senderClient, receiverClient;
            // searching vector for client who sent message via socket
            for(senderClient = clients.begin(); (*senderClient)->getSocket() != clientSocket; ++senderClient);
            // list: [0] - receiver name [1] - actual message
            QStringList incomingMessageTextItems(incomingMessage->getText().split(";"));
            QString receiverClientName(incomingMessageTextItems[0]);
            QString actualMessage(incomingMessageTextItems[1]);
            // constructing message text "sendername;actualmessage"
            QString outcomingMessageText((*senderClient)->getName() + ";" + actualMessage);
            Message outcomingMessage(MESSAGE_TO_CLIENT, outcomingMessageText);
            // searching vector for receiver client by name
            for(receiverClient = clients.begin(); (*receiverClient)->getName() != receiverClientName; ++receiverClient);
            outcomingMessage.send((*receiverClient)->getSocket());
            break;
        }
    case MESSAGE_TO_CHAT:
        {
            QVector<Client*>::iterator senderClient;
            // searching vector for client who sent message via socket
            for(senderClient = clients.begin(); (*senderClient)->getSocket() != clientSocket; ++senderClient);
            // list: [0] - receiver name [1] - actual message
            QStringList incomingMessageTextItems(incomingMessage->getText().split(";"));
            QString receiverClientName(incomingMessageTextItems[0]);
            QString actualMessage(incomingMessageTextItems[1]);
            // constructing message text "sendername;receivername;actualmessage"
            QString outcomingMessageText((*senderClient)->getName() + ";" + receiverClientName + ";" + actualMessage);
            Message outcomingMessage(MESSAGE_TO_CHAT, outcomingMessageText);
            // broadcasting
            for (int i = 0; i < clients.size(); ++i)
                outcomingMessage.send(clients[i]->getSocket());
            break;
        }

    }
    delete incomingMessage;
}

//-------------------------------------------------------------------------

void QueleaServer::slotByeClient(QTcpSocket* disconnectedClientSocket)
{
    QVector<Client*>::iterator disconnectedClient;
    // searching for client whose socket has disconnected
    for(disconnectedClient = clients.begin(); (*disconnectedClient)->getSocket() != disconnectedClientSocket; ++disconnectedClient);
    ui->log("[" + QTime::currentTime().toString() + "]" + " "
            + tr("Клиент") + " " +  (*disconnectedClient)->getName() + " " + tr("отключен"));
    delete (*disconnectedClient);
    // removing client from vector
    clients.erase(disconnectedClient);
    // constructing new contacts list and broadcasting it
    QString contacts_string = "";
    for (int i = 0; i < clients.size(); ++i)
        contacts_string.append(clients[i]->getName()+";");
    Message contacts_list(CONTACTS_RESPONSE, contacts_string);
    for (int i = 0; i < clients.size(); ++i)
        contacts_list.send(clients[i]->getSocket());
}

//-------------------------------------------------------------------------

QueleaServer::~QueleaServer()
{
    ui->log("<b>[" + QTime::currentTime().toString() + "]" + " " + tr("Сервер остановлен.") + "</b>");
}
