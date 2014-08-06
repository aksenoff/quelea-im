#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include "connectionStateMachine.h"
#include "QueleaClientUI.h"

class ConnectionStateMachine;
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
    bool newMessageExists;
    ConnectionStateMachine* stateMachine;
    QueleaClientUI* ui;
    void visibleAtNewMessage();
public:
    SystemTray(QueleaClientUI*);
    void assignMachine(ConnectionStateMachine*);
    void setConnectionActionEnabled(bool);
protected:
    virtual void closeEvent(QCloseEvent*);
private slots:
    void slotIconActivated(QSystemTrayIcon::ActivationReason);
    void slotShowMessage(const QString&);
    void slotNewMessage(const QString&);
public slots:
    void slotShowHide();
    void slotChangeIcon(const QString&);
    void enableDisconnected();
    void enableConnection();
    void enableConnected();
signals:
    void changeStateByTray();
};

#endif
