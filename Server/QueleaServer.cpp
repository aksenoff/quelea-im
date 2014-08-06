#include "QueleaServer.h"
#include <QMessageBox>
#include "../codes.h"

QueleaServer::QueleaServer(const QString& ip, QueleaServerUI* userInterface)
    : ipAddress(ip), port(49212), ui(userInterface)
{
    // starting server
    if (!listen(static_cast<QHostAddress>(ipAddress), port))
    {
        QMessageBox::critical(0, tr("Server error"), tr("Unable to start server") +": " + errorString());
        close();
        return;
    }
    ui->log("<b>" + tr("Server started on") + " " + ipAddress + ":" + QString::number(port) + "</b>");
    connect(this, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection()));
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
                ui->log(tr("Client") + " " + incomingMessage->getText() + " " + tr("connected"));
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
                contacts_string.append(clients[i]->getName()+QChar::Null);
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
            QStringList incomingMessageTextItems(incomingMessage->getText().split(QChar::Null));
            QString receiverClientName(incomingMessageTextItems[0]);
            QString actualMessage(incomingMessageTextItems[1]);
            // constructing message text "sendername0x0000actualmessage"
            QString outcomingMessageText((*senderClient)->getName() + QChar::Null + actualMessage);
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
            QStringList incomingMessageTextItems(incomingMessage->getText().split(QChar::Null));
            QString receiverClientName(incomingMessageTextItems[0]);
            QString actualMessage(incomingMessageTextItems[1]);
            // constructing message text "sendername0x0000receivername0x0000actualmessage"
            QString outcomingMessageText((*senderClient)->getName() + QChar::Null + receiverClientName + QChar::Null + actualMessage);
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
    ui->log(tr("Client") + " " +  (*disconnectedClient)->getName() + " " + tr("disconnected"));
    delete (*disconnectedClient);
    // removing client from vector
    clients.erase(disconnectedClient);
    // constructing new contacts list and broadcasting it
    QString contacts_string = "";
    for (int i = 0; i < clients.size(); ++i)
        contacts_string.append(clients[i]->getName() + QChar::Null);
    Message contacts_list(CONTACTS_RESPONSE, contacts_string);
    for (int i = 0; i < clients.size(); ++i)
        contacts_list.send(clients[i]->getSocket());
}

//-------------------------------------------------------------------------

QueleaServer::~QueleaServer()
{
    ui->log("<b>" + tr("Server stopped") + "</b>");
}
