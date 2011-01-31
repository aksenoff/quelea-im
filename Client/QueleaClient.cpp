#include <QtNetwork>
#include <QtGui>
#include "QueleaClient.h"
#include "settingsDialog.h"
#include "../codes.h"
#include "../message.h"

QueleaClient::QueleaClient(QueleaClientUI* UI, const QString& cn, const QString& sa)
    : serverAddress(sa), clientName(cn), ui(UI)
{
    clientStatus = "offline";
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
            ui->log(tr("Соединено."));
            Message auth_req(AUTH_REQUEST, clientName);
            auth_req.send(serverSocket);
            break;
        }
    case AUTH_RESPONSE:
        {
            if(incomingMessage.getText()=="auth_ok")
            {
                ui->log(tr("Вход выполнен."));
                Message contacts_req(CONTACTS_REQUEST);
                contacts_req.send(serverSocket);
                emit authOkSignal();
                changeStatus("online");
            }
            if(incomingMessage.getText()=="auth_error")
            {
                ui->log(tr("<FONT COLOR=RED>Ошибка: Такое имя уже используется<FONT>"));
                emit authErrorSignal();
            }
            break;
        }
    default:
        ui->parseMessage(incomingMessage);
    }
}

// ----------------------------------------------------------------------

void QueleaClient::slotError(QAbstractSocket::SocketError err)
{
    QString error =
        "<FONT COLOR=BLUE>["+QTime::currentTime().toString()+"]<FONT>"+" <FONT COLOR=RED>"+tr("Ошибка: ") +
                    (err == QAbstractSocket::HostNotFoundError ?
                     tr("Сервер не найден.") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     tr("Удалённый хост закрыл соединение.") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     tr("В соединении было отказано.") :
                     QString(serverSocket->errorString())
                    )+"<FONT>";

    emit socketError();
    ui->log(error);
    changeStatus("offline");
}

// ------------------------------------------------------------------

void QueleaClient::sendPrivateMessage(const QString& receiver, const QString& messageText) const //outcoming private message
{
    QString str = receiver + ";" + messageText;
    Message newmess(MESSAGE_TO_SERVER, str);
    newmess.send(serverSocket);
}

void QueleaClient::sendChatMessage(const QString& receiver, const QString& messageText) const
{
    QString str = receiver + ";" + messageText;
    Message newmess(MESSAGE_TO_CHAT, str);
    newmess.send(serverSocket);
}

// ----------------------------------------------------------------------

void QueleaClient::disconnectFromServer()
{
    serverSocket->close();
    serverSocket->abort();
    changeStatus("offline");
}

// ----------------------------------------------------------------------

void QueleaClient::changeStatus(const QString& status)
{
    clientStatus = status;
    emit statusChanged(status); //?
}

// ----------------------------------------------------------------------

QString QueleaClient::getStatus() const
{
    return clientStatus;
}

// ----------------------------------------------------------------------

void QueleaClient::changeSettings(const QString& cn, const QString& sa)
{
    clientName = cn;
    serverAddress = sa;
}
