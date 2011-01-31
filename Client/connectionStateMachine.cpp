#include "connectionStateMachine.h"

ConnectionStateMachine::ConnectionStateMachine(QueleaClientUI* userInterface, QueleaClient* c, SystemTray* s, bool autoConnect)
    :ui(userInterface), client(c) ,st(s)
{
    disconnectedState = new QState;
    connectionState = new QState;
    connectedState = new QState;

    disconnectedState->addTransition(ui, SIGNAL(connectButtonClicked()), connectionState);
    disconnectedState->addTransition(st,SIGNAL(changeStateByTray()),connectionState);

    connectionState->addTransition(client, SIGNAL(authOkSignal()), connectedState);
    connectionState->addTransition(client, SIGNAL(authErrorSignal()), disconnectedState);
    connectionState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    connectionState->addTransition(client, SIGNAL(socketError()),disconnectedState);
    connectionState->addTransition(st,SIGNAL(changeStateByTray()),disconnectedState);

    connectedState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    connectedState->addTransition(client, SIGNAL(socketError()),disconnectedState);
    connectedState->addTransition(st,SIGNAL(changeStateByTray()),disconnectedState);

    connect(disconnectedState, SIGNAL(entered()),
            ui, SLOT(enableDisconnected()));
    connect(connectionState, SIGNAL(entered()),
            ui, SLOT(enableConnection()));
    connect(connectedState, SIGNAL(entered()),
            ui, SLOT(enableConnected()));

    connect(disconnectedState, SIGNAL(entered()),
            st, SLOT(enableDisconnected()));
    connect(connectionState,SIGNAL(entered()),
            st, SLOT(enableConnection()));
    connect(connectedState,SIGNAL(entered()),
            st, SLOT(enableConnected()));

    connect(disconnectedState, SIGNAL(entered()),
            client, SLOT(disconnectFromServer()));
    connect(connectionState,SIGNAL(entered()),
            client, SLOT(connectToServer()));

    addState(disconnectedState);
    addState(connectionState);
    addState(connectedState);
    autoConnect? setInitialState(connectionState)
        :setInitialState(disconnectedState);
    start();
}
