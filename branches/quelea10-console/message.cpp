#include "message.h"

Message::operator int() const
{
    return code;
}
const QString& Message::getText() const
{
    return text;
}

Message::Message(unsigned char c, QString s)
    : code(c), text(s)
{
}

Message::Message(QTcpSocket* clientSocket)
{
    QDataStream incomingStream(clientSocket);
    incomingStream.setVersion(QDataStream::Qt_4_6);
    quint64 fullSize = 0;
    incomingStream >> fullSize;
    while(quint64(clientSocket->bytesAvailable()) < fullSize)
        clientSocket->waitForReadyRead(500);
    incomingStream >> code >> text;
}

void Message::send(QTcpSocket* socket) const
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint64(0) << code << text;
    out.device()->seek(0);
    out << quint64(arrBlock.size() - sizeof(quint64));
    socket->write(arrBlock);
}
