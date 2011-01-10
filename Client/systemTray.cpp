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

actShowHide = new QAction(tr("Скрыть"),this);
connect(actShowHide,SIGNAL(triggered()),this,SLOT(slotShowHide()));

actChangeStatus = new QAction(tr("Подключиться"),this);
connect(actChangeStatus,SIGNAL(triggered()),this,SLOT(slotChangeStatus()));

actSettings = new QAction(tr("Настройки..."),this);
connect(actSettings,SIGNAL(triggered()),qc,SLOT(openSettingDialog()));

actQuit = new QAction(tr("Выход"),this);
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
trayIcon->setToolTip(tr("Quelea"));

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
   actShowHide->setText(tr("Скрыть"));
   qc->activateWindow();
   if (nMess){
       if (receiverName=="all")
           qc->tabWidget->setCurrentIndex(0);
       else{
         for (int i=0; i<=qc->tabWidget->count();i++)
               if (receiverName==qc->tabWidget->tabText(i)){
                   qc->tabWidget->setCurrentIndex(i);
                   break;
               }
       }
      slotChangeIcon(qc->clientStatus);
      nMess = false;
      receiverName="";
   }
 }
 else
    actShowHide->setText(tr("Показать"));
 }

 void SystemTray::slotShowMessage(QString receiver)
 {
     if (receiver=="all")
         trayIcon->showMessage(tr("Сообщение из чата"),tr("Сообщение из чата для вас"),QSystemTrayIcon::Information,3000);
     else
         trayIcon->showMessage(tr("Новое сообщение"),tr("Новое сообщение от ")+receiver,QSystemTrayIcon::Information,3000);
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
 else {
    pixmapName ="/icon.png";
    actChangeStatus->setText(tr("Отключиться"));
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
