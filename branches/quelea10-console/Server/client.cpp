#include "client.h"

QTcpSocket* Client::getSocket() const
{
    return socket;
}

QString Client::getName() const
{
    return name;
}

Client::Client(const QString &n, QTcpSocket *s)
    :name(n), socket(s)
{
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(socketClosed()));
}

void Client::socketClosed()
{
    emit goodbye(socket);
}

