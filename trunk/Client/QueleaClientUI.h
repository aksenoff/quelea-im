#ifndef QUELEACLIENTUI_H
#define QUELEACLIENTUI_H

#include <QtGui>
#include "QueleaClient.h"
#include "systemTray.h"
#include "clientTab.h"
#include "../message.h"
#include "connectionStateMachine.h"

class QueleaClient;
class SystemTray;
class ConnectionStateMachine;

class QueleaClientUI : public QWidget {
Q_OBJECT
private:
    enum {GUEST_AUTH, DB_AUTH, LDAP_AUTH};
    QueleaClient* client;
    SystemTray* tray;
    ConnectionStateMachine* connectionState;
    QLabel* stateLabel;
    QLabel* yourCompanionsLabel;
    QLabel* currentLengthLabel;
    QTextEdit* chatLog;
    QTextEdit* messageInput;
    QListWidget* contactsList;
    QPushButton* connectButton;
    QPushButton* sendButton;
    QPushButton* sendFileButton;
    QPushButton* settingsButton;
    QPushButton* aboutButton;
    ClientTab* tabWidget;
    QString myName;
    QString serverAddress;
    QString dbName;
    QString dbPassword;
    QString ldapName;
    QString ldapPassword;
    bool enableSound;
    bool autoConnect;
    int authType;
public:
    QueleaClientUI(QWidget* pwgt = 0);
    ~QueleaClientUI();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void readSettings(QString settingsPath);
    void writeSettings(bool writeGlobal);
    void calculateLength();
    void enableSendButton();
    void addTab(QListWidgetItem*);
    void tabChanged(const int);
    void closeTab(const int);
    void sendButtonFunction();
    void playSound(const QString&) const;
    void showAboutBox();
    void messageReceived(const QString&);
    void setClientSettings();
    void sendFile();
public slots:
    void openSettingDialog();
    void enableDisconnected();
    void enableConnection();
    void enableConnected();
    void parseMessage(const Message& message);
    void log(const QString&);
    void setCurrentTab(const QString&);
    void slotShow();
signals:
    void newMessage(const QString&);
    void connectButtonClicked();
};

#endif
