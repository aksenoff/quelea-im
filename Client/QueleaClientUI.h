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
    QLabel* statusInscriptionLabel;
    QTextEdit*  textInfo;
    QTextEdit*  messInput;
    QListWidget* contlist;
    QPushButton* connbutton;
    QPushButton* sendButton;
    QPushButton* settingsButton;
    QPushButton* sendtochat;
    QPushButton* aboutButton;
    QSpacerItem* spacer1,*spacer2,*spacer3;
    QShortcut* sendShortcut;
    ClientTab* tabWidget;
    QString clientName;
    bool enableSound;
public:
    QueleaClientUI(QWidget* pwgt = 0);
    void setUISettings(bool sound);


private slots:
    void enableSendButton();
    void addTab(QListWidgetItem*);
    void tabChanged(int tab);
    void closeTab(int index);
    void sendButtonFunction();
    void playSound(QString reason);
    void messageReceived(QString receiver);
public slots:
    void openSettingDialog();
    void enableDisconnected();
    void enableConnection();
    void enableConnected();
    void parseMessage(Message message);
    void logAction(QString action);
    void setUItoDisconnected();
    void setCurrentTab(QString sender);

signals:
    void newMessage(QString receiver);
    void connectButtonClicked();
};

#endif
