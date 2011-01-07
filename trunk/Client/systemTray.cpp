#include <QSystemTrayIcon>
#include<QAction>
#include<QMenu>
#include<QApplication>
#include "systemTray.h"

SystemTray::SystemTray(QWidget *pwgt /*=0*/, QueleaClient* client /*= 0*/)
    : QWidget(pwgt),iconSwitcher(false), nMess(false),qc(client)
{

connect(qc,SIGNAL(statusChanged(QString)),this,SLOT(slotChangeIcon(QString)));
connect(qc,SIGNAL(newMessage(QString)),this,SLOT(slotNewMessage(QString)));

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
   if (nMess){
       for (int i=0; i<=qc->tabWidget->count();i++)
           if (receiverName==qc->tabWidget->tabText(i)){
                qc->tabWidget->setCurrentIndex(i);
                break;
            }
      slotChangeIcon(qc->clientStatus);
      nMess = false;
      receiverName="";
   }
 }
 else
    actShowHide->setText(tr("Show"));
 }

 void SystemTray::slotShowMessage(QString receiver)
 {
     trayIcon->showMessage("New message","New message from "+receiver,QSystemTrayIcon::Information,3000);
 }


 void SystemTray::slotChangeIcon(QString status)
 {
 QString pixmapName;
 if (status=="offline"){
    pixmapName ="/icon-offline.png";
    actChangeStatus->setText(tr("Connect"));
 }
 else if(status=="message")
     pixmapName="/message.png";
 else {
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

 void SystemTray::slotNewMessage(QString receiver)
 {
     if (!qc->isVisible()){
        slotChangeIcon("message");
        slotShowMessage(receiver);
    }
     else
         if (!qc->isActiveWindow())
             QApplication::alert(qc);
    nMess = true;
    receiverName = receiver;

 }
