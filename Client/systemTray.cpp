#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include "systemTray.h"

SystemTray::SystemTray(QueleaClientUI* userInterface)
    : newMessageExists(false), ui(userInterface)
{
    connect(ui, SIGNAL(newMessage(const QString&)),
            this, SLOT(slotNewMessage(const QString&)));

    actShowHide = new QAction(tr("Скрыть"), this);
    connect(actShowHide, SIGNAL(triggered()),
            this, SLOT(slotShowHide()));

    actChangeStatus = new QAction(tr("Подключиться"), this);
    connect(actChangeStatus,SIGNAL(triggered()),
            this, SIGNAL(changeStateByTray()));

    actSettings = new QAction(QPixmap(":/images/settings.png"),tr("Настройки..."), this);
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

    trayIcon = new QSystemTrayIcon(QPixmap(":/images/icon-offline.png"),this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("Quelea"));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(trayIcon, SIGNAL(messageClicked()),
            this, SLOT(slotShowHide()));

    trayIcon->show();
}

//---------------------------------------------------------

void SystemTray::assignMachine(ConnectionStateMachine* machine)
{
    stateMachine = machine;
}

//---------------------------------------------------------

void SystemTray::setConnectionActionEnabled(bool enabled)
{
    actChangeStatus->setEnabled(enabled);
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
    if (reason == QSystemTrayIcon::Trigger)
        slotShowHide();
}

//---------------------------------------------------------

void SystemTray::slotShowHide()
{
    ui->setVisible(!(ui->isVisible()));

    if (ui->isVisible())
    {
        actShowHide->setText(tr("Скрыть"));
        ui->activateWindow();
        if (newMessageExists)
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
        pixmapName =":/images/icon-offline.png";

    else if(status=="message")
        pixmapName=":/images/message.png";
    else
        pixmapName =":/images/icon.png";

    trayIcon->setIcon(QPixmap(pixmapName));
}

//---------------------------------------------------------

void SystemTray::slotNewMessage(const QString& senderName)
{
    if (!ui->isVisible())
    {
        slotChangeIcon("message");
        slotShowMessage(senderName);
    }
    else
        if (!ui->isActiveWindow())
            QApplication::alert(ui);
    newMessageExists = true;
    newMessageSenderName = senderName;
}

//---------------------------------------------------------

void SystemTray::visibleAtNewMessage()
{
    ui->setCurrentTab(newMessageSenderName);
    slotChangeIcon(stateMachine->currentConnectionState());
    newMessageExists = false;
    newMessageSenderName = "";
}

//---------------------------------------------------------

void SystemTray::enableDisconnected()
{
    slotChangeIcon("offline");
    actChangeStatus->setText(tr("Подключиться"));
    actChangeStatus->setIcon(QPixmap(":/images/connect.png"));
}

//---------------------------------------------------------

void SystemTray::enableConnection()
{
    actChangeStatus->setText(tr("Отключиться"));
    actChangeStatus->setIcon(QPixmap(":/images/disconnect.png"));
}

//---------------------------------------------------------

void SystemTray::enableConnected()
{
    slotChangeIcon("online");
}
