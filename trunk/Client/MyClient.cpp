

#include <QtNetwork>
#include <QtGui>
#include "MyClient.h"
#include "../codes.h"

// ----------------------------------------------------------------------
MyClient::MyClient(
                  QWidget*       pwgt /*=0*/
                  ) : QWidget(pwgt)
                    , m_nNextBlockSize(0)


{
    m_ptxtInfo  = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    clname = new QComboBox;
    contlist = new QListWidget;
    contlist->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    recipname = new QLineEdit;
    spacer1 = new QSpacerItem(100,0);
    spacer2 = new QSpacerItem(100,0);



    m_pTcpSocket = new QTcpSocket(this);

    clname->setEditable(true);



    m_ptxtInfo->setReadOnly(true);

    pcmd = new QPushButton((QString::fromLocal8Bit(" Отправить лично ")));
    connect(pcmd, SIGNAL(clicked()), SLOT(sendmess()));
    pcmd->setEnabled(false);
    pcmd->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connbutton = new QPushButton((QString::fromLocal8Bit(" Подключиться ")));
    connect(connbutton, SIGNAL(clicked()), SLOT(opendial()));
    connbutton->setEnabled(false);
    connbutton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    sendtochat = new QPushButton((QString::fromLocal8Bit(" Отправить в чат ")));
    connect(sendtochat, SIGNAL(clicked()), SLOT(sendchat()));
    sendtochat->setEnabled(false);
    sendtochat->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    info = new QPushButton("&Info");
    info->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(clname, SIGNAL(editTextChanged(QString)),
            this, SLOT(enableConnButton()));

    connect(m_ptxtInput, SIGNAL(textEdited(QString)),
            this, SLOT(enableSendButton()));

    connect(contlist, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem *)),
            this, SLOT(enableSendButton()));


    //Layout setup
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QHBoxLayout* send2chatLayout = new QHBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    nameLayout->addWidget(new QLabel(QString::fromLocal8Bit("Вас зовут:")),0,Qt::AlignRight);
    nameLayout->addWidget(clname);
    leftLayout->addLayout(nameLayout);
    leftLayout->addWidget(m_ptxtInfo);
    send2chatLayout->addSpacerItem(spacer1);
    send2chatLayout->addWidget(sendtochat);
    send2chatLayout->addSpacerItem(spacer2);
    leftLayout->addLayout(send2chatLayout);
    leftLayout->addWidget(m_ptxtInput);
    rightLayout->addWidget(connbutton);
    rightLayout->addWidget(new QLabel(QString::fromLocal8Bit("Ваши собеседники:")));
    rightLayout->addWidget(contlist);
    rightLayout->addWidget(pcmd);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);


}




void MyClient::conn(QString ipadr)
{
    m_pTcpSocket->connectToHost(ipadr, 49212);
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );






}
// ----------------------------------------------------------------------
void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
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
                clname->setEnabled(false);
                m_ptxtInfo->append("["+time.toString()+"]" + " " + str);
                break;
            }

        case AUTH_RESPONSE:
           {
               str = QString::fromLocal8Bit("Вход выполнен.");;
               Message* contacts_req = new Message(CONTACTS_REQUEST);
               SendToServer(contacts_req);
			   delete contacts_req;
               m_ptxtInfo->append("["+time.toString()+"]" + " " + str);
               break;
           }
        case CONTACTS_RESPONSE:
           {
               if (contlist->count()==0)
               m_ptxtInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("Список контактов получен."));
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
               m_ptxtInfo->append("["+time.toString()+"]"+ " " + str);
               if (clist[0]!=clname->currentText())
               QSound::play("incom.wav");
               break;
           }

            
        }


        delete mess;

        m_nNextBlockSize = 0;
    }
}

// ----------------------------------------------------------------------
void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError = 
        QString::fromLocal8Bit("Ошибка: ") + (err == QAbstractSocket::HostNotFoundError ?
                     QString::fromLocal8Bit("Сервер не найден.") :
                     err == QAbstractSocket::RemoteHostClosedError ? 
                     QString::fromLocal8Bit("Удалённый хост закрыл соединение.") :
                     err == QAbstractSocket::ConnectionRefusedError ? 
                     QString::fromLocal8Bit("В соединении было отказано.") :
                     QString(m_pTcpSocket->errorString())
                    );
    m_ptxtInfo->append(strError);
}

// ----------------------------------------------------------------------
void MyClient::SendToServer(Message* message)
{


    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);


    out << quint16(0)<< *message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);




}

// ------------------------------------------------------------------

void MyClient::sendmess()
{
    QString str=contlist->currentItem()->text()+";"+m_ptxtInput->text();
    Message* newmess = new Message(MESSAGE_TO_SERVER,str);
    SendToServer(newmess);
    if (contlist->currentItem()->text()!=QString::fromLocal8Bit(">Все собеседники"))
        m_ptxtInfo->append("["+QTime::currentTime().toString()+"]"+" "+clname->currentText()+": "+m_ptxtInput->text());
    m_ptxtInput->setText("");
}

void MyClient::sendchat()
{
    QString str=contlist->currentItem()->text()+";"+m_ptxtInput->text();
    Message* newmess = new Message( MESSAGE_TO_CHAT,str);
    SendToServer(newmess);
    m_ptxtInput->setText("");


}

void MyClient::opendial()
{
    QStringList items;


        bool ok;
        QString ipadress = QInputDialog::getItem(this, (QString::fromLocal8Bit("Выберите сервер")),
                                                 (QString::fromLocal8Bit("Выберите сервер:")), items, 0, true, &ok);
        if (ok && !ipadress.isEmpty())
        { conn(ipadress);
            m_ptxtInfo->append(QString::fromLocal8Bit("Соединение с сервером..."));}

}

void MyClient::enableConnButton()

{

    connbutton->setEnabled(!clname->currentText().isEmpty());
}


void MyClient::enableSendButton()
{

pcmd->setEnabled(!m_ptxtInput->text().isEmpty() && contlist->count()!=0 && contlist->currentItem()->text()!=QString::fromLocal8Bit(">Все собеседники"));
sendtochat->setEnabled(!m_ptxtInput->text().isEmpty() && contlist->count()!=0);
}
