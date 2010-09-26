

#include <QtNetwork>
#include <QtGui>
#include "QueleaClient.h"
#include "settingsDialog.h"
#include "../codes.h"

// ----------------------------------------------------------------------
QueleaClient::QueleaClient(QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    textInfo  = new QTextEdit;
    messInput = new QLineEdit;
    contlist = new QListWidget;
    contlist->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    stateLabel = new QLabel("<FONT COLOR=RED>Offline</FONT>");
    spacer1 = new QSpacerItem(100,0);
    spacer2 = new QSpacerItem(100,0);
    tcpSocket = new QTcpSocket(this);
    textInfo->setReadOnly(true);
    pcmd = new QPushButton(QString::fromLocal8Bit(" Отправить лично "));
    connect(pcmd, SIGNAL(clicked()), SLOT(sendmess()));
    pcmd->setEnabled(false);
    pcmd->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connbutton = new QPushButton(QString::fromLocal8Bit(" Подключиться "));
    connbutton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    sendtochat = new QPushButton(QString::fromLocal8Bit(" Отправить в чат "));
    connect(sendtochat, SIGNAL(clicked()), SLOT(sendchat()));
    sendtochat->setEnabled(false);
    sendtochat->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    info = new QPushButton("&Info");
    info->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    settingsButton = new QPushButton("&Settings");
    settingsButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(settingsButton, SIGNAL(clicked()), SLOT(openSettingDialog()));



    connect(messInput, SIGNAL(textEdited(QString)),
            this, SLOT(enableSendButton()));

    connect(contlist, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem *)),
            this, SLOT(enableSendButton()));
    connect(tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

    QState (*connectedState) = new QState, (*disconnectedState) = new QState;
    connectedState->assignProperty(connbutton, "text", QString::fromLocal8Bit(" Отключиться "));
    disconnectedState->assignProperty(connbutton, "text", QString::fromLocal8Bit(" Подключиться "));
    connectedState->addTransition(connbutton, SIGNAL(clicked()), disconnectedState);
    connectedState->addTransition(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),disconnectedState);
    disconnectedState->addTransition(this, SIGNAL(startedConnect()), connectedState);
    connect(connectedState, SIGNAL(entered()), this, SLOT(enableConnected()));
    connect(disconnectedState, SIGNAL(entered()), this, SLOT(enableDisconnected()));
    connectionStatus.addState(connectedState);
    connectionStatus.addState(disconnectedState);
    connectionStatus.setInitialState(disconnectedState);

    connectionStatus.start();

    //Layout setup
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QHBoxLayout* send2chatLayout = new QHBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    nameLayout->addWidget(settingsButton);
    nameLayout->addWidget(new QLabel(QString::fromLocal8Bit("Статус:")),0,Qt::AlignRight);
    nameLayout->addWidget(stateLabel);
    leftLayout->addLayout(nameLayout);
    leftLayout->addWidget(textInfo);
    send2chatLayout->addSpacerItem(spacer1);
    send2chatLayout->addWidget(sendtochat);
    send2chatLayout->addSpacerItem(spacer2);
    leftLayout->addLayout(send2chatLayout);
    leftLayout->addWidget(messInput);
    rightLayout->addWidget(connbutton);
    rightLayout->addWidget(new QLabel(QString::fromLocal8Bit("Ваши собеседники:")));
    rightLayout->addWidget(contlist);
    rightLayout->addWidget(pcmd);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
    setWindowTitle(tr("Quelea"));
    setWindowIcon(QIcon::QIcon ("resource.rc"));



    QFile file("set.dat");

    if (file.open(QIODevice::ReadOnly)){
        QTextStream stream (&file);
        QString readname;
        QString readserver;
        bool autoconnect;
        clientName=stream.readLine();
        serverAdr=stream.readLine();
        autoconnect= stream.readLine().toInt();
        if (autoconnect)
            conn();
        file.close();
    }
    else {


        openSettingDialog();

    }

}
void QueleaClient::enableConnected()
{
    connect(connbutton, SIGNAL(clicked()), this, SLOT(disconn()));
}


void QueleaClient::enableDisconnected()
{
    connect(connbutton, SIGNAL(clicked()), this, SLOT(conn()));
}
// ---------------------------------------------------------------------
void QueleaClient::conn()
{

    emit startedConnect();
    disconnect(connbutton, SIGNAL(clicked()), this, SLOT(conn()));
    stateLabel->setText("Connection to server...");
    tcpSocket->connectToHost(serverAdr, 49212);


}

// ----------------------------------------------------------------------
void QueleaClient::slotReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!nextBlockSize) {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }

        if (tcpSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
        QTime time = QTime::currentTime();
        QString str;
        Message *mess=0; //!
        in >> mess;
        switch(int(*mess))
        {
        case CONNECTED:
            {
                str = QString::fromLocal8Bit("Соединение установлено.");
                Message* auth_req = new Message(AUTH_REQUEST, clientName);
                SendToServer(auth_req);
                delete auth_req;
                stateLabel->setText(str);
                break;
            }

        case AUTH_RESPONSE:
           {
               str = QString::fromLocal8Bit("Вход выполнен.");
               Message* contacts_req = new Message(CONTACTS_REQUEST);
               SendToServer(contacts_req);
               delete contacts_req;
               stateLabel->setText("<FONT COLOR=GREEN>Online</FONT>");
               break;
           }
        case CONTACTS_RESPONSE:
           {
               QStringList clist = mess->text.split(";");
               contlist->clear();
               clist.removeOne(clientName);
               clist.removeOne("");
               if (clist.count()!=0)
               contlist->addItem(QString::fromLocal8Bit(">Все собеседники"));
               contlist->addItems(clist);
               contlist->setCurrentRow(0);
               enableSendButton();
               break;
           }

        case MESSAGE_TO_CHAT:
            {
               QStringList mlist = mess->text.split(";"); // mlist[0]=от кого, mlist[1]=кому, mlist[2]=текст сообщения
               if (mlist[1]=="all")
                   textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=GREEN>"+mlist[0]+"</FONT>"+": "+mlist[2]);
               else
                   textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=GREEN>"+mlist[0]+"</FONT>"+": "+"<FONT COLOR=ORANGERED>["+mlist[1]+"]</FONT> "+mlist[2]);
                break;
            }
        case MESSAGE_TO_CLIENT: // incoming private message
           {
               QStringList mlist = mess->text.split(";"); //mlist[0]=from who, mlist[1]=text
               textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=DARKVIOLET>"+mlist[0]+"</FONT>: "+mlist[1]);
               break;
           }
        }
        delete mess;
        nextBlockSize = 0;
    }
}

// ----------------------------------------------------------------------
void QueleaClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "["+QTime::currentTime().toString()+"]"+" "+QString::fromLocal8Bit("Ошибка: ") + (err == QAbstractSocket::HostNotFoundError ?
                     QString::fromLocal8Bit("Сервер не найден.") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     QString::fromLocal8Bit("Удалённый хост закрыл соединение.") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     QString::fromLocal8Bit("В соединении было отказано.") :
                     QString(tcpSocket->errorString())
                    );
    textInfo->append(strError);
    stateLabel->setText("<FONT COLOR=RED>Offline</FONT>");
}

// ----------------------------------------------------------------------
void QueleaClient::SendToServer(Message* message)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0)<< *message;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    tcpSocket->write(arrBlock);
}

// ------------------------------------------------------------------

void QueleaClient::sendmess() //outcoming private message
{
    QString str=contlist->currentItem()->text()+";"+messInput->text();
    Message* newmess = new Message(MESSAGE_TO_SERVER,str);
    SendToServer(newmess);
    textInfo->append("<FONT COLOR=BLUE>["+QTime::currentTime().toString()+"]</FONT>"+" "+"<FONT COLOR=GREEN>"+clientName+" "+"</FONT><FONT COLOR=DARKVIOLET>["+contlist->currentItem()->text()+"]</FONT>: "+messInput->text());
    messInput->setText("");
    enableSendButton();
}

void QueleaClient::sendchat()
{
    QString receiver;
    if (contlist->currentRow()==0)
        receiver = "all";
    else
        receiver = contlist->currentItem()->text();

    QString str=receiver+";"+messInput->text();
    Message* newmess = new Message( MESSAGE_TO_CHAT,str);
    SendToServer(newmess);
    messInput->setText("");
    enableSendButton();

}
// ----------------------------------------------------------------------

void QueleaClient::enableSendButton()
{

    pcmd->setEnabled(!messInput->text().isEmpty() && contlist->count()!=0 && contlist->currentItem()->text()!=QString::fromLocal8Bit(">Все собеседники"));
    sendtochat->setEnabled(!messInput->text().isEmpty() && contlist->count()!=0);
}

void QueleaClient::disconn()
{
    QTime time = QTime::currentTime();
    disconnect(connbutton, SIGNAL(clicked()), this, SLOT(disconn()));
    tcpSocket->close();
    tcpSocket->abort();
    contlist->clear();
    stateLabel->setText("<FONT COLOR=RED>Offline</FONT>");
}

void QueleaClient::openSettingDialog()
{
   SettingsDialog* setdial = new SettingsDialog;
    if (setdial->exec() == QDialog::Accepted) {

        serverAdr=setdial->serverAdr();
        clientName=setdial->clientName();

        QFile file("set.dat");
        if (file.open(QIODevice::WriteOnly)){
            QTextStream stream (&file);
            QString str = setdial->clientName()+"\n"+setdial->serverAdr();//+"\n"+setdial->autoconnect();
            stream<<setdial->clientName()<<'\n'<< flush<<setdial->serverAdr()<<'\n'<< flush<<setdial->autoconnect();
            file.close();

        }

    }
    delete setdial;
}
