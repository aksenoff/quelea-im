#ifndef QUELEASERVERUI_H
#define QUELEASERVERUI_H

#include <QtGui>
#include "QueleaServer.h"

class QueleaServer;

class QueleaServerUI : public QWidget {
Q_OBJECT
private:
    QTextEdit* serverLog;
    QComboBox* ipBox;
    QueleaServer* server;
    QVBoxLayout* mainLayout;
    QHBoxLayout* buttonsLayout;
    QPushButton* startStopButton;
    QPushButton* aboutButton;
    bool populateIpBox();
    QString currentIp;
public:
    QueleaServerUI(QWidget* pwgt = 0);
    void log(const QString&) const;
    ~QueleaServerUI();
private slots:
    void showAboutBox();
    void startServer();
    void stopServer();
};

#endif