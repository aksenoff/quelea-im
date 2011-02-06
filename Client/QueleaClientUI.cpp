#include "QueleaClientUI.h"
#include "settingsDialog.h"
#include "../codes.h"

QueleaClientUI::QueleaClientUI(QWidget* pwgt)
    : QWidget(pwgt)
{
    stateLabel = new QLabel(tr("<FONT COLOR=RED>Отключен</FONT>"));
    stateLabel->setFixedWidth(100);
    yourCompanionsLabel = new QLabel(tr("Ваши собеседники:"));
    QLabel* statusInscriptionLabel = new QLabel(tr("Статус:"));

    messageInput = new QTextEdit(this);
    messageInput->document()->setDefaultFont(QFont("Arial",11));
    connect(messageInput, SIGNAL(textChanged()),
            this, SLOT(enableSendButton()));

    chatLog = new QTextEdit;
    chatLog->setReadOnly(true);
    chatLog->document()->setDefaultFont((QFont("Arial",11)));

    contactsList = new QListWidget;
    contactsList->setFont((QFont("Arial",10)));
    contactsList->setMinimumWidth(150);
    contactsList->setFocusProxy(messageInput);
    contactsList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    connect(contactsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(addTab(QListWidgetItem*)));
    connect(contactsList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem *)),
            this, SLOT(enableSendButton()));

    sendButton = new QPushButton(QPixmap(":/images/send.png"),tr("&Отправить"));
    sendButton->setEnabled(false);
    sendButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(sendButton, SIGNAL(clicked()),
            this, SLOT(sendButtonFunction()));

    connectButton = new QPushButton(tr(" &Подключиться "));
    connectButton->setFixedSize(110, 24);
    connectButton->setEnabled(false); // we don't know if connection settings are present
    connect(connectButton, SIGNAL(clicked()),
            this, SIGNAL(connectButtonClicked()));

    aboutButton = new QPushButton(QPixmap(":/images/about.png"),tr(" О п&рограмме "));
    aboutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(aboutButton,SIGNAL(clicked()),
            this,SLOT(showAboutBox()));

    settingsButton = new QPushButton(QPixmap(":/images/settings.png"),tr(" &Настройки "));
    settingsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(settingsButton, SIGNAL(clicked()),
            this, SLOT(openSettingDialog()));

    tabWidget = new ClientTab();
    tabWidget->setTabsClosable(true);
    tabWidget->addTab(chatLog, tr("Общий разговор"));
    tabWidget->getTabBar()->setTabButton(0, QTabBar::RightSide,0);
    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(closeTab(int)));

    QShortcut* sendShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Return ), this);
    connect(sendShortcut, SIGNAL(activated()),
            sendButton, SLOT(click()));
    QSpacerItem* spacer1 = new QSpacerItem(300, 0, QSizePolicy::MinimumExpanding);
    QSpacerItem* spacer2 = new QSpacerItem(0, 29);
    QSpacerItem* spacer3 = new QSpacerItem(0, 30);

    //returning focus to messageInput QTextEdit:
    connect(contactsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            messageInput, SLOT(setFocus()));
    connect(contactsList, SIGNAL(itemClicked(QListWidgetItem*)),
            messageInput, SLOT(setFocus()));
    connect(contactsList, SIGNAL(clicked(QModelIndex)),
            messageInput, SLOT(setFocus()));
    connect(connectButton, SIGNAL(clicked()),
            messageInput, SLOT(setFocus()));
    connect(sendButton, SIGNAL(clicked()),
            messageInput, SLOT(setFocus()));

    //Layout setup
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QHBoxLayout* sendLayout = new QHBoxLayout;

    buttonLayout->addWidget(aboutButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(statusInscriptionLabel, 0, Qt::AlignRight);
    buttonLayout->addWidget(stateLabel);
    buttonLayout->addWidget(connectButton);
    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(tabWidget, 3);
    leftLayout->addWidget(messageInput, 1);
    sendLayout->addSpacerItem(spacer1);
    sendLayout->addWidget(sendButton);
    leftLayout->addLayout(sendLayout);
    rightLayout->addSpacerItem(spacer3);
    rightLayout->addWidget(yourCompanionsLabel);//may be hidden
    rightLayout->addWidget(contactsList); //may be hidden
    rightLayout->addSpacerItem(spacer2);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    resize(560, 370);
    setWindowTitle(tr("Quelea"));
    setWindowIcon(QIcon::QIcon ("resource.rc"));
    messageInput->setFocus();
    client = 0;
    connectionState = 0;

    //Reading settings:
    QFile file("settings.dat");
    bool autoConnect = false;
    tray = new SystemTray(this);
    tray->setConnectionActionEnabled(false); // we don't know if connection settings are present
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        myName = stream.readLine();
        serverAddress = stream.readLine();
        autoConnect = stream.readLine().toInt();
        enableSound = stream.readLine().toInt();
        file.close();
        if(!(myName.isEmpty() || serverAddress.isEmpty())) // connection settings are present, whew
        {
            client = new QueleaClient(this, myName, serverAddress); // creating client and state machine
            connectionState = new ConnectionStateMachine(this, client, tray, autoConnect);
            tray->setConnectionActionEnabled(true); // allowing user
            connectButton->setEnabled(true);        // to connect
        }
        else // setting provided by the file are unsuffucient
            openSettingDialog();
    }
    else
        openSettingDialog(); // client may be created there as well
}

//---------------------------------------------------------

void QueleaClientUI::enableSendButton()
{
    bool currentContactExist = false;
    for (int i = 1; i < contactsList->count(); i++)
        if (contactsList->item(i)->text() == tabWidget->tabText(tabWidget->currentIndex()) || tabWidget->currentIndex() == 0){
            currentContactExist = true;
            break;
        }
    sendButton->setEnabled(!messageInput->toPlainText().isEmpty() && contactsList->count() != 0 && currentContactExist);
}

//---------------------------------------------------------

void QueleaClientUI::openSettingDialog()
{

    SettingsDialog* settingsDialog = new SettingsDialog;
    connect(settingsDialog, SIGNAL(finished(int)),
            messageInput, SLOT(setFocus()));
    if (settingsDialog->exec() == QDialog::Accepted) // ok button pressed
    {
        enableSound = settingsDialog->enableSound();
        myName = settingsDialog->clientName();
        serverAddress = settingsDialog->serverAddress();
        bool autoConnect = settingsDialog->autoConnect();
        if(!(myName.isEmpty() || serverAddress.isEmpty())) // we have connection settings
        {
            if(client) // if client is present, update its settings
            {
                client->changeSettings(myName, serverAddress);
                connectButton->setEnabled(true);        // allowing user
                tray->setConnectionActionEnabled(true); // to connect
            }
            else // seems to be the first time we entered settings
            {    // creating client and state machine
                client = new QueleaClient(this, myName, serverAddress);
                connectionState = new ConnectionStateMachine(this, client, tray, autoConnect);
                connectButton->setEnabled(true);        // allowing user
                tray->setConnectionActionEnabled(true); // to connect
            }
        }
        else  // the fields were left blank
            if(connectionState) // if client and state machine exist (i.e. user deleted connection settings)
            {                   // and we're offline, disable connection ways
                if(connectionState->currentConnectionState() == "offline")
                {               // (if we're online, they'll be disabled automatically after disconnection)
                    connectButton->setEnabled(false);
                    tray->setConnectionActionEnabled(false);
                }
            }
        QFile file("settings.dat"); // anyway, write to the file anything we have
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << myName << '\n'
                   << serverAddress << '\n'
                   << autoConnect << '\n'
                   << enableSound << flush;
            file.close();
        }
    }
    disconnect(settingsDialog, SIGNAL(finished(int)),
               messageInput, SLOT(setFocus()));
    delete settingsDialog;
}

//---------------------------------------------------------

void QueleaClientUI::addTab(QListWidgetItem * item)
{
    if (item->listWidget()->currentRow() == 0)
        tabWidget->setCurrentIndex(0);
    else
    {
        bool tabState = true;
        for (int i = 0; i <= tabWidget->count(); i++)
            if (item->text()==tabWidget->tabText(i))
            {
                tabWidget->setCurrentIndex(i);
                tabState = false;
                break;
            }
        if (tabState==true)
        {
            QTextEdit* privateChatLog = new QTextEdit;
            privateChatLog->setReadOnly(true);
            privateChatLog->document()->setDefaultFont((QFont("Arial",11)));
            tabWidget->setCurrentIndex(tabWidget->addTab(privateChatLog, item->text()));
        }
    }
}

//---------------------------------------------------------

void QueleaClientUI::tabChanged(int tab)
{
    tabWidget->getTabBar()->setTabTextColor(tab,"black");
    if (tab != 0)
    {
        contactsList->setHidden(true);
        yourCompanionsLabel->setHidden(true);
        setWindowTitle(tr("Разговор: ")
                       + tabWidget->tabText(tabWidget->currentIndex())
                       + tr(" - Quelea"));
    }
    else
    {
        contactsList->setHidden(false);
        contactsList->setCurrentRow(0);
        yourCompanionsLabel->setHidden(false);
        setWindowTitle(tr("Чат - Quelea"));
    }
    enableSendButton();
}

//---------------------------------------------------------

void QueleaClientUI::closeTab(int index)
{
    tabWidget->removeTab(index);
}

//---------------------------------------------------------

void QueleaClientUI::sendButtonFunction()
{
    if (tabWidget->currentIndex() == 0)
    {
        QString receiverName = (contactsList->currentRow() == 0)? "all":
                                                                  contactsList->currentItem()->text();
        QString messageText(messageInput->toHtml());
        client->sendChatMessage(receiverName, messageText);
        messageInput->clear();
        enableSendButton();
    }
    else
    {
        QTextEdit *privateChatLog = static_cast<QTextEdit*>(tabWidget->currentWidget());
        privateChatLog->setReadOnly(true);
        QString receiverName(tabWidget->tabText(tabWidget->currentIndex()));
        QString messageText(messageInput->toHtml());
        client->sendPrivateMessage(receiverName, messageText);
        privateChatLog->append("<FONT COLOR=BLUE>[" + QTime::currentTime().toString() + "]</FONT>" + " "
                               + "<FONT COLOR=GREEN>" + myName + "</FONT>: " + messageText);
        messageInput->clear();
        enableSendButton();
      }
}

//---------------------------------------------------------

void QueleaClientUI::messageReceived(const QString& receiver)
{
    emit newMessage(receiver);
    playSound("message");
}

//---------------------------------------------------------

void QueleaClientUI::playSound(const QString& reason) const
{
    if (QSound::isAvailable() && enableSound)
            QSound::play(":/sound/"+reason+".wav");
}

//---------------------------------------------------------

void QueleaClientUI::parseMessage(const Message& incomingMessage)
{
    QTime time = QTime::currentTime();
    switch(incomingMessage)
    {
    case CONTACTS_RESPONSE:
        {
            QStringList contacts = incomingMessage.getText().split(QChar::Null);
            contactsList->clear();
            contacts.removeOne(myName);
            contacts.removeOne("");
            if (contacts.count() != 0)
                contactsList->addItem(tr(">Все собеседники"));
            contacts.sort();
            contactsList->addItems(contacts);
            contactsList->setCurrentRow(0);

            bool tabFound = false;
            int i;
            for (i = 1; i < tabWidget->count() && !(tabFound); i++)
            {
                 bool matchFound = false;
                 for (int j = 1; j < contactsList->count() && !(matchFound); j++)
                     if (tabWidget->tabText(i) == contactsList->item(j)->text())
                         matchFound = true;
                     if(!matchFound)
                         tabFound = true;
            }
            if (tabFound == true){
                QWidget* widget = tabWidget->widget(i-1);
                QTextEdit* privateChatLog = static_cast<QTextEdit*>(widget);
                privateChatLog->setReadOnly(true);
                privateChatLog->append("<FONT COLOR=GRAY>[" + time.toString() + "]"+tr(" Клиент ")
                                       +tabWidget->tabText(i-1)+tr(" отключен")+"</FONT>");
            }

            enableSendButton();
            tabChanged(tabWidget->currentIndex());
            break;
        }
    case MESSAGE_TO_CHAT:
        {
            if (tabWidget->currentIndex() != 0)
                tabWidget->getTabBar()->setTabTextColor (0, "Blue");
            QStringList incomingMessageTextItems = incomingMessage.getText().split(QChar::Null);
            QString senderName(incomingMessageTextItems[0]);
            QString receiverName(incomingMessageTextItems[1]);
            QString actualMessage(incomingMessageTextItems[2]);
            QString fromWhoColor = "GREEN";
            if (senderName == myName)
                fromWhoColor = "DARKVIOLET";
            if (receiverName == "all")
                chatLog->append("<FONT COLOR=BLUE>[" + time.toString() + "]</FONT>"+ " "
                                + "<FONT COLOR=" + fromWhoColor + ">" + senderName + "</FONT>" + ": "
                                + actualMessage);
            else
            {
                QString toWhoColor = "ORANGERED";
                if (receiverName == myName)
                {
                    toWhoColor = "DARKVIOLET";
                    emit newMessage("all");
                    playSound("chat");
                }
                chatLog->append("<FONT COLOR=BLUE>[" + time.toString() + "]</FONT>" + " "
                                + "<FONT COLOR=" + fromWhoColor + ">" + senderName + "</FONT>" + ": "
                                + "<FONT COLOR=" + toWhoColor + ">[" + receiverName + "]</FONT> "
                                + actualMessage);
            }
            break;
        }
    case MESSAGE_TO_CLIENT: // incoming private message
        {
            bool tabState = false;
            QStringList incomingMessageTextItems = incomingMessage.getText().split(QChar::Null);
            QString senderName(incomingMessageTextItems[0]);
            QString actualMessage(incomingMessageTextItems[1]);
            for (int i = 0; i <= tabWidget->count(); i++)
                if (senderName == tabWidget->tabText(i))
                {
                    tabState = true;
                    if (tabWidget->currentIndex() != i)
                        tabWidget->getTabBar()->setTabTextColor(i, "Blue");
                    QWidget* widget = tabWidget->widget(i);
                    QTextEdit* privateChatLog = static_cast<QTextEdit*>(widget);
                    privateChatLog->setReadOnly(true);
                    privateChatLog->append("<FONT COLOR=BLUE>[" + time.toString() + "]</FONT>" + " "
                                           + "<FONT COLOR=DARKVIOLET>" + senderName + "</FONT>: "
                                           + actualMessage);
                    break;
                }
            if (tabState == false)
            {
                QTextEdit* privateChatLog = new QTextEdit;
                privateChatLog->setReadOnly(true);
                privateChatLog->document()->setDefaultFont(QFont("Arial",11));
                tabWidget->getTabBar()->setTabTextColor(tabWidget->addTab(privateChatLog, senderName), "Blue");
                privateChatLog->append("<FONT COLOR=BLUE>[" + time.toString() + "]</FONT>" + " "
                                       + "<FONT COLOR=DARKVIOLET>" + senderName + "</FONT>: "
                                       + actualMessage);
            }
            messageReceived(senderName);
            break;
        }        
    }
}

//---------------------------------------------------------

void QueleaClientUI::log(const QString& event)
{
    chatLog->append(event);
}

//---------------------------------------------------------

void QueleaClientUI::enableDisconnected()
{
    connectButton->setText(" " + tr("&Подключиться") + " ");
    connectButton->setIcon(QPixmap(":/images/connect.png"));
    stateLabel->setText("<FONT COLOR=RED>" + tr("Отключен") + "</FONT>");
    contactsList->clear();
    enableSendButton();
    if(myName.isEmpty() || serverAddress.isEmpty()) // settings disappeared?
        connectButton->setEnabled(false);
}

//---------------------------------------------------------

void QueleaClientUI::enableConnection()
{
    connectButton->setText(" " + tr("О&тключиться") + " ");
    connectButton->setIcon(QPixmap(":/images/disconnect.png"));
    stateLabel->setText(tr("Подключение..."));
}                            

//---------------------------------------------------------

void QueleaClientUI::enableConnected()
{
    stateLabel->setText("<FONT COLOR=GREEN>" + tr("В сети") + "</FONT>");
}

//---------------------------------------------------------

void QueleaClientUI::setCurrentTab(const QString& senderName)
{
    if (senderName == "all")
        tabWidget->setCurrentIndex(0);
    else
    {
        for (int i = 0; i <= tabWidget->count(); i++)
            if (senderName == tabWidget->tabText(i))
            {
                tabWidget->setCurrentIndex(i);
                break;
            }
    }
}

//---------------------------------------------------------

void QueleaClientUI::showAboutBox()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle(tr("О программе - Quelea"));
    aboutBox.setIconPixmap(QPixmap(":/images/icon.png"));
    aboutBox.setText("<strong>"+tr("Quelea 1.0 beta")+"</strong>");
    aboutBox.setInformativeText("<p>" + tr("Используется Qt 4.7.0<br>Распространяется по лицензии "
                                         "<a href=http://www.gnu.org/licenses/gpl/html>GNU GPLv3<a></p>"
                                         "<p><strong>Разработчики:</strong><br>Алексей Аксёнов (aksenoff.a@gmail.com)"
                                         "<br>Роман Сухов (romsuhov@gmail.com)<br>Алексей Топчий (alextopchiy@gmail.com)</p>")
                                + "<p><a href=http://quelea-im.googlecode.com>http://quelea-im.googlecode.com<a></p>"
                                + tr("© Разработчики Quelea, 2011"));
    aboutBox.exec();
}

//---------------------------------------------------------

void QueleaClientUI::closeEvent(QCloseEvent *event)
{
    event->ignore();
    tray->slotShowHide();
}

//---------------------------------------------------------

QueleaClientUI::~QueleaClientUI()
{
    delete tray;
    delete connectionState;
}

