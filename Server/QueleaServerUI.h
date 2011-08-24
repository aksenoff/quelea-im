#ifndef QUELEASERVERUI_H
#define QUELEASERVERUI_H

#include <QtGui>
#include "QueleaServer.h"
#include "database.h"

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
    QPushButton* settingsButton;
    bool useDB;
    QString dbFileName;
    QString currentIp;
    Database* db;
public:
    QueleaServerUI(QWidget* pwgt = 0);
    void log(const QString&) const;
    ~QueleaServerUI();
private slots:
    void openSettingDialog();
    void showAboutBox();
    void startServer();
    void stopServer();
    void writeSettings(bool writeGlobal);
    void readSettings(QString settingsPath);
    bool populateIpBox();
};

#endif
