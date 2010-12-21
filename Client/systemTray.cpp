#include <QSystemTrayIcon>
#include<QAction>
#include<QMenu>
#include<QApplication>
#include "systemTray.h"

SystemTray::SystemTray(QWidget *pwgt /*=0*/, QueleaClient* client /*= 0*/)
    : QWidget(pwgt),iconSwitcher(false),qc(client)
{

connect(qc,SIGNAL(statusChanged(QString)),this,SLOT(slotChangeIcon(QString)));

actShowHide = new QAction(tr("Hide"),this);
connect(actShowHide,SIGNAL(triggered()),this,SLOT(slotShowHide()));

//actShowMessage = new QAction("Show message",this);
//connect(actShowMessage,SIGNAL(triggered()),this,SLOT(slotShowMessage()));

actChangeStatus = new QAction(tr("Connect"),this);
connect(actChangeStatus,SIGNAL(triggered()),this,SLOT(slotChangeStatus()));

actSettings = new QAction(tr("Settings..."),this);
connect(actSettings,SIGNAL(triggered()),qc,SLOT(openSettingDialog()));

actQuit = new QAction(tr("Quit"),this);
connect(actQuit,SIGNAL(triggered()),qApp,SLOT(quit()));

trayIconMenu = new QMenu(this);
trayIconMenu->addAction(actShowHide);
trayIconMenu->setDefaultAction(actShowHide);
trayIconMenu->addAction(actChangeStatus);
trayIconMenu->addAction(actSettings);
trayIconMenu->addSeparator();
trayIconMenu->addAction(actQuit);

trayIcon = new QSystemTrayIcon(this);
trayIcon->setContextMenu(trayIconMenu);
trayIcon->setToolTip("Quelea");

connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

slotChangeIcon();
trayIcon->show();
}

/*virual*/void SystemTray::closeEvent(QCloseEvent * pe)
{
  if (trayIcon->isVisible()){
     qc->hide();
     }
}

void SystemTray::iconActivated(QSystemTrayIcon::ActivationReason reason)
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
 qc->setVisible(!(qc->isVisible()));

 if (qc->isVisible())
 {
   actShowHide->setText(tr("Hide"));
   qc->activateWindow();
 }
 else
    actShowHide->setText(tr("Show"));
 }

 void SystemTray::slotShowMessage()
 {
     trayIcon->showMessage("Information","This is information",QSystemTrayIcon::Information,3000);
 }


 void SystemTray::slotChangeIcon(QString status)
 {
 QString pixmapName;
 if (status=="offline")
 {
    pixmapName ="/icon-offline.png";
    actChangeStatus->setText(tr("Connect"));
 }
 else
 {
    pixmapName ="/icon.png";
    actChangeStatus->setText(tr("Disconnect"));
 }
 trayIcon->setIcon(QPixmap(pixmapName));
 }

 void SystemTray::slotChangeStatus()
 {


    if(qc->clientStatus=="offline")
        qc->conn();
    else
        qc->disconn();

 }

