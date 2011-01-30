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
    SystemTray* st;
    QState* disconnectedState;
    QState* connectionState;
    QState* connectedState;
public:
    ConnectionStateMachine(QueleaClientUI*, QueleaClient*, SystemTray*, bool);
};
#endif
