#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include "QueleaClient.h"

class SystemTray : public QWidget{
Q_OBJECT
private:
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QAction* actShowHide;
    QAction* actChangeStatus;
    QAction* actSettings;
    QAction* actQuit;
    QString newMessageSenderName;
    bool newMessageExist;
    QueleaClient* client;
    void  visibleAtNewMessage();
public:
    SystemTray(QWidget* pwgt = 0, QueleaClient* qc = 0);
    ~SystemTray();
protected:
    virtual void closeEvent(QCloseEvent *);
private slots:
    void slotIconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotShowHide();
    void slotShowMessage(QString senderName);
    void slotChangeIcon(QString status="offline");
    void slotChangeStatus();
    void slotNewMessage(QString senderName);
};

#endif // SYSTEMTRAY_H
