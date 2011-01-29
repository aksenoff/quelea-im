#include "connectionStateMachine.h"

ConnectionStateMachine::ConnectionStateMachine(QueleaClientUI* userInterface, QueleaClient* c, SystemTray* s)
    :ui(userInterface), client(c) ,st(s)
{
    connectedState = new QState;
    disconnectedState = new QState;

    connectedState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    connectedState->addTransition(client, SIGNAL(socketError()),disconnectedState);
    connectedState->addTransition(client, SIGNAL(disconnectSignal()), disconnectedState);
    connectedState->addTransition(st,SIGNAL(disconnectByTray()),disconnectedState);

    disconnectedState->addTransition(client, SIGNAL(startedConnect()), connectedState);
    disconnectedState->addTransition(st,SIGNAL(connectByTray()),connectedState);

    connect(connectedState, SIGNAL(entered()), ui, SLOT(enableConnected()));
    connect(disconnectedState, SIGNAL(entered()), ui, SLOT(enableDisconnected()));

    addState(connectedState);
    addState(disconnectedState);
    setInitialState(disconnectedState);
    start();
}
