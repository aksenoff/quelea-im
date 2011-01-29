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
    QueleaClient* client;
    QueleaClientUI* ui;
    SystemTray* st;
    QState* connectedState;
    QState* disconnectedState;
public:
    ConnectionStateMachine(QueleaClientUI*, QueleaClient*, SystemTray*);
};
#endif
