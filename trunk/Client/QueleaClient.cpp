

#include <QtNetwork>
#include <QtGui>
#include "QueleaClient.h"
#include "settingsDialog.h"
#include "../codes.h"
#include "../message.h"

// ----------------------------------------------------------------------
QueleaClient::QueleaClient(QueleaClientUI* userInterface)
    : ui(userInterface), nextBlockSize(0)
{
    clientStatus="offline";
    tcpSocket = new QTcpSocket(this);


    connect(tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slotError(QAbstractSocket::SocketError)));


    //Reading settings:

    QFile file("set.dat");

    if (file.open(QIODevice::ReadOnly)){
        QTextStream stream (&file);
        bool autoconnect;
        clientName = stream.readLine();
        serverAdr = stream.readLine();
        autoconnect = stream.readLine().toInt();
        ui->setUISettings(stream.readLine().toInt());
        if (autoconnect){
            conn();
         //   connectionStatus.setInitialState(connectedState);
        }
        file.close();
    }
    else
        ui->openSettingDialog();


}

// ---------------------------------------------------------------------
void QueleaClient::conn()
{
    emit startedConnect();
    ui->changeStatusLabel(tr("Соединение..."));
    tcpSocket->connectToHost(serverAdr, 49212);


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
            ui->logAction(tr("Соединено."));
            Message auth_req(AUTH_REQUEST, clientName);
            auth_req.send(tcpSocket);
            break;
        }

    case AUTH_RESPONSE:
        {
            if(mess.getText()=="auth_ok")
            {
                ui->logAction(tr("Вход выполнен."));
                Message contacts_req(CONTACTS_REQUEST);
                contacts_req.send(tcpSocket);
                changeStatus("online");
            }
            if(mess.getText()=="auth_error")
            {
                ui->logAction(tr("<FONT COLOR=RED>Ошибка: Такое имя уже используется<FONT>"));
                emit disconnectSignal();
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

void QueleaClient::sendmess(QString receiver, QString messageText) //outcoming private message
{
    QString str=receiver+";"+messageText;
    Message newmess(MESSAGE_TO_SERVER,str);
    newmess.send(tcpSocket);

}

void QueleaClient::sendchat(QString receiver, QString messageText)
{


    QString str=receiver+";"+messageText;
    Message newmess(MESSAGE_TO_CHAT,str);
    newmess.send(tcpSocket);;


}
// ----------------------------------------------------------------------



void QueleaClient::disconn()
{
    disconnectSignal();
    tcpSocket->close();
    tcpSocket->abort();
    ui->setUItoDisconnected();
    changeStatus("offline");
}


void QueleaClient::changeStatus(QString status)
{
    clientStatus=status;
    emit statusChanged(status);
    if(status=="online")
        ui->changeStatusLabel(tr("<FONT COLOR=GREEN>В сети</FONT>"));
    else
        ui->changeStatusLabel(tr("<FONT COLOR=RED>Отключен</FONT>"));
}

void QueleaClient::setSettings(QString name, QString server)
{
    clientName = name;
    serverAdr = server;
}

QString QueleaClient::getStatus()
{
    return clientStatus;
}

void QueleaClientUI::setUISettings(bool sound)
{
    enableSound = sound;
}

void QueleaClientUI::changeStatusLabel(QString status)
{
    stateLabel->setText(status);
}
