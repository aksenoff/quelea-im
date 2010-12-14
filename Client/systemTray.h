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
    QueleaClient* qc;

protected:
    virtual void closeEvent(QCloseEvent *);

public:
    SystemTray(QWidget* pwgt = 0, QueleaClient* qc = 0);

public slots:
    void slotShowHide();
    void slotShowMessage();
    void slotChangeIcon();
};

#endif // SYSTEMTRAY_H
