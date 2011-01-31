#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include "systemTray.h"

SystemTray::SystemTray(QWidget *pwgt /*=0*/, QueleaClient* queleaClient /*= 0*/, QueleaClientUI* userInterface /*= 0*/)
    : QWidget(pwgt), newMessageExist(false), client(queleaClient), ui(userInterface)
{
    connect(client, SIGNAL(statusChanged(const QString&)),
            this, SLOT(slotChangeIcon(const QString&)));
    connect(ui, SIGNAL(newMessage(const QString&)),
            this, SLOT(slotNewMessage(const QString&)));

    actShowHide = new QAction(tr("Скрыть"), this);
    connect(actShowHide, SIGNAL(triggered()),
            this, SLOT(slotShowHide()));

    actChangeStatus = new QAction(tr("Подключиться"), this);
    connect(actChangeStatus,SIGNAL(triggered()),
            this, SIGNAL(changeStateByTray()));

    actSettings = new QAction(tr("Настройки..."), this);
    connect(actSettings, SIGNAL(triggered()),
            ui, SLOT(openSettingDialog()));

    actQuit = new QAction(tr("Выход"), this);
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

    trayIcon->show();
}

//---------------------------------------------------------

void SystemTray::closeEvent(QCloseEvent * pe)
{
    if(trayIcon->isVisible())
    {
        ui->hide();
    }
}

//---------------------------------------------------------

void SystemTray::slotIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        {
            slotShowHide(); // gownocode detected. Roma, kamenti?
            break;
        }
    default:
        ;
    }
}

//---------------------------------------------------------

void SystemTray::slotShowHide()
{
    ui->setVisible(!(ui->isVisible()));

    if (ui->isVisible())
    {
        actShowHide->setText(tr("Скрыть"));
        ui->activateWindow();
        if (newMessageExist)
            visibleAtNewMessage();
    }
    else
        actShowHide->setText(tr("Показать"));
}

//---------------------------------------------------------

void SystemTray::slotShowMessage(const QString& senderName)
{
    if (senderName=="all")
        trayIcon->showMessage(tr("Сообщение в чате"),tr("Сообщение в чате для вас"),QSystemTrayIcon::NoIcon,3000);
    else
        trayIcon->showMessage(tr("Новое сообщение"),tr("Отправитель: ")+senderName,QSystemTrayIcon::NoIcon,3000);
}

//---------------------------------------------------------

void SystemTray::slotChangeIcon(const QString& status)
{
    QString pixmapName;
    if (status=="offline")
        pixmapName ="/icon-offline.png";

    else if(status=="message")
        pixmapName="/message.png";
    else
        pixmapName ="/icon.png";

    trayIcon->setIcon(QPixmap(pixmapName));
}

//---------------------------------------------------------

void SystemTray::slotNewMessage(const QString& senderName)
{
    if (ui->isVisible())
    {
        slotChangeIcon("message");
        slotShowMessage(senderName);
    }
    else
        if (!ui->isActiveWindow())
            QApplication::alert(ui);
    newMessageExist = true;
    newMessageSenderName = senderName;
}

//---------------------------------------------------------

void SystemTray::visibleAtNewMessage()
{
    ui->setCurrentTab(newMessageSenderName); //?
    slotChangeIcon(client->getStatus());
    newMessageExist = false;
    newMessageSenderName = "";
}

//---------------------------------------------------------

void SystemTray::enableDisconnected()
{
    slotChangeIcon("offline");
    actChangeStatus->setText(tr("Подключиться"));
}

//---------------------------------------------------------

void SystemTray::enableConnection()
{
    actChangeStatus->setText(tr("Отключиться"));
}

//---------------------------------------------------------

void SystemTray::enableConnected()
{
    slotChangeIcon("online");
}
