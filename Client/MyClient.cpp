

#include <QtNetwork>
#include <QtGui>
#include "MyClient.h"
#include "../codes.h"

// ----------------------------------------------------------------------
MyClient::MyClient(QWidget* pwgt) : QWidget(pwgt), NextBlockSize(0)
{
    TextInfo  = new QTextEdit;
    messInput = new QLineEdit;
    clname = new QComboBox;
    contlist = new QListWidget;
    contlist->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    spacer1 = new QSpacerItem(100,0);
    spacer2 = new QSpacerItem(100,0);
    TcpSocket = new QTcpSocket(this);
    clname->setEditable(true);
    TextInfo->setReadOnly(true);    
    pcmd = new QPushButton(QString::fromLocal8Bit(" ��������� ����� "));
    connect(pcmd, SIGNAL(clicked()), SLOT(sendmess()));
    pcmd->setEnabled(false);
    pcmd->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connbutton = new QPushButton(QString::fromLocal8Bit(" ������������ "));
    connbutton->setEnabled(false);
    connbutton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    sendtochat = new QPushButton(QString::fromLocal8Bit(" ��������� � ��� "));
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

    QState (*connectedState) = new QState(), (*disconnectedState) = new QState;
    connectedState->assignProperty(connbutton, "text", QString::fromLocal8Bit(" ����������� "));
    disconnectedState->assignProperty(connbutton, "text", QString::fromLocal8Bit(" ������������ "));
    connectedState->addTransition(connbutton, SIGNAL(clicked()), disconnectedState);
    disconnectedState->addTransition(connbutton, SIGNAL(clicked()), connectedState);
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
    nameLayout->addWidget(new QLabel(QString::fromLocal8Bit("��� �����:")),0,Qt::AlignRight);
    nameLayout->addWidget(clname);
    leftLayout->addLayout(nameLayout);
    leftLayout->addWidget(TextInfo);
    send2chatLayout->addSpacerItem(spacer1);
    send2chatLayout->addWidget(sendtochat);
    send2chatLayout->addSpacerItem(spacer2);
    leftLayout->addLayout(send2chatLayout);
    leftLayout->addWidget(messInput);
    rightLayout->addWidget(connbutton);
    rightLayout->addWidget(new QLabel(QString::fromLocal8Bit("���� �����������:")));
    rightLayout->addWidget(contlist);
    rightLayout->addWidget(pcmd);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
    setWindowTitle(tr("Quelea"));
    setWindowIcon(QIcon::QIcon ("myapp.rc"));


}
void MyClient::enableConnected()
{
    connect(connbutton, SIGNAL(clicked()), this, SLOT(disconn()));
}


void MyClient::enableDisconnected()
{
    connect(connbutton, SIGNAL(clicked()), this, SLOT(opendial()));
}
// ---------------------------------------------------------------------
void MyClient::conn(QString ipadr)
{
    TcpSocket->connectToHost(ipadr, 49212);
    connect(TcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );
}

// ----------------------------------------------------------------------
void MyClient::slotReadyRead()
{
    QDataStream in(TcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!NextBlockSize) {
            if (TcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> NextBlockSize;
        }

        if (TcpSocket->bytesAvailable() < NextBlockSize) {
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
                str = QString::fromLocal8Bit("���������� �����������.");
                Message* auth_req = new Message(AUTH_REQUEST, clname->currentText());
                SendToServer(auth_req);
                delete auth_req;
                clname->setEnabled(false);
                TextInfo->append("["+time.toString()+"]" + " " + str);
                break;
            }

        case AUTH_RESPONSE:
           {
               str = QString::fromLocal8Bit("���� ��������.");;
               Message* contacts_req = new Message(CONTACTS_REQUEST);
               SendToServer(contacts_req);
               delete contacts_req;
               TextInfo->append("["+time.toString()+"]" + " " + str);
               break;
           }
        case CONTACTS_RESPONSE:
           {
               if (contlist->count()==0)
               TextInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("������ ��������� �������."));
               QStringList clist = mess->text.split(";");
               contlist->clear();
               clist.removeOne(clname->currentText());
               clist.removeOne("");
               if (clist.count()!=0)
               contlist->addItem(QString::fromLocal8Bit(">��� �����������"));
               contlist->addItems(clist);
               contlist->setCurrentRow(0);
               enableSendButton();
               break;
           }
        case MESSAGE_TO_CLIENT:
           {
               QStringList clist = mess->text.split(";");
               str=clist[0]+clist[2]+": "+clist[1];
               TextInfo->append("["+time.toString()+"]"+ " " + str);
               if (clist[0]!=clname->currentText())
               QSound::play("incom.wav");
               break;
           }            
        }
        delete mess;
        NextBlockSize = 0;
    }
}

// ----------------------------------------------------------------------
void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError = 
        QString::fromLocal8Bit("������: ") + (err == QAbstractSocket::HostNotFoundError ?
                     QString::fromLocal8Bit("������ �� ������.") :
                     err == QAbstractSocket::RemoteHostClosedError ? 
                     QString::fromLocal8Bit("�������� ���� ������ ����������.") :
                     err == QAbstractSocket::ConnectionRefusedError ? 
                     QString::fromLocal8Bit("� ���������� ���� ��������.") :
                     QString(TcpSocket->errorString())
                    );
    TextInfo->append(strError);
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
    TcpSocket->write(arrBlock);
}

// ------------------------------------------------------------------

void MyClient::sendmess()
{
    QString str=contlist->currentItem()->text()+";"+messInput->text();
    Message* newmess = new Message(MESSAGE_TO_SERVER,str);
    SendToServer(newmess);
    if (contlist->currentItem()->text()!=QString::fromLocal8Bit(">��� �����������"))
        TextInfo->append("["+QTime::currentTime().toString()+"]"+" "+clname->currentText()+": "+messInput->text());
    messInput->setText("");
}

void MyClient::sendchat()
{
    QString str=contlist->currentItem()->text()+";"+messInput->text();
    Message* newmess = new Message( MESSAGE_TO_CHAT,str);
    SendToServer(newmess);
    messInput->setText("");

}
// ----------------------------------------------------------------------
void MyClient::opendial()
{
    disconnect(connbutton, SIGNAL(clicked()), this, SLOT(opendial()));
    QStringList items;
    QTime time = QTime::currentTime();
        bool ok=false;
        QString ipadress = QInputDialog::getItem(this, (QString::fromLocal8Bit("�������� ������")),
                                                 (QString::fromLocal8Bit("�������� ������:")), items, 0, true, &ok);
        if (ok && !ipadress.isEmpty())
        {
            conn(ipadress);
            TextInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("���������� � ��������..."));
        }
}
// ----------------------------------------------------------------------
void MyClient::enableConnButton()

{
    connbutton->setEnabled(!clname->currentText().isEmpty());
}


void MyClient::enableSendButton()
{

    pcmd->setEnabled(!messInput->text().isEmpty() && contlist->count()!=0 && contlist->currentItem()->text()!=QString::fromLocal8Bit(">��� �����������"));
    sendtochat->setEnabled(!messInput->text().isEmpty() && contlist->count()!=0);
}

void MyClient::disconn()
{
    QTime time = QTime::currentTime();
    disconnect(connbutton, SIGNAL(clicked()), this, SLOT(disconn()));
    TcpSocket->close();
    TextInfo->append("["+time.toString()+"]" + " "+QString::fromLocal8Bit("�������� �� �������"));
}

