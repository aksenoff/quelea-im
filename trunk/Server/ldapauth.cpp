#include "ldapauth.h"
#include <ldap.h>
#include <QDebug>

LdapAuth::LdapAuth(QString host, QString domain):host(host),domain(domain)
{
    ad = true;
    port = 389;
}

LdapAuth::LdapAuth(QString h, int p, QString d,QString admin, QString pwd):
    host(h),port(p), domain(d),adminDN(admin),adminPWD(pwd)
{
    ad = false;
}

bool LdapAuth::authorize(QString uid, QString password)
{
    LDAP *ld;
    int desired_version = LDAP_VERSION3;
    QString uri = "ldap://"+host+":"+QString::number(port);
    QByteArray ba1 = uri.toAscii();
    ldap_initialize(&ld,ba1.data());
    ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version);
    if(ad){
        QString dn = uid+"@"+domain;
        QByteArray ba2 = dn.toAscii();
        struct berval cred;
        cred.bv_val =  password.toAscii().data();
        cred.bv_len = password.length();
        if(ldap_sasl_bind_s(ld,ba2.data(),NULL,&cred,NULL,NULL,NULL) == LDAP_SUCCESS)
            return true;
        return false;
    }
    else {
        LDAPMessage* msg;
        LDAPMessage* entry;
        struct berval cred;
        QByteArray adn = adminDN.toAscii();
        cred.bv_val =  adminPWD.toAscii().data();
        cred.bv_len = adminPWD.length();
        ldap_sasl_bind_s(ld,adn.data(),NULL,&cred,NULL,NULL,NULL);

        QByteArray base = domain.toAscii();
        QString filter ="(uid="+uid+")";
        QByteArray fi = filter.toAscii();
        ldap_search_ext_s(ld, base.data(), LDAP_SCOPE_SUBTREE, fi.data(), NULL, 0, NULL,NULL,NULL,0,&msg);
        entry = ldap_first_entry(ld, msg);
        if (entry==NULL)
            return false;
        char* sdn  = ldap_get_dn(ld,entry);
        cred.bv_val = password.toAscii().data();
        cred.bv_len = password.length();
        if(ldap_sasl_bind_s(ld,sdn,NULL,&cred,NULL,NULL,NULL) == LDAP_SUCCESS)
            return true;
        return false;
    }
}
