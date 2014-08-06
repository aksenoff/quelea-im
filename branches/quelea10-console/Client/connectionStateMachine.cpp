#include "connectionStateMachine.h"

ConnectionStateMachine::ConnectionStateMachine(QueleaClientUI* UI, QueleaClient* qc, SystemTray* st, bool autoConnect)
    :ui(UI), client(qc) ,tray(st)
{
    tray->assignMachine(this);
    disconnectedState = new QState;
    connectionState = new QState;
    connectedState = new QState;

    // ways to beigin connecting: pressing a button in main window
    disconnectedState->addTransition(ui, SIGNAL(connectButtonClicked()), connectionState);
    //                            clicking a tray menu
    disconnectedState->addTransition(tray, SIGNAL(changeStateByTray()), connectionState);

    // way to succeessfully end connection process: "ok" authorization response
    connectionState->addTransition(client, SIGNAL(authOkSignal()), connectedState);

    // ways to fail connection: "error" authorization response
    connectionState->addTransition(client, SIGNAL(authErrorSignal()), disconnectedState);
    //                          pressing a button in main window
    connectionState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    //                          having a socket error
    connectionState->addTransition(client, SIGNAL(socketError()), disconnectedState);
    //                          clicking a tray menu
    connectionState->addTransition(tray, SIGNAL(changeStateByTray()), disconnectedState);

    // ways to disconnect: pressing a button in main window
    connectedState->addTransition(ui, SIGNAL(connectButtonClicked()), disconnectedState);
    //                     having a socket error
    connectedState->addTransition(client, SIGNAL(socketError()), disconnectedState);
    //                     clicking a tray menu
    connectedState->addTransition(tray, SIGNAL(changeStateByTray()), disconnectedState);

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

    connect(connectionState, SIGNAL(entered()),
            this, SLOT(setConnectionState()));
    connect(connectedState, SIGNAL(entered()),
            this, SLOT(setConnectedState()));
    connect(disconnectedState, SIGNAL(entered()),
            this, SLOT(setDisconnectedState()));

    addState(disconnectedState);
    addState(connectionState);
    addState(connectedState);

    autoConnect? setInitialState(connectionState)
        :setInitialState(disconnectedState);

    start();
}

void ConnectionStateMachine::setConnectionState()
{
    currentState = "connecting";
}

void ConnectionStateMachine::setConnectedState()
{
    currentState = "online";
}

void ConnectionStateMachine::setDisconnectedState()
{
    currentState = "offline";
}

const QString& ConnectionStateMachine::currentConnectionState() const
{
    return currentState;
}
