#ifndef QUELEASERVERUI_H
#define QUELEASERVERUI_H

#include <QtWidgets>
#include "QueleaServer.h"
#include "database.h"
#include "ldapauth.h"

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
    bool useLDAP;
    bool useAdLdap;
    QString dbFileName;
    QString ldapHost;
    int ldapPort;
    QString ldapDomain;
    QString ldapAdmin;
    QString ldapAdminPw;
    QString currentIp;
    Database* db;
    LdapAuth* ldath;
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
