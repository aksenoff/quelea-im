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
    quint16 nextBlockSize = 0;
    QVector<QString> textBlocks;
    QString temp("");
    incomingStream >> nextBlockSize >> code;
    nextBlockSize = 0;
    while(true)
    {
        if (!nextBlockSize) {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            incomingStream >> nextBlockSize;
        }

        if (clientSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
        incomingStream >> temp;
        textBlocks.push_back(temp);
        nextBlockSize = 0;
    }
    for(QVector<QString>::const_iterator i = textBlocks.begin(); i != textBlocks.end(); ++i)
        text += *i;
}

void Message::send(QTcpSocket* socket) const
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    QString temp("");
    out << quint16(sizeof(code)) << code;
    socket->write(arrBlock);
    arrBlock.clear();
    temp = text;
    while(temp.size() > 1000)
    {
        out.device()->seek(0);
        out << quint16(0) << temp.left(1000);
        temp = temp.right(temp.size() - 1000);
        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
        arrBlock.clear();
    }
    if(temp.size() != 0)
    {
        out.device()->seek(0);
        out << quint16(0) << temp;
        //    QFile file("test.txt");
        //    QTextStream stream(&file);
        //    stream << text;
        //    file.close();
        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}
