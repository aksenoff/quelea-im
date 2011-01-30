

#include <QtNetwork>
#include <QtGui>
#include "QueleaClient.h"
#include "settingsDialog.h"
#include "../codes.h"
#include "../message.h"

// ----------------------------------------------------------------------
QueleaClient::QueleaClient(QueleaClientUI* userInterface, QString cn, QString sa)
    : serverAddress(sa), clientName(cn), ui(userInterface)
{
    clientStatus="offline";
    tcpSocket = new QTcpSocket(this);


    connect(tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slotError(QAbstractSocket::SocketError)));
}

// ---------------------------------------------------------------------
void QueleaClient::conn()
{
    tcpSocket->connectToHost(serverAddress, 49212);
}

// ----------------------------------------------------------------------
void QueleaClient::slotReadyRead()
{
    Message mess(tcpSocket);
    QTime time = QTime::currentTime();
    QString str;
    switch(mess)
    {
    case CONNECTED:
        {
            QString logString(tr("Соединено."));
            ui->logAction(logString);
            Message auth_req(AUTH_REQUEST, clientName);
            auth_req.send(tcpSocket);
            break;
        }

    case AUTH_RESPONSE:
        {
            if(mess.getText()=="auth_ok")
            {
                QString logString(tr("Вход выполнен."));
                ui->logAction(logString);
                Message contacts_req(CONTACTS_REQUEST);
                contacts_req.send(tcpSocket);
                emit authOkSignal();
                changeStatus("online");
            }
            if(mess.getText()=="auth_error")
            {
                QString logString(tr("<FONT COLOR=RED>Ошибка: Такое имя уже используется<FONT>"));
                ui->logAction(logString);
                emit authErrorSignal();
            }
            break;
        }
    default:
        ui->parseMessage(mess);
    }
}

// ----------------------------------------------------------------------
void QueleaClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "<FONT COLOR=BLUE>["+QTime::currentTime().toString()+"]<FONT>"+" <FONT COLOR=RED>"+tr("Ошибка: ") +
                    (err == QAbstractSocket::HostNotFoundError ?
                     tr("Сервер не найден.") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     tr("Удалённый хост закрыл соединение.") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     tr("В соединении было отказано.") :
                     QString(tcpSocket->errorString())
                    )+"<FONT>";

    emit socketError();
    ui->logAction(strError);
    changeStatus("offline");

}

// ------------------------------------------------------------------

void QueleaClient::sendmess(QString& receiver, QString& messageText) //outcoming private message
{
    QString str=receiver+";"+messageText;
    Message newmess(MESSAGE_TO_SERVER,str);
    newmess.send(tcpSocket);

}

void QueleaClient::sendchat(QString& receiver, QString& messageText)
{
    QString str=receiver+";"+messageText;
    Message newmess(MESSAGE_TO_CHAT,str);
    newmess.send(tcpSocket);;
}
// ----------------------------------------------------------------------



void QueleaClient::disconn()
{
    tcpSocket->close();
    tcpSocket->abort();
    changeStatus("offline");
}


void QueleaClient::changeStatus(QString status)
{
    clientStatus=status;
    emit statusChanged(status);
}


QString QueleaClient::getStatus()
{
    return clientStatus;
}

void QueleaClient::changeSettings(QString cn, QString sa)
{
    clientName = cn;
    serverAddress = sa;
}
