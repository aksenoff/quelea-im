
#ifndef QUELEASERVER_H
#define QUELEASERVER_H

#include "../message.h"
#include <QtNetwork>
#include "QueleaServerUI.h"
#include "client.h"
#include "database.h"
#include "ldapauth.h"

class QueleaServerUI;
class Database;

class QueleaServer : public QTcpServer {
Q_OBJECT
private:
    enum {GUEST_AUTH, DB_AUTH, LDAP_AUTH};
    QString ipAddress;
    quint16 port;
    QVector<Client*> clients;
    QueleaServerUI* ui;
    Database* db;
    LdapAuth* ldath;
public:
    QueleaServer(const QString&, QueleaServerUI*, Database*, LdapAuth*);
    ~QueleaServer();
private slots:
    bool nameCollision(QString&);
    void authOk(Client*);
    void authError(Client*);
public slots:
    void slotNewConnection();
    void slotReadClient();
    void slotByeClient(QTcpSocket*);
    void sendFile();
};

#endif
