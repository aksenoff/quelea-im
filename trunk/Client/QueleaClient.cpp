

#include <QtNetwork>
#include <QtGui>
#include "QueleaClient.h"
#include "../codes.h"

// ----------------------------------------------------------------------
QueleaClient::QueleaClient(QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    textInfo  = new QTextEdit;
    messInput = new QLineEdit;
    clname = new QComboBox;
    contlist = new QListWidget;
    contlist->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    spacer1 = new QSpacerItem(100,0);
    spacer2 = new QSpacerItem(100,0);
    tcpSocket = new QTcpSocket(this);
    clname->setEditable(true);
    textInfo->setReadOnly(true);
    pcmd = new QPushButton(QString::fromLocal8Bit(" Отправить лично "));
    connect(pcmd, SIGNAL(clicked()), SLOT(sendmess()));
    pcmd->setEnabled(false);
    pcmd->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connbutton = new QPushButton(QString::fromLocal8Bit(" Подключиться "));
    connbutton->setEnabled(false);
    connbutton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    sendtochat = new QPushButton(QString::fromLocal8Bit(" Отправить в чат "));
    connect(sendtochat, SIGNAL(clicked()), SLOT(sendchat()));
    sendtochat->setEnabled(false);
    sendtochat->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    info = new QPushButton("&Info");
    info->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    
    connect(clname, SIGNAL(editTextChanged(QString)),
            this, SLOT(enableConnButton()));

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
    nameLayout->addWidget(new QLabel(QString::fromLocal8Bit("Вас зовут:")),0,Qt::AlignRight);
    nameLayout->addWidget(clname);
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


}
void QueleaClient::enableConnected()
{
    connect(connbutton, SIGNAL(clicked()), this, SLOT(disconn()));
}


void QueleaClient::enableDisconnected()
{
    connect(connbutton, SIGNAL(clicked()), this, SLOT(opendial()));
}
// ---------------------------------------------------------------------
void QueleaClient::conn(QString ipadr)
{
    emit startedConnect();
    clname->setEnabled(false);
    tcpSocket->connectToHost(ipadr, 49212);

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
                Message* auth_req = new Message(AUTH_REQUEST, clname->currentText());
                SendToServer(auth_req);
                delete auth_req;
                textInfo->append("["+time.toString()+"]" + " " + str);
                break;
            }

        case AUTH_RESPONSE:
           {
               str = QString::fromLocal8Bit("Вход выполнен.");;
               Message* contacts_req = new Message(CONTACTS_REQUEST);
               SendToServer(contacts_req);
               delete contacts_req;
               textInfo->append("["+time.toString()+"]" + " " + str);
               break;
           }
        case CONTACTS_RESPONSE:
           {
               textInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("Список контактов получен."));
               QStringList clist = mess->text.split(";");
               contlist->clear();
               clist.removeOne(clname->currentText());
               clist.removeOne("");
               if (clist.count()!=0)
               contlist->addItem(QString::fromLocal8Bit(">Все собеседники"));
               contlist->addItems(clist);
               contlist->setCurrentRow(0);
               enableSendButton();
               break;
           }
        case MESSAGE_TO_CLIENT:
           {
               QStringList clist = mess->text.split(";");
               str=clist[0]+clist[2]+": "+clist[1];
               textInfo->append("["+time.toString()+"]"+ " " + str);
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

void QueleaClient::sendmess()
{
    QString str=contlist->currentItem()->text()+";"+messInput->text();
    Message* newmess = new Message(MESSAGE_TO_SERVER,str);
    SendToServer(newmess);
    if (contlist->currentItem()->text()!=QString::fromLocal8Bit(">Все собеседники"))
        textInfo->append("["+QTime::currentTime().toString()+"]"+" "+clname->currentText()+" -> "+contlist->currentItem()->text()+": "+messInput->text());
    messInput->setText("");
    enableSendButton();
}

void QueleaClient::sendchat()
{
    QString str=contlist->currentItem()->text()+";"+messInput->text();
    Message* newmess = new Message( MESSAGE_TO_CHAT,str);
    SendToServer(newmess);
    messInput->setText("");
    enableSendButton();

}
// ----------------------------------------------------------------------
void QueleaClient::opendial()
{

    QStringList items;
    QTime time = QTime::currentTime();
        bool ok=false;
        QString ipadress = QInputDialog::getItem(this, (QString::fromLocal8Bit("Выберите сервер")),
                                                 (QString::fromLocal8Bit("Выберите сервер:")), items, 0, true, &ok);
        if (ok && !ipadress.isEmpty())
        {
            conn(ipadress);
            disconnect(connbutton, SIGNAL(clicked()), this, SLOT(opendial()));
            textInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("Соединение с сервером..."));

        }
}
// ----------------------------------------------------------------------
void QueleaClient::enableConnButton()

{
    connbutton->setEnabled(!clname->currentText().isEmpty());
}


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
    clname->setEnabled(true);
    textInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("Отключен от сервера"));
}

