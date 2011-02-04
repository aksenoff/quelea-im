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
    quint16 nextBlockSize;
    incomingStream >> nextBlockSize >> code >> text;
}

void Message::send(QTcpSocket* socket) const
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint16(0) << code << text;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);
}
