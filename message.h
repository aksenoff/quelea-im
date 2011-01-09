#ifndef MESSAGE_H
#define MESSAGE_H

#include <QWidget>

class Message
{
    unsigned char code;
    QString text;
public:
    operator int();
    QString gettext();
    friend QDataStream& operator<<(QDataStream&, const Message&);
    friend QDataStream& operator>>(QDataStream&, Message*&);
    Message(unsigned char, QString="");
};

#endif
