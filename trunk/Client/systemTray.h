#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include<QLabel>
#include"QueleaClient.h"

class QSystemTrayIcon;
class QMenu;

class SystemTray : public QWidget{
Q_OBJECT
private:
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    bool iconSwitcher;
    QAction* actShowHide;
    QAction* actChangeStatus;
    QAction* actSettings;
    QAction* actQuit;
    QString receiverName;
    bool nMess;
    QueleaClient* qc;

protected:
    virtual void closeEvent(QCloseEvent *);

public:
    SystemTray(QWidget* pwgt = 0, QueleaClient* qc = 0);

public slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotShowHide();
    void slotShowMessage(QString receiver);
    void slotChangeIcon(QString status="offline");
    void slotChangeStatus();
    void slotNewMessage(QString receiver);
};

#endif // SYSTEMTRAY_H
