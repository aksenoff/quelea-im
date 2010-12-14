#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include<QLabel>

class QSystemTrayIcon;
class QMenu;

class SystemTray : public QWidget{
Q_OBJECT
private:
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    bool iconSwitcher;

protected:
    virtual void closeEvent(QCloseEvent *);

public:
    SystemTray(QWidget* pwgt = 0);

public slots:
    void slotShowHide();
    void slotShowMessage();
    void slotChangeIcon();
};

#endif // SYSTEMTRAY_H
