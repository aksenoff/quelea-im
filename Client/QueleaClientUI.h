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
    QueleaClient* client;
    SystemTray* tray;
    ConnectionStateMachine* connectionState;
    QLabel* stateLabel;
    QLabel* yourCompanionsLabel;
    QTextEdit* chatLog;
    QTextEdit* messageInput;
    QListWidget* contactsList;
    QPushButton* connectButton;
    QPushButton* sendButton;
    QPushButton* settingsButton;
    QPushButton* aboutButton;
    ClientTab* tabWidget;
    QString myName;
    QString serverAddress;
    bool enableSound;
public:
    QueleaClientUI(QWidget* pwgt = 0);
    ~QueleaClientUI();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void enableSendButton();
    void addTab(QListWidgetItem*);
    void tabChanged(const int);
    void closeTab(const int);
    void sendButtonFunction();
    void playSound(const QString&) const;
    void showAboutBox();
    void messageReceived(const QString&);
public slots:
    void openSettingDialog();
    void enableDisconnected();
    void enableConnection();
    void enableConnected();
    void parseMessage(const Message& message);
    void log(const QString&);
    void setCurrentTab(const QString&);
signals:
    void newMessage(const QString&);
    void connectButtonClicked();
};

#endif
