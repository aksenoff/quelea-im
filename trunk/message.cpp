#include "message.h"

Message::operator int()
{
    return code;
}
QString Message::gettext()
{
    return text;
}

Message::Message(unsigned char c, QString s)
    :code(c),text(s)
{}

QDataStream& operator<<(QDataStream& out, const Message& m)
{
    return out << m.code << m.text;
}

QDataStream& operator>>(QDataStream& in, Message*& m) 
{
    unsigned char code = '\0';
    QString text("");
    QDataStream& ds = in >> code >> text;
    if(!m) m = new Message(code, text);
    return ds;
}

