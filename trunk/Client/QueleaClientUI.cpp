#include "QueleaClientUI.h"
#include "settingsDialog.h"
#include "../codes.h"

QueleaClientUI::QueleaClientUI(QWidget* pwgt)
    : QWidget(pwgt)
{
    stateLabel = new QLabel(tr("<FONT COLOR=RED>Отключен</FONT>"));
    yourCompanionsLabel = new QLabel(tr("Ваши собеседники:"));
    QLabel* statusInscriptionLabel = new QLabel(tr("Статус:"));

    messageInput = new QTextEdit(this);
    connect(messageInput, SIGNAL(textChanged()),
            this, SLOT(enableSendButton()));

    chatLog = new QTextEdit;
    chatLog->setReadOnly(true);

    contactsList = new QListWidget;
    contactsList->resize(50, 100);
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
    connectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(connectButton, SIGNAL(clicked()),
            this, SIGNAL(connectButtonClicked()));

    aboutButton = new QPushButton(QPixmap(":/images/about.png"),tr(" О п&рограмме "));
    aboutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(aboutButton,SIGNAL(clicked()),
            this,SLOT(showAboutBox()));

    settingsButton = new QPushButton(QPixmap(":/images/settings.png"),tr("&Настройки"));
    settingsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(settingsButton, SIGNAL(clicked()),
            this, SLOT(openSettingDialog()));

    tabWidget = new ClientTab();
    tabWidget->setTabsClosable(true);
    tabWidget->addTab(chatLog, tr("Общий разговор"));
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
    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(tabWidget);
    leftLayout->addWidget(messageInput);
    sendLayout->addSpacerItem(spacer1);
    sendLayout->addWidget(sendButton);
    leftLayout->addLayout(sendLayout);
    rightLayout->addWidget(connectButton);
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

    //Reading settings:
    QFile file("settings.dat");
    bool autoConnect = false;
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream (&file);
        myName = stream.readLine();
        QString serverAddress(stream.readLine());
        autoConnect = stream.readLine().toInt();
        enableSound = stream.readLine().toInt();
        file.close();
        client = new QueleaClient(this, myName, serverAddress);
    }
    else
        openSettingDialog(); // client may be created there as well
    tray = new SystemTray(this);
    connectionState = new ConnectionStateMachine(this, client, tray, autoConnect);
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
    if (settingsDialog->exec() == QDialog::Accepted)
    {
        enableSound = settingsDialog->enableSound();
        myName = settingsDialog->clientName();
        QString serverAddress(settingsDialog->serverAddress());
        bool autoConnect = settingsDialog->autoConnect();
        if(client)
            client->changeSettings(myName, serverAddress);
        else
            client = new QueleaClient(this, myName, serverAddress);

        QFile file("settings.dat");
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
            QTextEdit* privatechatLog = new QTextEdit;
            privatechatLog->setReadOnly(true);
            tabWidget->setCurrentIndex(tabWidget->addTab(privatechatLog,item->text()));
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
        QString messageText(messageInput->toPlainText());
        client->sendChatMessage(receiverName, messageText);
        messageInput->clear();
        enableSendButton();
    }
    else
    {
        QTextEdit *privateChatLog = static_cast<QTextEdit*>(tabWidget->currentWidget());
        privateChatLog->setReadOnly(true);
        QString receiverName(tabWidget->tabText(tabWidget->currentIndex()));
        QString messageText(messageInput->toPlainText());
        client->sendPrivateMessage(receiverName, messageText);
        privateChatLog->append("<FONT COLOR=BLUE>[" + QTime::currentTime().toString() + "]</FONT>" + " "
                               + "<FONT COLOR=GREEN>" + myName + " " + "</FONT>: " + messageText);
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
            QStringList contacts = incomingMessage.getText().split(";");
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
            QStringList incomingMessageTextItems = incomingMessage.getText().split(";");
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
            QStringList incomingMessageTextItems = incomingMessage.getText().split(";");
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
    aboutBox.setInformativeText("<p>" + tr("Используется Qt 4.7.1<br>Распространяется по лизензии "
                                         "<a href=http://www.gnu.org/licenses/gpl/html>GNU GPLv3<a></p>"
                                         "<p><strong>Разработчики:</strong><br>Алексей Аксёнов (aksenoff.a@gmail.com)"
                                         "<br>Роман Сухов (romsuhov@gmail.com)<br>Алексей Топчий (alextopchiy@gmail.com)</p>")
                                + "<p><a href=http://quelea-im.googlecode.com>http://quelea-im.googlecode.com<a></p>"
                                + tr("© Разработчики Quelea, 2011"));
    aboutBox.exec();
}
//---------------------------------------------------------

QueleaClientUI::~QueleaClientUI()
{
    delete tray;
    delete connectionState;
}

