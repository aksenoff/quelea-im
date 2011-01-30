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
    SystemTray* st;
    ConnectionStateMachine* connectionState;
    QLabel* stateLabel;
    QLabel* yourCompanionsLabel;
    QTextEdit* textInfo;
    QTextEdit* messInput;
    QListWidget* contlist;
    QPushButton* connbutton;
    QPushButton* sendButton;
    QPushButton* settingsButton;
    QPushButton* aboutButton;
    ClientTab* tabWidget;
    QString clientName;
    bool enableSound;
public:
    QueleaClientUI(QWidget* pwgt = 0);
    ~QueleaClientUI();
private slots:
    void enableSendButton();
    void addTab(QListWidgetItem*);
    void tabChanged(int);
    void closeTab(int);
    void sendButtonFunction();
    void playSound(QString&);
    void messageReceived(QString&);
public slots:
    void openSettingDialog();
    void enableDisconnected();
    void enableConnection();
    void enableConnected();
    void parseMessage(Message& message);
    void logAction(QString&);
    void setCurrentTab(QString&);
signals:
    void newMessage(QString&);
    void connectButtonClicked();
};

#endif
