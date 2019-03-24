#include "ldapauth.h"
#include <ldap.h>
#include <QDebug>

LdapAuth::LdapAuth(QString host, QString domain):host(host),domain(domain)
{
    ad = true;
    port = 389;
}

//-------------------------------------------------------------------------------

LdapAuth::LdapAuth(QString h, int p, QString d,QString admin, QString pwd):
    host(h),port(p), domain(d),adminDN(admin),adminPWD(pwd)
{
    ad = false;
}

//-------------------------------------------------------------------------------

void LdapAuth::setSettings(QString h, QString d)
{
    ad = true;
    host = h;
    domain = d;
}

//-------------------------------------------------------------------------------

void LdapAuth::setSettings(QString h, int p, QString d, QString admin, QString pwd)
{
    ad = false;
    host = h;
    port = p;
    domain = d;
    adminDN = admin;
    adminPWD = pwd;

}
//-------------------------------------------------------------------------------
int LdapAuth::testConnection(QString h)
{
    LDAP *ld;
    QString uri = "ldap://"+h+":"+QString::number(389);
    QByteArray ba1 = uri.toLatin1();
    if (ldap_initialize(&ld,ba1.data())== LDAP_SUCCESS)
        return 1;
    else
        return 0;
 }

//-------------------------------------------------------------------------------

int LdapAuth::testConnection(QString h, int p, QString admin, QString pwd)
{
    LDAP *ld;
    int desired_version = LDAP_VERSION3;
    QString uri = "ldap://"+h+":"+QString::number(p);
    QByteArray ba1 = uri.toLatin1();
    if (ldap_initialize(&ld,ba1.data())!= LDAP_SUCCESS)
        return 0;
    ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version);

    struct berval cred;
    QByteArray adn = admin.toLatin1();
    cred.bv_val =  pwd.toLatin1().data();
    cred.bv_len = pwd.length();
    if(ldap_sasl_bind_s(ld,adn.data(),NULL,&cred,NULL,NULL,NULL)== LDAP_SUCCESS)
        return 1;
    else
        return 0;
}
//-------------------------------------------------------------------------------
bool LdapAuth::authorize(QString uid, QString password)
{
    LDAP *ld;
    int desired_version = LDAP_VERSION3;
    QString uri = "ldap://"+host+":"+QString::number(port);
    QByteArray ba1 = uri.toLatin1();
    ldap_initialize(&ld,ba1.data());
    ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version);
    if(ad){
        QString dn = uid+"@"+domain;
        QByteArray ba2 = dn.toLatin1();
        struct berval cred;
        cred.bv_val =  password.toLatin1().data();
        cred.bv_len = password.length();
        if(ldap_sasl_bind_s(ld,ba2.data(),NULL,&cred,NULL,NULL,NULL) == LDAP_SUCCESS)
            return true;
        return false;
    }
    else {
        LDAPMessage* msg;
        LDAPMessage* entry;
        struct berval cred;
        QByteArray adn = adminDN.toLatin1();
        cred.bv_val =  adminPWD.toLatin1().data();
        cred.bv_len = adminPWD.length();
        ldap_sasl_bind_s(ld,adn.data(),NULL,&cred,NULL,NULL,NULL);

        QByteArray base = domain.toLatin1();
        QString filter ="(uid="+uid+")";
        QByteArray fi = filter.toLatin1();
        ldap_search_ext_s(ld, base.data(), LDAP_SCOPE_SUBTREE, fi.data(), NULL, 0, NULL,NULL,NULL,0,&msg);
        entry = ldap_first_entry(ld, msg);
        if (entry==NULL)
            return false;
        char* sdn  = ldap_get_dn(ld,entry);
        cred.bv_val = password.toLatin1().data();
        cred.bv_len = password.length();
        if(ldap_sasl_bind_s(ld,sdn,NULL,&cred,NULL,NULL,NULL) == LDAP_SUCCESS)
            return true;
        return false;
    }
}
//-------------------------------------------------------------------------------
