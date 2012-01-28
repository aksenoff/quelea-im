#ifndef LDAPAUTH_H
#define LDAPAUTH_H

#include <ldap.h>
#include <QString>

class LdapAuth
{
public:
    LdapAuth(QString host, QString domain); //AD auth
    LdapAuth(QString h, int p, QString d, QString admin, QString pwd); //other LDAP auth
    bool authorize(QString uid, QString password);
    static int testConnection(QString host, QString domain);
    static int testConnection(QString h, int p, QString d, QString admin, QString pwd);
private:
    LDAP *ld;
    bool ad;
    QString host;
    int port;
    QString password;
    QString domain;
    QString adminDN;
    QString adminPWD;
};

#endif // LDAPAUTH_H
