#ifndef CONECTIONSTATEMACHINE_H
#define CONECTIONSTATEMACHINE_H

#include <QStateMachine>
#include "QueleaClient.h"
#include "QueleaClientUI.h"
#include "systemTray.h"

class QueleaClientUI;
class SystemTray;
class QueleaClient;

class ConnectionStateMachine : public QStateMachine {
Q_OBJECT
private:
    QueleaClientUI* ui;
    QueleaClient* client;
    SystemTray* tray;
    QString currentState;
    QState* disconnectedState;
    QState* connectionState;
    QState* connectedState;
private slots:
    void setConnectionState();
    void setConnectedState();
    void setDisconnectedState();
public:
    ConnectionStateMachine(QueleaClientUI*, QueleaClient*, SystemTray*, bool);
    const QString& currentConnectionState() const;
};

#endif
