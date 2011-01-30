#include "connectionStateMachine.h"

ConnectionStateMachine::ConnectionStateMachine(QueleaClientUI* userInterface, QueleaClient* c, SystemTray* s)
    :ui(userInterface), client(c) ,st(s)
{
    disconnectedState = new QState;
    connectionState = new QState;
    connectedState = new QState;

    disconnectedState->addTransition(ui, SIGNAL(connectButtonClicked()), connectionState);
    disconnectedState->addTransition(st,SIGNAL(connectByTray()),connectionState);

    connectionState->addTransition(client, SIGNAL(authOkSignal()), connectedState);
    connectionState->addTransition(client, SIGNAL(authErrorSignal()), disconnectedState);
    connectionState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    connectionState->addTransition(client, SIGNAL(socketError()),disconnectedState);
    connectionState->addTransition(st,SIGNAL(disconnectByTray()),disconnectedState);

    connectedState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    connectedState->addTransition(client, SIGNAL(socketError()),disconnectedState);
    connectedState->addTransition(st,SIGNAL(disconnectByTray()),disconnectedState);

    connect(disconnectedState, SIGNAL(entered()), ui, SLOT(enableDisconnected()));
    connect(disconnectedState, SIGNAL(exited()), ui, SLOT(disableDisconnected()));
    connect(connectionState, SIGNAL(entered()), ui, SLOT(enableConnection()));
    connect(connectedState, SIGNAL(entered()), ui, SLOT(enableConnected()));

    addState(disconnectedState);
    addState(connectionState);
    addState(connectedState);
    setInitialState(disconnectedState);
    start();
}
