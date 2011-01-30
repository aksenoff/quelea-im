#include "QueleaClientUI.h"
#include "settingsDialog.h"
#include "systemTray.h"
#include "../codes.h"

QueleaClientUI::QueleaClientUI(QWidget* pwgt) : QWidget(pwgt)
{
    stateLabel = new QLabel(tr("<FONT COLOR=RED>Отключен</FONT>"));
    yourCompanionsLabel = new QLabel(tr("Ваши собеседники:"));
    QLabel* statusInscriptionLabel = new QLabel(tr("Статус:"));

    messInput = new QTextEdit(this);
    connect(messInput, SIGNAL(textChanged()),this, SLOT(enableSendButton()));

    textInfo  = new QTextEdit;
    textInfo->setReadOnly(true);

    contlist = new QListWidget;
    contlist->resize(50,100);
    contlist->setFocusProxy(messInput);
    contlist->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    connect(contlist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(addTab(QListWidgetItem*)));
    connect(contlist, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem *)),
            this, SLOT(enableSendButton()));

    sendButton = new QPushButton(tr("&Отправить"));
    sendButton->setEnabled(false);
    sendButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(sendButton, SIGNAL(clicked()),
            this, SLOT(sendButtonFunction()));

    connbutton = new QPushButton(tr(" &Подключиться "));
    connbutton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(connbutton, SIGNAL(clicked()),
            this, SIGNAL(connectButtonClicked()));

    aboutButton = new QPushButton(tr(" О п&рограмме "));
    aboutButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    settingsButton = new QPushButton(tr("&Настройки"));
    settingsButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(settingsButton, SIGNAL(clicked()),
            this, SLOT(openSettingDialog()));

    tabWidget = new ClientTab();
    tabWidget->setTabsClosable(true);
    tabWidget->addTab(textInfo,tr("Общий разговор"));
    tabWidget->getTabBar()->setTabButton(0,QTabBar::RightSide,0);
    connect(tabWidget ,SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(closeTab(int)));

    QShortcut* sendShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Return ), this);
    connect(sendShortcut, SIGNAL(activated()),
            sendButton, SLOT(click()));
    QSpacerItem* spacer1 = new QSpacerItem(300, 0, QSizePolicy::MinimumExpanding);
    QSpacerItem* spacer2 = new QSpacerItem(0, 29);
    QSpacerItem* spacer3 = new QSpacerItem(0, 30);

    //returning focus to messInput QTextEdit:
    connect(contlist, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            messInput, SLOT(setFocus()));
    connect(contlist, SIGNAL(itemClicked(QListWidgetItem*)),
            messInput, SLOT(setFocus()));
    connect(contlist, SIGNAL(clicked(QModelIndex)),
            messInput, SLOT(setFocus()));
    connect(connbutton, SIGNAL(clicked()),
            messInput, SLOT(setFocus()));
    connect(sendButton, SIGNAL(clicked()),
            messInput, SLOT(setFocus()));

    //Layout setup
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QHBoxLayout* sendLayout = new QHBoxLayout;

    buttonLayout->addWidget(aboutButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(statusInscriptionLabel,0,Qt::AlignRight);
    buttonLayout->addWidget(stateLabel);
    buttonLayout->addWidget(connbutton);
    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(tabWidget);
    leftLayout->addWidget(messInput);
    sendLayout->addSpacerItem(spacer1);
    sendLayout->addWidget(sendButton);
    leftLayout->addLayout(sendLayout);
    rightLayout->addSpacerItem(spacer3);
    rightLayout->addWidget(yourCompanionsLabel);//may be hidden
    rightLayout->addWidget(contlist); //may be hidden
    rightLayout->addSpacerItem(spacer2);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    resize(560, 370);
    setWindowTitle(tr("Quelea"));
    setWindowIcon(QIcon::QIcon ("resource.rc"));
    messInput->setFocus();
    client = 0;

    //Reading settings:

    QFile file("settings.dat");
    QString serverAddress("");
    bool autoConnect;
    if (file.open(QIODevice::ReadOnly)){
        QTextStream stream (&file);
        clientName = stream.readLine();
        serverAddress = stream.readLine();
        autoConnect = stream.readLine().toInt();
        enableSound = stream.readLine().toInt();
        file.close();
        client = new QueleaClient(this, clientName, serverAddress);
    }
    else
        openSettingDialog();
    st = new SystemTray(0, client, this);
    connectionState = new ConnectionStateMachine(this, client, st, autoConnect);
}

void QueleaClientUI::enableSendButton()
{
    sendButton->setEnabled(!messInput->toPlainText().isEmpty() && contlist->count()!=0);
}

void QueleaClientUI::openSettingDialog()
{

    SettingsDialog* setdial = new SettingsDialog;
    connect(setdial,SIGNAL(finished(int)),messInput,SLOT(setFocus()));
    QString serverAddress;
    bool autoConnect;
    if (setdial->exec() == QDialog::Accepted) {
        enableSound = setdial->enableSound();
        clientName = setdial->clientName();
        serverAddress = setdial->serverAddress();
        autoConnect = setdial->autoconnect();
        if(client)
            client->changeSettings(clientName, serverAddress);
        else
            client = new QueleaClient(this, clientName, serverAddress);

        QFile file("settings.dat");
        if (file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            stream << clientName << '\n' << serverAddress << '\n' << autoConnect << '\n' << enableSound << flush;
            file.close();
        }
    }
    delete setdial;
}

void QueleaClientUI::addTab(QListWidgetItem * item)
{
    if (item->listWidget()->currentRow()==0)
        tabWidget->setCurrentIndex(0);
    else {
        bool tabState = true;
        for (int i=0; i<=tabWidget->count();i++)
        if (item->text()==tabWidget->tabText(i)){
            tabWidget->setCurrentIndex(i);
            tabState = false;
            break;
        }
         if (tabState==true){
            QTextEdit* privateTextInfo = new QTextEdit;
            privateTextInfo->setReadOnly(true);
            tabWidget->setCurrentIndex(tabWidget->addTab(privateTextInfo,item->text()));
         }
     }
}

void QueleaClientUI::tabChanged(int tab)
{
    tabWidget->getTabBar()->setTabTextColor(tab,"black");
    if (tab!=0){
        contlist->setHidden(true);
        yourCompanionsLabel->setHidden(true);
        setWindowTitle(tr("Разговор: ")+tabWidget->tabText(tabWidget->currentIndex())+tr(" - Quelea"));
    }
    else{
        contlist->setHidden(false);
        yourCompanionsLabel->setHidden(false);
        setWindowTitle(tr("Чат - Quelea "));
    }
}

void QueleaClientUI::closeTab(int index)
{
    tabWidget->removeTab(index);
}

void QueleaClientUI::sendButtonFunction()
{
    if (tabWidget->currentIndex()==0){
        QString receiver;
        if (contlist->currentRow()==0)
            receiver = "all";
        else
            receiver = contlist->currentItem()->text();
        QString messsageText = messInput->toPlainText();
        client->sendchat(receiver, messsageText);
        messInput->clear();
        enableSendButton();
     }
    else{  
        QWidget *widget = tabWidget->currentWidget();
        QTextEdit *edit = static_cast<QTextEdit *>(widget);
        edit->setReadOnly(true);
        QString receiver(tabWidget->tabText(tabWidget->currentIndex())), messageText(messInput->toPlainText());
        client->sendmess(receiver, messageText);
        edit->append("<FONT COLOR=BLUE>["+QTime::currentTime().toString()+"]</FONT>"+" "+"<FONT COLOR=GREEN>"+clientName+" "+"</FONT>: "+messInput->toPlainText());
        messInput->clear();
        enableSendButton();
      }
}

void QueleaClientUI::messageReceived(QString& receiver)
{
    emit newMessage(receiver);
    QString reason = "message";
    playSound(reason);
}

void QueleaClientUI::playSound(QString& reason)
{
    if (QSound::isAvailable())
        if (enableSound)
            QSound::play("/"+reason+".wav");
}

void QueleaClientUI::parseMessage(Message& message)
{
    QTime time = QTime::currentTime();
    QString str;
    switch(message){

    case CONTACTS_RESPONSE:
        {
            QStringList clist = message.getText().split(";");
            contlist->clear();
            clist.removeOne(clientName);
            clist.removeOne("");
            if (clist.count()!=0)
                contlist->addItem(tr(">Все собеседники"));
            contlist->addItems(clist);
            contlist->setCurrentRow(0);
            enableSendButton();
            tabChanged(tabWidget->currentIndex());
            break;
        }

    case MESSAGE_TO_CHAT:
        {
            if (tabWidget->currentIndex() != 0)
                tabWidget->getTabBar()->setTabTextColor (0,"Blue");

            QStringList mlist = message.getText().split(";"); // mlist[0]=от кого, mlist[1]=кому, mlist[2]=текст сообщения
            QString fromWhoColor = "GREEN";
            if (mlist[0]==clientName)
                fromWhoColor = "DARKVIOLET";
            if (mlist[1]=="all")
                textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR="+fromWhoColor+">"+mlist[0]+"</FONT>"+": "+mlist[2]);
            else{
                QString toWhoColor = "ORANGERED";
                if (mlist[1]==clientName){
                    toWhoColor = "DARKVIOLET";
                    QString receiver = "all";
                    emit newMessage(receiver);
                    QString reason = "chat";
                    playSound(reason);
                }
                textInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR="+fromWhoColor+">"+mlist[0]+"</FONT>"+": "+"<FONT COLOR="+toWhoColor+">["+mlist[1]+"]</FONT> "+mlist[2]);
            }
            break;
        }
    case MESSAGE_TO_CLIENT: // incoming private message
        {
            bool tabState=false;
            QStringList mlist = message.getText().split(";"); //mlist[0]=from who, mlist[1]=text

            for (int i=0; i<=tabWidget->count(); i++)

                if (mlist[0]==tabWidget->tabText(i))
                {
                tabState=true;

                if (tabWidget->currentIndex() != i)
                    tabWidget->getTabBar()->setTabTextColor (i,"Blue");

                QWidget *widget = tabWidget->widget(i);
                QTextEdit *privateTextInfo = static_cast<QTextEdit *>(widget);
                privateTextInfo->setReadOnly(true);
                privateTextInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=DARKVIOLET>"+mlist[0]+"</FONT>: "+mlist[1]);
                break;
            }

            if (tabState==false)
            {
                QTextEdit* privateTextInfo = new QTextEdit;
                privateTextInfo->setReadOnly(true);
                tabWidget->getTabBar()->setTabTextColor (tabWidget->addTab(privateTextInfo,mlist[0]),"Blue");
                privateTextInfo->append("<FONT COLOR=BLUE>["+time.toString()+"]</FONT>"+ " "+"<FONT COLOR=DARKVIOLET>"+mlist[0]+"</FONT>: "+mlist[1]);
            }

            messageReceived(mlist[0]);

            break;
        }

    }
}

void QueleaClientUI::logAction(QString& action)
{
    textInfo->append(action);
}

void QueleaClientUI::enableDisconnected()
{
    connbutton->setText(tr(" &Подключиться "));
    stateLabel->setText(tr("<FONT COLOR=RED>Отключен</FONT>"));
    //st->slotChangeIcon("offline");
    client->disconn();
    contlist->clear();
    enableSendButton();
}

void QueleaClientUI::enableConnection()
{
    connbutton->setText(tr(" О&тключиться "));
    stateLabel->setText(tr("Соединение..."));
    client->conn();
}                            


void QueleaClientUI::enableConnected()
{
    stateLabel->setText(tr("<FONT COLOR=GREEN>В сети</FONT>"));
   // st->slotChangeIcon("online");
}

void QueleaClientUI::setCurrentTab(QString& sender)
{
    if (sender=="all")
        tabWidget->setCurrentIndex(0);
    else{
        for (int i=0; i<=tabWidget->count(); i++)
            if (sender == tabWidget->tabText(i)){
                tabWidget->setCurrentIndex(i);
                break;
            }
    };
}

QueleaClientUI::~QueleaClientUI()
{
    delete st;
    delete connectionState;
}
