#include "client.h"

QTcpSocket* Client::getSocket()
{
    return socket;
}

QString Client::getName()
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

void Client::send(QByteArray ba)
{
    socket->write(ba);
}
