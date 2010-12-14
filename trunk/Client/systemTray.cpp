#include <QSystemTrayIcon>
#include<QAction>
#include<QMenu>
#include<QApplication>
#include "systemTray.h"

SystemTray::SystemTray(QWidget *pwgt /*=0*/, QueleaClient* client /*= 0*/)
    : QWidget(pwgt),iconSwitcher(false),qc(client)
{
QAction* actShowHide = new QAction("Show/Hide",this);
connect(actShowHide,SIGNAL(triggered()),this,SLOT(slotShowHide()));
QAction* actShowMessage = new QAction("Show message",this);
connect(actShowMessage,SIGNAL(triggered()),this,SLOT(slotShowMessage()));

QAction* actChangeIcon = new QAction("Change icon",this);
connect(actChangeIcon,SIGNAL(triggered()),this,SLOT(slotChangeIcon()));

QAction* actQuit = new QAction("Quit",this);
connect(actQuit,SIGNAL(triggered()),qApp,SLOT(quit()));

trayIconMenu = new QMenu(this);
trayIconMenu->addAction(actShowHide);
trayIconMenu->addAction(actShowMessage);
trayIconMenu->addAction(actChangeIcon);
trayIconMenu->addAction(actQuit);

trayIcon = new QSystemTrayIcon(this);
trayIcon->setContextMenu(trayIconMenu);
trayIcon->setToolTip("Quelea");

slotChangeIcon();
trayIcon->show();
}

/*virual*/void SystemTray::closeEvent(QCloseEvent * pe)
{
  if (trayIcon->isVisible()){
     hide();
     }
}


 void SystemTray::slotShowHide()
 {
 qc->setVisible(!(qc->isVisible()));
 }

 void SystemTray::slotShowMessage()
 {
     trayIcon->showMessage("Information","This is information",QSystemTrayIcon::Information,3000);
 }


 void SystemTray::slotChangeIcon()
 {
 iconSwitcher = !iconSwitcher;
 QString pixmapName = iconSwitcher ? "img1.bmp":"img2.bmp";
 trayIcon->setIcon(QPixmap(pixmapName));
 }
