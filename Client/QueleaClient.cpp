#include <QtNetwork>
#include <QtGui>
#include "QueleaClient.h"
#include "settingsDialog.h"
#include "../codes.h"
#include "../message.h"

QueleaClient::QueleaClient(QueleaClientUI* UI, const QString& cn, const QString& sa)
    : serverAddress(sa), clientName(cn), ui(UI)
{
    serverSocket = new QTcpSocket(this);
    connect(serverSocket, SIGNAL(readyRead()),
            this, SLOT(slotReadServer()));
    connect(serverSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
}

// ---------------------------------------------------------------------

void QueleaClient::connectToServer()
{
    serverSocket->connectToHost(serverAddress, 49212);
}

// ----------------------------------------------------------------------

void QueleaClient::slotReadServer()
{
    Message incomingMessage(serverSocket);
    switch(incomingMessage)
    {
    case CONNECTED:
        {
        ui->log("<FONT COLOR=GRAY>[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "] " + tr("Connected") + "</FONT>");
            // if we're connected, send an authorization request
            Message auth_req(AUTH_REQUEST, clientName);
            auth_req.send(serverSocket);
            break;
        }
    case AUTH_RESPONSE:
        {
            if(incomingMessage.getText()=="auth_ok")
            {
                ui->log("<FONT COLOR=GRAY>[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "] " + tr("Authorized") + "</FONT>");
                // if we're authorized, request a list of other clients connected to the server
                Message contacts_req(CONTACTS_REQUEST);
                contacts_req.send(serverSocket);
                emit authOkSignal();
            }
            if(incomingMessage.getText()=="auth_error")
            {
                ui->log("<FONT COLOR=BLUE>[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "]</FONT>"
                        + " <FONT COLOR=RED>" + tr("Error: Such name is already in use") + "." + "</FONT>");
                emit authErrorSignal();
            }
            break;
        }
    default:
        // if received message is of another type, delegate its' parsing to ui
        ui->parseMessage(incomingMessage);
    }
}

// ----------------------------------------------------------------------

void QueleaClient::slotError(QAbstractSocket::SocketError err)
{
    QString error = "<FONT COLOR=BLUE>[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "]</FONT>"
                    + " <FONT COLOR=RED>" + tr("Error") + ": " +
                    (err == QAbstractSocket::HostNotFoundError ?
                     tr("The host was not found") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     tr("The remote host is closed") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     tr("The connection was refused") :
                     QString(serverSocket->errorString())
                    ) + "</FONT>";
    emit socketError();
    // send an error message to chat
    ui->log(error);
}

// ------------------------------------------------------------------

void QueleaClient::sendPrivateMessage(const QString& receiverName, const QString& actualMessage) const
{
    // constructing message text "receivername0x0000actualmessage"
    QString messageText = receiverName + QChar::Null + actualMessage;
    Message outcomingMessage(MESSAGE_TO_SERVER, messageText);
    outcomingMessage.send(serverSocket);
}

// ----------------------------------------------------------------------

void QueleaClient::sendChatMessage(const QString& receiverName, const QString& actualMessage) const
{
    // constructing message text "receivername0x0000actualmessage"
    QString messageText = receiverName + QChar::Null + actualMessage;
    Message outcomingMessage(MESSAGE_TO_CHAT, messageText);
    outcomingMessage.send(serverSocket);
}

// ----------------------------------------------------------------------

void QueleaClient::disconnectFromServer()
{
    ui->log("<FONT COLOR=GRAY>[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "] " + tr("Disconnected") + "." + "</FONT>");
    serverSocket->close();
    serverSocket->abort();
}

// ----------------------------------------------------------------------

// this function is called anytime we change connection settings AFTER constructing the client
// i.e. starting the program. Client must be disconnected and connected again for the changes
// to take place
void QueleaClient::changeSettings(const QString& cn, const QString& sa)
{
    clientName = cn;
    serverAddress = sa;
}
