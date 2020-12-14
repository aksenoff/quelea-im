#include <QtGui>
#include "settingsDialog.h"
#include "database.h"
#include "ldapauth.h"

SettingsDialog::SettingsDialog(Database* DB, QWidget* pwgt/*= 0*/)
    : QDialog(pwgt), db(DB)
{
    tabWidget = new QTabWidget;
    QWidget* authWidget = new QWidget;
    dbGroupBox = new QGroupBox(tr("Use database"));
    dbGroupBox->setCheckable(true);
    dbGroupBox->setChecked(false);
    dbGroupBox->resize(411,150);
    connect(dbGroupBox, SIGNAL(toggled(bool)),
            this, SLOT(dbGroupBoxToggled(bool)));

    dbTooltip = new QLabel(tr("Load DB file or create new one"));
    dbState = new QLabel(tr("File is not selected"));
    dbPathEdit = new QLineEdit;
    dbPathEdit->setReadOnly(true);
    dbLoadButton = new QPushButton(tr("Load")+"...");
    dbCreateButton = new QPushButton(tr("Create")+"...");
    dbEditButton = new QPushButton(tr("Edit")+"...");

    dbLoadButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    dbCreateButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    dbEditButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(dbLoadButton, SIGNAL(clicked()),
            this, SLOT(loadDB()));
    connect(dbCreateButton, SIGNAL(clicked()),
            this, SLOT(createDB()));
    connect(dbEditButton, SIGNAL(clicked()),
            this, SLOT(openDBEditor()));

    QHBoxLayout* dbButtonLayout = new QHBoxLayout;
    dbButtonLayout->addSpacing(200);
    dbButtonLayout->addWidget(dbLoadButton);
    dbButtonLayout->addWidget(dbCreateButton);
    dbButtonLayout->addWidget(dbEditButton);

    QVBoxLayout* dbLayout = new QVBoxLayout(dbGroupBox);
    dbLayout->addWidget(dbPathEdit);
    dbLayout->addWidget(dbState);
    dbLayout->addWidget(dbTooltip);
    dbLayout->addLayout(dbButtonLayout);

    ldapGroupBox = new QGroupBox(tr("Use LDAP"));
    ldapGroupBox->setCheckable(true);
    ldapGroupBox->setChecked(false);

    adRadio = new QRadioButton(tr("Active Directory"));
    adRadio->setChecked(true);
    otherRadio = new QRadioButton(tr("Other LDAP server"));
    ldapServerEdit = new QLineEdit;
    ldapPortEdit = new QLineEdit;
    ldapPortEdit->setText("389");
    ldapDomainEdit = new QLineEdit;
    ldapAdminDnEdit = new QLineEdit;
    ldapAdminPwdEdit = new QLineEdit;
    ldapAdminPwdEdit ->setEchoMode(QLineEdit::Password);
    ldapState = new QLabel(tr("No connection"));

    QLabel* ldapServerLabel = new QLabel(tr("LDAP server"));
    QLabel* ldapPortLabel = new QLabel(tr("port"));
    QLabel* ldapDomainLabel = new QLabel(tr("domain"));
    QLabel* ldapAdminDnLabel = new QLabel(tr("admin DN"));
    QLabel* ldapAdminPwdLabel = new QLabel(tr("admin password"));

    ldapTestButton = new QPushButton(tr("Test connection"));
    ldapTestButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(ldapTestButton,SIGNAL(clicked()),
            this, SLOT(ldapTest()));

    connect(adRadio, SIGNAL(toggled(bool)),
            this, SLOT(ldapTypeToggled(bool)));
    ldapTypeToggled(true);

    QVBoxLayout* ldapLeftLayout = new QVBoxLayout;
    ldapLeftLayout->addWidget(adRadio);
    ldapLeftLayout->addWidget(ldapServerLabel);
    ldapLeftLayout->addWidget(ldapServerEdit);
    ldapLeftLayout->addWidget(ldapDomainLabel);
    ldapLeftLayout->addWidget(ldapDomainEdit);
    ldapLeftLayout->addWidget(ldapTestButton);
    ldapLeftLayout->addWidget(ldapState);

    QVBoxLayout* ldapRightLayout = new QVBoxLayout;
    ldapRightLayout->addWidget(otherRadio);
    ldapRightLayout->addWidget(ldapPortLabel);
    ldapRightLayout->addWidget(ldapPortEdit);
    ldapRightLayout->addWidget(ldapAdminDnLabel);
    ldapRightLayout->addWidget(ldapAdminDnEdit);
    ldapRightLayout->addWidget(ldapAdminPwdLabel);
    ldapRightLayout->addWidget(ldapAdminPwdEdit);

    QHBoxLayout* ldapLayout = new QHBoxLayout(ldapGroupBox);
    ldapLayout->addLayout(ldapLeftLayout);
    ldapLayout->addLayout(ldapRightLayout);


    QVBoxLayout* authLayout = new QVBoxLayout;
    authLayout->addWidget(dbGroupBox);
    authLayout->addWidget(ldapGroupBox);
    authWidget->setLayout(authLayout);

    tabWidget->addTab(authWidget,tr("Authorization"));

    okButton = new QPushButton(tr("&Ok"));
    cancelButton = new QPushButton(tr("&Cancel"));

    okButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    // Layout setup
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    resize(451,338);
    setWindowTitle(tr("Settings")+" - Quelea Server");


    // Reading settings to populate the dialog
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Quelea Server/settings.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        if (stream.readLine() == "1")
            dbGroupBox->setChecked(true);
        dbPathEdit->setText(stream.readLine());

        if (stream.readLine() == "1")
             ldapGroupBox->setChecked(true);
        if (stream.readLine() == "1")
            adRadio->setChecked(true);
        else
            otherRadio->setChecked(true);
        ldapServerEdit->setText(stream.readLine());
        ldapPortEdit->setText(stream.readLine());
        ldapDomainEdit->setText(stream.readLine());
        ldapAdminDnEdit->setText(stream.readLine());
        ldapAdminPwdEdit->setText(stream.readLine());
        file.close();
     }

    if (db->isOpened())
        dbState->setText("<FONT COLOR=GREEN>"+tr("Database opened")+"</FONT>");
    else
        if (!dbPathEdit->text().isEmpty())
            dbState->setText(tr("Database is not used"));

    if (adRadio->isChecked())
        ldapPortEdit->setText("389");

}

// ----------------------------------------------------------------------

void SettingsDialog::loadDB()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open database"),
                                                    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                    tr("Quelea Database Files (*.qdb)"));
        if (!fileName.isEmpty())
            switch (db->openDB(fileName)) {
                case DB_OPEN_OK :
                dbState->setText("<FONT COLOR=GREEN>"+tr("Database opened")+"</FONT>");
                dbPathEdit->setText(fileName);
                break;

                case DB_OPEN_ERROR :
                dbState->setText("<FONT COLOR=RED>"+tr("Database open error: file error")+"</FONT>");
                break;

                case DB_VERSION_ERROR :
                dbState->setText("<FONT COLOR=RED>"+tr("Database open error: uncompatible DB version")+"</FONT>");
                break;
        }


}

void SettingsDialog::loadDB(QString fileName)
{
            switch (db->openDB(fileName)) {
                case DB_OPEN_OK :
                dbState->setText("<FONT COLOR=GREEN>"+tr("Database opened")+"</FONT>");
                dbPathEdit->setText(fileName);
                break;

                case DB_OPEN_ERROR :
                dbState->setText("<FONT COLOR=RED>"+tr("Database open error: file error")+"</FONT>");
                break;

                case DB_VERSION_ERROR :
                dbState->setText("<FONT COLOR=RED>"+tr("Database open error: uncompatible DB version")+"</FONT>");
                break;
        }


}

// ----------------------------------------------------------------------

void SettingsDialog::createDB()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Create new database"),
                                                    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                    tr("Quelea Database Files (*.qdb)"));
    if(db->createDB(fileName)) {
        dbState->setText("<FONT COLOR=GREEN>"+tr("Database created and opened")+"</FONT>");
        dbPathEdit->setText(fileName);
    }
    else
        dbState->setText("<FONT COLOR=RED>"+tr("Database creation error")+"</FONT>");
}

// ----------------------------------------------------------------------
void SettingsDialog::openDBEditor()
{
    dbEdit = new DBEditor(db);
    dbEdit->exec();
    delete dbEdit;

}
// ----------------------------------------------------------------------

void SettingsDialog::dbGroupBoxToggled(bool enabled)
{
    if(enabled) {
        if(!dbPathEdit->text().isEmpty() && !db->isOpened())
            loadDB(dbPathEdit->text());
        if (db->isOpened())
            dbState->setText("<FONT COLOR=GREEN>"+tr("Database opened")+"</FONT>");
    }
    else
        dbState->setText(tr("Database is not used"));

}

// ----------------------------------------------------------------------
void SettingsDialog::ldapTypeToggled(bool ad)
{
    if(ad){
        ldapPortEdit->setEnabled(false);
        ldapAdminDnEdit->setEnabled(false);
        ldapAdminPwdEdit->setEnabled(false);
    }
    else{
        ldapPortEdit->setEnabled(true);
        ldapAdminDnEdit->setEnabled(true);
        ldapAdminPwdEdit->setEnabled(true);
    }

}
// ----------------------------------------------------------------------

bool SettingsDialog::useDB() const
{
    return dbGroupBox->isChecked();
}
// ----------------------------------------------------------------------
QString SettingsDialog::dbFileName() const
{
    return dbPathEdit->text();
}
// ----------------------------------------------------------------------
bool SettingsDialog::useLDAP()
{
    return ldapGroupBox->isChecked();
}
// ----------------------------------------------------------------------
bool SettingsDialog::useLdapAd()
{
    return adRadio->isChecked();
}
// ----------------------------------------------------------------------
void SettingsDialog::getLdapAdSettings(QString& h, QString& d)
{
    h = ldapServerEdit->text();
    d = ldapDomainEdit->text();
}
// ----------------------------------------------------------------------
void SettingsDialog::getLdapNoAdSettings(QString &h, int &p, QString &d, QString &admin, QString &pwd)
{
    h = ldapServerEdit->text();
    p = ldapPortEdit->text().toInt();
    d = ldapDomainEdit->text();
    admin = ldapAdminDnEdit->text();
    pwd = ldapAdminPwdEdit->text();
}
//-----------------------------------------------------------------------
void SettingsDialog::ldapTest()
{
    if (adRadio->isChecked()){
        if(LdapAuth::testConnection(ldapServerEdit->text()))
           ldapState->setText("<FONT COLOR=GREEN>"+tr("LDAP connection is OK")+"</FONT>");
        else
           ldapState->setText("<FONT COLOR=RED>"+tr("LDAP connection error")+"</FONT>");
    }
    else {
        if(LdapAuth::testConnection(ldapServerEdit->text(),ldapPortEdit->text().toInt(),
                                    ldapAdminDnEdit->text(),ldapAdminPwdEdit->text()))
            ldapState->setText("<FONT COLOR=GREEN>"+tr("LDAP connection is OK")+"</FONT>");
        else
            ldapState->setText("<FONT COLOR=RED>"+tr("LDAP connection error")+"</FONT>");
    }
}
