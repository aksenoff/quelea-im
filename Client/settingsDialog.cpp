#include "settingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint)
{  
    guestRadio = new QRadioButton(tr("Use guest authorization"));
    guestRadio->setChecked(true);
    clientNameEdit = new QLineEdit;
    clientNameEdit->setEnabled(true);
    serverAddressEdit  = new QLineEdit;
    autoConnectCheckBox = new QCheckBox;
    enableSoundCheckBox = new QCheckBox;

    dbRadio = new QRadioButton(tr("Use database authorization"));
    dbGroupBox = new QGroupBox;
    dbGroupBox->setEnabled(false);
    dbNameEdit = new QLineEdit(dbGroupBox);
    dbPasswordEdit = new QLineEdit(dbGroupBox);
    dbPasswordEdit->setEchoMode(QLineEdit::Password);

    ldapRadio = new QRadioButton(tr("Use LDAP authorization"));
    ldapGroupBox = new QGroupBox;
    ldapGroupBox->setEnabled(false);
    ldapNameEdit = new QLineEdit(ldapGroupBox);
    ldapPasswordEdit = new QLineEdit(ldapGroupBox);
    ldapPasswordEdit->setEchoMode(QLineEdit::Password);

    QLabel* clientNameLabel = new QLabel(tr("&Name")+":");
    QLabel* serverAddressLabel = new QLabel(tr("&Server")+":");
    QLabel* autoConnectLabel = new QLabel(tr("&Connect at startup"));
    QLabel* enableSoundLabel = new QLabel(tr("&Enable sound"));

    QLabel* dbNameLabel = new QLabel(tr("&Username"), dbGroupBox);
    QLabel* dbPasswordLabel = new QLabel(tr("&Password"), dbGroupBox);

    QLabel* ldapNameLabel = new QLabel(tr("&Username"), ldapGroupBox);
    QLabel* ldapPasswordLabel = new QLabel(tr("&Password"), ldapGroupBox);

    dbNameLabel->setBuddy(dbNameEdit);
    dbPasswordLabel->setBuddy(dbPasswordEdit);

    ldapNameLabel->setBuddy(ldapNameEdit);
    ldapPasswordLabel->setBuddy(ldapPasswordEdit);

    clientNameLabel->setBuddy(clientNameEdit);
    serverAddressLabel->setBuddy(serverAddressEdit);
    autoConnectLabel->setBuddy(autoConnectCheckBox);
    enableSoundLabel->setBuddy(enableSoundCheckBox);

    connect(guestRadio, SIGNAL(toggled(bool)),
            clientNameEdit, SLOT(setEnabled(bool)));
    connect(dbRadio, SIGNAL(toggled(bool)),
            dbGroupBox, SLOT(setEnabled(bool)));
    connect(ldapRadio, SIGNAL(toggled(bool)),
            ldapGroupBox, SLOT(setEnabled(bool)));

    okButton = new QPushButton(tr("&Ok"));
    cancelButton = new QPushButton(tr("&Cancel"));

    okButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    // Layout setup
    QVBoxLayout* rightDBLayout = new QVBoxLayout;
    QVBoxLayout* leftDBLayout = new QVBoxLayout;
    QHBoxLayout* dbLayout = new QHBoxLayout(dbGroupBox);
    leftDBLayout->addWidget(dbNameLabel);
    leftDBLayout->addWidget(dbPasswordLabel);
    rightDBLayout->addWidget(dbNameEdit);
    rightDBLayout->addWidget(dbPasswordEdit);
    dbLayout->addLayout(leftDBLayout);
    dbLayout->addLayout(rightDBLayout);

    QVBoxLayout* rightLDAPLayout = new QVBoxLayout;
    QVBoxLayout* leftLDAPLayout = new QVBoxLayout;
    QHBoxLayout* LDAPLayout = new QHBoxLayout(ldapGroupBox);
    leftLDAPLayout->addWidget(ldapNameLabel);
    leftLDAPLayout->addWidget(ldapPasswordLabel);
    rightLDAPLayout->addWidget(ldapNameEdit);
    rightLDAPLayout->addWidget(ldapPasswordEdit);
    LDAPLayout->addLayout(leftLDAPLayout);
    LDAPLayout->addLayout(rightLDAPLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* topLayout = new QHBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    leftLayout->addWidget(clientNameLabel);
    leftLayout->addWidget(serverAddressLabel);
    leftLayout->addWidget(autoConnectCheckBox);
    leftLayout->addWidget(enableSoundCheckBox);
    rightLayout->addWidget(clientNameEdit);
    rightLayout->addWidget(serverAddressEdit);
    rightLayout->addWidget(autoConnectLabel);
    rightLayout->addWidget(enableSoundLabel);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    topLayout->addLayout(leftLayout);
    topLayout->addLayout(rightLayout);
    mainLayout->addWidget(guestRadio);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(dbRadio);
    mainLayout->addWidget(dbGroupBox);
    mainLayout->addWidget(ldapRadio);
    mainLayout->addWidget(ldapGroupBox);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    clientNameEdit->setFocus();
    setWindowTitle(tr("Settings")+" - Quelea");

    // Reading settings to populate the dialog
    QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/settings.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        clientNameEdit->setText(stream.readLine());
        int at = stream.readLine().toInt();
        if (at == GUEST_AUTH)
            guestRadio->setChecked(true);
        if (at == DB_AUTH)
            dbRadio->setChecked(true);
        if (at == LDAP_AUTH)
            ldapRadio->setChecked(true);
        dbNameEdit->setText(stream.readLine());
        dbPasswordEdit->setText(stream.readLine());
        oldHashedPass = dbPasswordEdit->text();
        ldapNameEdit->setText(stream.readLine());
        ldapPasswordEdit->setText(stream.readLine());
        serverAddressEdit->setText(stream.readLine());
        if (stream.readLine() == "1")
            autoConnectCheckBox->setChecked(true);
        if (stream.readLine() == "1")
            enableSoundCheckBox->setChecked(true);
        file.close();
    }
}

// ----------------------------------------------------------------------

QString SettingsDialog::clientName() const
{
    return clientNameEdit->text();
}

// ----------------------------------------------------------------------

QString SettingsDialog::serverAddress() const
{
    return serverAddressEdit->text();
}

// ----------------------------------------------------------------------

bool SettingsDialog::autoConnect() const
{
    return autoConnectCheckBox->isChecked();
}

// ----------------------------------------------------------------------

bool SettingsDialog::enableSound() const
{
    return enableSoundCheckBox->isChecked();
}

// ----------------------------------------------------------------------

int SettingsDialog::authType()
{
    if (dbRadio->isChecked())
        return DB_AUTH;
    else if (ldapRadio->isChecked())
        return LDAP_AUTH;
    else // (guestRadio->isChecked())
        return GUEST_AUTH;
}

// ----------------------------------------------------------------------

QString SettingsDialog::dbName() const
{
    return dbNameEdit->text();
}

// ----------------------------------------------------------------------

QString SettingsDialog::ldapName() const
{
    return ldapNameEdit->text();
}

// ----------------------------------------------------------------------

QString SettingsDialog::ldapPassword()
{
    return ldapPasswordEdit->text();
}

// ----------------------------------------------------------------------

QString SettingsDialog::dbPassword()
{
    if (dbPasswordEdit->text() == oldHashedPass) //we will fix it later
       return oldHashedPass;
    else
       return hash(dbPasswordEdit->text());
}

// ----------------------------------------------------------------------

QString SettingsDialog::hash(QString password)
{
    QByteArray array;
    array.insert(0, password.toUtf8());
    return QString(QCryptographicHash::hash(array, QCryptographicHash::Md5).toBase64());
}
