#ifndef MESSAGE_H
#define MESSAGE_H

#include <QWidget>
#include <QtNetwork>

class Message
{
    unsigned char code;
    QString text;
public:
    operator int() const;
    QString getText() const;
    Message(unsigned char, QString="");
    Message(QTcpSocket*);
    void send(QTcpSocket*) const;
};

#endif
