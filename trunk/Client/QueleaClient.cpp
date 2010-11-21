

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
    connect(contlist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(addTab(QListWidgetItem*)));
    stateLabel = new QLabel("<FONT COLOR=RED>Offline</FONT>");
    yourCompanionsLabel = new QLabel(QString::fromLocal8Bit("���� �����������:"));
    spacer1 = new QSpacerItem(100,0);
    spacer2 = new QSpacerItem(100,0);
    spacer3 = new QWidget();
    spacer3->setHidden(true);
    spacer4 = new QWidget();
    tcpSocket = new QTcpSocket(this);
    textInfo->setReadOnly(true);
    sendButton = new QPushButton(QString::fromLocal8Bit(" ��������� "));
  //  connect(sendButton, SIGNAL(clicked()), SLOT(sendchat()));
    sendButton->setEnabled(false);
    sendButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connbutton = new QPushButton(QString::fromLocal8Bit(" ������������ "));
    connbutton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    info = new QPushButton("&Info");
    info->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    settingsButton = new QPushButton("&Settings");
    settingsButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(settingsButton, SIGNAL(clicked()), SLOT(openSettingDialog()));
    tabWidget = new TabWt;
    tabWidget->setTabsClosable(true);
    connect(tabWidget,SIGNAL(currentChanged(int)),SLOT(normalizeTabColor(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    tabWidget->addTab(textInfo,"All");
    connect(tabWidget, SIGNAL(currentChanged(int)),this,SLOT(sendButtonFunc(int)));
    tabWidget->gettabbar()->setTabButton(0,QTabBar::RightSide,0);

    sendShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Return ), this);
    connect(sendShortcut, SIGNAL(activated()), sendButton, SLOT(click()));


    connect(messInput, SIGNAL(textEdited(QString)),
            this, SLOT(enableSendButton()));

    connect(contlist, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem *)),
            this, SLOT(enableSendButton()));
    connect(tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

    QState (*connectedState) = new QState, (*disconnectedState) = new QState;
    connectedState->assignProperty(connbutton, "text", QString::fromLocal8Bit(" ����������� "));
    disconnectedState->assignProperty(connbutton, "text", QString::fromLocal8Bit(" ������������ "));
    connectedState->addTransition(connbutton, SIGNAL(clicked()), disconnectedState);
    connectedState->addTransition(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),disconnectedState);
    connectedState->addTransition(this, SIGNAL(toDisconnStateBydisconn()), disconnectedState);
    disconnectedState->addTransition(this, SIGNAL(startedConnect()), connectedState);
    connect(connectedState, SIGNAL(entered()), this, SLOT(enableConnected()));
    connect(disconnectedState, SIGNAL(entered()), this, SLOT(enableDisconnected()));
    connectionStatus.addState(connectedState);
    connectionStatus.addState(disconnectedState);
    connectionStatus.setInitialState(disconnectedState);

    connectionStatus.start();


    QState (*sendChatState) = new QState, (*sendPrivateState) = new QState;
    sendChatState->addTransition(this,SIGNAL(sendButtonChangeToPrivate()), sendPrivateState);
    sendPrivateState->addTransition(this,SIGNAL(sendButtonChangeToChat()), sendChatState);
    connect(sendChatState,SIGNAL(entered()),this,SLOT(enableSendChat()));
    connect(sendPrivateState,SIGNAL(entered()),this,SLOT(enableSendPrivate()));
    sendButtonStatus.addState(sendChatState);
    sendButtonStatus.addState(sendPrivateState);
    sendButtonStatus.setInitialState(sendChatState);

    sendButtonStatus.start();


    //Layout setup
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QHBoxLayout* send2chatLayout = new QHBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    nameLayout->addWidget(settingsButton);
    nameLayout->addWidget(new QLabel(QString::fromLocal8Bit("������:")),0,Qt::AlignRight);
    nameLayout->addWidget(stateLabel);
    leftLayout->addLayout(nameLayout);
    leftLayout->addWidget(tabWidget);
    send2chatLayout->addSpacerItem(spacer1);
   // send2chatLayout->addWidget(sendtochat);
    send2chatLayout->addSpacerItem(spacer2);
    leftLayout->addLayout(send2chatLayout);
    leftLayout->addWidget(messInput);
    rightLayout->addWidget(connbutton);
    rightLayout->addWidget(yourCompanionsLabel);//may be hidden
    rightLayout->addWidget(contlist); //may be hidden
    rightLayout->addWidget(spacer3);
   // rightLayout->addWidget(spacer4);
    rightLayout->addWidget(sendButton);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    
    setLayout(mainLayout);
    setWindowTitle(tr("Quelea"));
    setWindowIcon(QIcon::QIcon ("resource.rc"));
    setFocusProxy(textInfo);


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
    contlist->clear();
    disconn();
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
                str = QString::fromLocal8Bit("���������� �����������.");
                Message* auth_req = new Message(AUTH_REQUEST, clientName);
                SendToServer(auth_req);
                delete auth_req;
                stateLabel->setText(str);
                break;
            }

        case AUTH_RESPONSE:
           {
               if(mess->text=="auth_ok")
                {
               str = QString::fromLocal8Bit("���� ��������.");
               Message* contacts_req = new Message(CONTACTS_REQUEST);
               SendToServer(contacts_req);
               delete contacts_req;
               stateLabel->setText("<FONT COLOR=GREEN>Online</FONT>");
           }
              if(mess->text=="auth_error")
               {
                  textInfo->append("Error: Sush name is already used");
                    emit toDisconnStateBydisconn();
              }
               break;
           }
        case CONTACTS_RESPONSE:
           {
               QStringList clist = mess->text.split(";");
               contlist->clear();
               clist.removeOne(clientName);
               clist.removeOne("");
               if (clist.count()!=0)
               contlist->addItem(QString::fromLocal8Bit(">��� �����������"));
               contlist->addItems(clist);
               contlist->setCurrentRow(0);
               enableSendButton();
               break;
           }

        case MESSAGE_TO_CHAT:
            {
               if (tabWidget->currentIndex() != 0)
               tabWidget->gettabbar()->setTabTextColor (0,"Blue");

               QStringList mlist = mess->text.split(";"); // mlist[0]=�� ����, mlist[1]=����, mlist[2]=����� ���������
               if (mlist[1]=="all")
                   textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=GREEN>"+mlist[0]+"</FONT>"+": "+mlist[2]);
               else
                   textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=GREEN>"+mlist[0]+"</FONT>"+": "+"<FONT COLOR=ORANGERED>["+mlist[1]+"]</FONT> "+mlist[2]);
                break;
            }
        case MESSAGE_TO_CLIENT: // incoming private message
           {
               bool tabState=false;
               QStringList mlist = mess->text.split(";"); //mlist[0]=from who, mlist[1]=text

               for (int i=0; i<=tabWidget->count(); i++)

                  if (mlist[0]==tabWidget->tabText(i))
                   {
                      tabState=true;

                      if (tabWidget->currentIndex() != i)
                      tabWidget->gettabbar()->setTabTextColor (i,"Blue");

                      QWidget *widget = tabWidget->currentWidget();
                      QTextEdit *privateTextInfo = static_cast<QTextEdit *>(widget);
                      privateTextInfo->setReadOnly(true);
                      privateTextInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=DARKVIOLET>"+mlist[0]+"</FONT>: "+mlist[1]);
                      break;
                   }

                   if (tabState==false)
                      {
                      QTextEdit* privateTextInfo = new QTextEdit;
                      privateTextInfo->setReadOnly(true);
                      tabWidget->gettabbar()->setTabTextColor (tabWidget->addTab(privateTextInfo,mlist[0]),"Blue");
                      privateTextInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=DARKVIOLET>"+mlist[0]+"</FONT>: "+mlist[1]);
                      }

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
        "["+QTime::currentTime().toString()+"]"+" "+QString::fromLocal8Bit("������: ") + (err == QAbstractSocket::HostNotFoundError ?
                     QString::fromLocal8Bit("������ �� ������.") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     QString::fromLocal8Bit("�������� ���� ������ ����������.") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     QString::fromLocal8Bit("� ���������� ���� ��������.") :
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
    QWidget *widget = tabWidget->currentWidget();
    QTextEdit *edit = static_cast<QTextEdit *>(widget);
    edit->setReadOnly(true);

    QString str=tabWidget->tabText(tabWidget->currentIndex())+";"+messInput->text();
    Message* newmess = new Message(MESSAGE_TO_SERVER,str);
    SendToServer(newmess);
    edit->append("<FONT COLOR=BLUE>["+QTime::currentTime().toString()+"]</FONT>"+" "+"<FONT COLOR=GREEN>"+clientName+" "+"</FONT><FONT COLOR=DARKVIOLET>["+tabWidget->tabText(tabWidget->currentIndex())+"]</FONT>: "+messInput->text());
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

    sendButton->setEnabled(!messInput->text().isEmpty() && contlist->count()!=0);
}

void QueleaClient::disconn()
{
    QTime time = QTime::currentTime();
    disconnect(connbutton, SIGNAL(clicked()), this, SLOT(disconn()));
    tcpSocket->close();
    tcpSocket->abort();
    contlist->clear();
    enableSendButton();
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
           // QString str = setdial->clientName()+"\n"+setdial->serverAdr();//+"\n"+setdial->autoconnect();
            stream<<setdial->clientName()<<'\n'<< flush<<setdial->serverAdr()<<'\n'<< flush<<setdial->autoconnect();
            file.close();
        }

    }
    delete setdial;
}

void QueleaClient::addTab(QListWidgetItem * item)
{
    if (item->listWidget()->currentRow()==0)
        tabWidget->setCurrentIndex(0);
    else {

    bool tabState = true;
    for (int i=0; i<=tabWidget->count();i++)
        if (item->text()==tabWidget->tabText(i))
        {
            tabWidget->setCurrentIndex(i);
            tabState=false;
            break;
        }
    if (tabState==true)
    {
        QTextEdit* privateTextInfo = new QTextEdit;
        privateTextInfo->setReadOnly(true);
        tabWidget->setCurrentIndex(tabWidget->addTab(privateTextInfo,item->text()));
    }
        }
}

void QueleaClient::normalizeTabColor(int tab)
{
    tabWidget->gettabbar()->setTabTextColor(tab,"black");
}

void QueleaClient::closeTab(int index)
{
    tabWidget->removeTab(index);
}

void QueleaClient::enableSendChat()
{
    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendchat()));
}

void QueleaClient::enableSendPrivate()
{
    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendmess()));
}

void QueleaClient::sendButtonFunc(int index)
{
    if (index==0)
        {
         contlist->setHidden(false);
         yourCompanionsLabel->setHidden(false);
         spacer3->setHidden(true);
         contlist->setCurrentRow(0);
         emit sendButtonChangeToChat();
         disconnect(sendButton,SIGNAL(clicked()),this,SLOT(sendmess()));
        }
    else
      {
         contlist->setHidden(true);
         yourCompanionsLabel->setHidden(true);
         spacer3->setHidden(false);
         emit sendButtonChangeToPrivate();
         disconnect(sendButton,SIGNAL(clicked()),this,SLOT(sendchat()));
      }
}
