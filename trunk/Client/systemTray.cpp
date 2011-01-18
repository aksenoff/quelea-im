#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include "systemTray.h"

SystemTray::SystemTray(QWidget *pwgt /*=0*/, QueleaClient* queleaClient /*= 0*/)
    : QWidget(pwgt), newMessageExist(false),client(queleaClient)
{
    connect(client, SIGNAL(statusChanged(QString)),
            this, SLOT(slotChangeIcon(QString)));
    connect(client, SIGNAL(newMessage(QString)),
            this, SLOT(slotNewMessage(QString)));

    actShowHide = new QAction(tr("Скрыть"),this);
    connect(actShowHide, SIGNAL(triggered()),
            this, SLOT(slotShowHide()));

    actChangeStatus = new QAction(tr("Подключиться"),this);
    connect(actChangeStatus, SIGNAL(triggered()),
            this, SLOT(slotChangeStatus()));

    actSettings = new QAction(tr("Настройки..."),this);
    connect(actSettings, SIGNAL(triggered()),
            client, SLOT(openSettingDialog()));

    actQuit = new QAction(tr("Выход"),this);
    connect(actQuit, SIGNAL(triggered()),
            qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(actShowHide);
    trayIconMenu->setDefaultAction(actShowHide);
    trayIconMenu->addAction(actChangeStatus);
    trayIconMenu->addAction(actSettings);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("Quelea"));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));

    slotChangeIcon();
    trayIcon->show();
}

SystemTray::~SystemTray()
{
    delete actShowHide;
    delete actChangeStatus;
    delete actSettings;
    delete actQuit;
    delete trayIcon;
    delete trayIconMenu;
}

void SystemTray::closeEvent(QCloseEvent * pe)
{
    if(trayIcon->isVisible()){
        client->hide();
    }
}

void SystemTray::slotIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        {
            slotShowHide();
            break;
        }
    default:
        ;
    }
}

void SystemTray::slotShowHide()
{
    client->setVisible(!(client->isVisible()));

    if (client->isVisible()){
        actShowHide->setText(tr("Скрыть"));
        client->activateWindow();
        if (newMessageExist)
            visibleAtNewMessage();
    }
    else
        actShowHide->setText(tr("Показать"));
}

void SystemTray::slotShowMessage(QString senderName)
{
    if (senderName=="all")
        trayIcon->showMessage(tr("Сообщение в чате"),tr("Сообщение в чате для вас"),QSystemTrayIcon::NoIcon,3000);
    else
        trayIcon->showMessage(tr("Новое сообщение"),tr("Отправитель: ")+senderName,QSystemTrayIcon::NoIcon,3000);
}


 void SystemTray::slotChangeIcon(QString status)
 {
    QString pixmapName;
    if (status=="offline"){
    pixmapName ="/icon-offline.png";
    actChangeStatus->setText(tr("Подключиться"));
    }
    else if(status=="message")
        pixmapName="/message.png";
    else{
        pixmapName ="/icon.png";
        actChangeStatus->setText(tr("Отключиться"));
    }
    trayIcon->setIcon(QPixmap(pixmapName));
 }

 void SystemTray::slotChangeStatus()
 {
    if(client->clientStatus=="offline")
        client->conn();
    else
        client->disconn();
 }

 void SystemTray::slotNewMessage(QString senderName)
 {
     if (!client->isVisible()){
        slotChangeIcon("message");
        slotShowMessage(senderName);
     }
     else
         if (!client->isActiveWindow())
             QApplication::alert(client);
     newMessageExist = true;
     newMessageSenderName = senderName;
 }

 void SystemTray::visibleAtNewMessage()
 {
     if (newMessageSenderName=="all")
         client->tabWidget->setCurrentIndex(0);
     else{
         for (int i=0; i<=client->tabWidget->count(); i++)
             if (newMessageSenderName==client->tabWidget->tabText(i)){
                 client->tabWidget->setCurrentIndex(i);
                 break;
             }
     }
     slotChangeIcon(client->clientStatus);
     newMessageExist = false;
     newMessageSenderName = "";
 }
