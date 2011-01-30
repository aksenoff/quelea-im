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
    void  visibleAtNewMessage();
public:
    SystemTray(QWidget* pwgt = 0, QueleaClient* qc = 0, QueleaClientUI* userInterface = 0);
    ~SystemTray();
protected:
    virtual void closeEvent(QCloseEvent *);
private slots:
    void slotIconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotShowHide();
    void slotShowMessage(QString senderName);
    void slotNewMessage(QString senderName);
public slots:
    void slotChangeIcon(QString status="offline");
    void enableDisconnected();
    void enableConnected();

signals:
    void changeStateByTray();
};

#endif // SYSTEMTRAY_H
