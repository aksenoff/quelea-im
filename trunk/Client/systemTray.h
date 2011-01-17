#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include "QueleaClient.h"

class QSystemTrayIcon;
class QMenu;

class SystemTray : public QWidget{
Q_OBJECT
private:
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QAction* actShowHide;
    QAction* actChangeStatus;
    QAction* actSettings;
    QAction* actQuit;
    QString receiverName;
    bool nMess;
    QueleaClient* client;
protected:
    virtual void closeEvent(QCloseEvent *);

public:
    SystemTray(QWidget* pwgt = 0, QueleaClient* qc = 0);
    ~SystemTray();

public slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotShowHide();
    void slotShowMessage(QString receiver);
    void slotChangeIcon(QString status="offline");
    void slotChangeStatus();
    void slotNewMessage(QString receiver);
};

#endif // SYSTEMTRAY_H
