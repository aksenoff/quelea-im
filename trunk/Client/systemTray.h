#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include "QueleaClient.h"
#include "QueleaClientUI.h"

class QueleaClient;
class QueleaClientUI;

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
    QueleaClientUI* ui;
    void visibleAtNewMessage();
public:
    SystemTray(QueleaClient*, QueleaClientUI*);
protected:
    virtual void closeEvent(QCloseEvent*);
private slots:
    void slotIconActivated(QSystemTrayIcon::ActivationReason);
    void slotShowHide();
    void slotShowMessage(const QString&);
    void slotNewMessage(const QString&);
public slots:
    void slotChangeIcon(const QString&);
    void enableDisconnected();
    void enableConnection();
    void enableConnected();

signals:
    void changeStateByTray();
};

#endif
