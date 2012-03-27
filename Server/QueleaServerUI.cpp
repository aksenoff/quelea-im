#include <QtNetwork>
#include <QtGui>
#include "QueleaServerUI.h"
#include "versionInfo.h"
#include "../codes.h"
#include "settingsDialog.h"

QueleaServerUI::QueleaServerUI(QWidget* pwgt /*=0*/)
    : QWidget(pwgt), currentIp("")
{
    serverLog = new QTextEdit;
    serverLog->setReadOnly(true);
    startStopButton = new QPushButton;
    startStopButton->setText(tr("Start"));
    aboutButton = new QPushButton(tr("About..."));
    settingsButton = new QPushButton(tr("Settings"));
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(serverLog);
    buttonsLayout = new QHBoxLayout;
    ipBox = new QComboBox(this);
    buttonsLayout->addWidget(startStopButton);
    buttonsLayout->addWidget(ipBox);
    buttonsLayout->addWidget(aboutButton);
    buttonsLayout->addWidget(settingsButton);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
    connect(startStopButton, SIGNAL(clicked()),
            this, SLOT(startServer()));
    connect(aboutButton, SIGNAL(clicked()),
            this, SLOT(showAboutBox()));
    connect(settingsButton, SIGNAL(clicked()),
                this, SLOT(openSettingDialog()));

    setWindowTitle(tr("Quelea Server"));
    setWindowIcon(QIcon("resource.rc"));//!
    resize(450, 300);
    useDB = false;
    useLDAP = false;
    ldath = NULL;
    db = new Database(this);

    //Settings reading
    const QString localSettings = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/Quelea Server/settings.dat";
    const QString globalSettings = "settings.dat";

    //Creating "data" directory if this does not exist to QDesktopServices::DataLocation will be right on Linux
    QDir localDataDir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.local/share");
    if (QDesktopServices::storageLocation(QDesktopServices::DataLocation) == localDataDir.absolutePath() + "/data//" && !localDataDir.exists("data"))
        localDataDir.mkdir("data");

    if (QFile::exists(localSettings))
        readSettings(localSettings);
    else
        if (QFile::exists(globalSettings))
        {
            readSettings(globalSettings);
            writeSettings(false);
        }
        else
            openSettingDialog();


    if (useDB)
        db->openDB(dbFileName);
    if (useLDAP){
        if (useAdLdap)
            ldath = new LdapAuth(ldapHost,ldapDomain);
        else
            ldath = new LdapAuth(ldapHost,ldapPort,ldapDomain,ldapAdmin,ldapAdminPw);
    }

       // сделать создание при настройке!



    populateIpBox();
    //ldath = new LdapAuth("192.168.1.12","quelea.local");
    //ldath = new LdapAuth("192.168.1.6",389,"dc=debian,dc=local","cn=admin,dc=debian,dc=local","vesna");
   // if(ldath->authorize("romsuhov","123"))
  //      qDebug()<<"True!";
   // delete ldath;
    startServer();
}

// ----------------------------------------------------------------------

// function returns true if any non-localhost interfaces found and false otherwise
bool QueleaServerUI::populateIpBox()
{

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    QStringList ipStringList;
    // scanning through interfaces to find all non-localhost ones
    for(int i = 0; i < ipAddressesList.size(); ++i)
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
            ipStringList << ipAddressesList.at(i).toString();
    // if nothing found, using localhost
    if(ipStringList.isEmpty())
    {
        ipBox->clear();
        ipBox->addItem(QHostAddress(QHostAddress::LocalHost).toString());
        return false;
    }
    else // populating combobox to let user decide
    {
        ipBox->clear();
        ipStringList << QHostAddress(QHostAddress::LocalHost).toString();
        ipBox->addItems(ipStringList);
        if(!currentIp.isEmpty())
            ipBox->setCurrentIndex(ipStringList.indexOf(currentIp));
        return true;
    }
}

// ----------------------------------------------------------------------

void QueleaServerUI::stopServer()
{
    disconnect(startStopButton, SIGNAL(clicked()),
               this, SLOT(stopServer()));
    connect(startStopButton, SIGNAL(clicked()),
            this, SLOT(startServer()));
    delete server;
    server = 0;
    startStopButton->setText(tr("Start"));
    ipBox->setEnabled(populateIpBox()); // true if non-localhost interfaces present
}

// ----------------------------------------------------------------------

void QueleaServerUI::startServer()
{
    currentIp = ipBox->currentText();
    server = new QueleaServer(currentIp, this, db, ldath);
    if(server->isListening())
    {
        disconnect(startStopButton, SIGNAL(clicked()),
                   this, SLOT(startServer()));
        connect(startStopButton, SIGNAL(clicked()),
                this, SLOT(stopServer()));
        startStopButton->setText(tr("Stop"));
        ipBox->setEnabled(false);
    }
    else
    {
        delete server;
        server = 0;
    }
}

// ----------------------------------------------------------------------

void QueleaServerUI::log(const QString &event) const
{
    serverLog->append("[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "]" + " " + event);
}

//---------------------------------------------------------

void QueleaServerUI::readSettings(QString settingsPath)
{
    QFile file(settingsPath);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        useDB = stream.readLine().toInt();
        dbFileName = stream.readLine();
        useLDAP = stream.readLine().toInt();
        useAdLdap = stream.readLine().toInt();
        ldapHost = stream.readLine();
        ldapPort = stream.readLine().toInt();
        ldapDomain = stream.readLine();
        ldapAdmin = stream.readLine();
        ldapAdminPw = stream.readLine();
        file.close();
    }
}

void QueleaServerUI::writeSettings(bool writeGlobal)
{
    if (writeGlobal)
    {
        QFile globalFile("settings.dat");
        if (globalFile.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&globalFile);
            stream << useDB << '\n'
                   << dbFileName << '\n'
                   << useLDAP << '\n'
                   << useAdLdap << '\n'
                   << ldapHost << '\n'
                   << ldapPort << '\n'
                   << ldapDomain << '\n'
                   << ldapAdmin << '\n'
                   << ldapAdminPw
                   << flush;
            globalFile.close();
        }
    }

    QDir localDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    if (!localDir.exists(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/Quelea"))
        localDir.mkdir("Quelea Server");

    QFile localFile(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/Quelea Server/settings.dat");
    if (localFile.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&localFile);
        stream << useDB << '\n'
               << dbFileName << '\n'
               << useLDAP << '\n'
               << useAdLdap << '\n'
               << ldapHost << '\n'
               << ldapPort << '\n'
               << ldapDomain << '\n'
               << ldapAdmin << '\n'
               << ldapAdminPw
               << flush;
        localFile.close();
    }
}
//----------------------------------------------------------
void QueleaServerUI::openSettingDialog()
{
    SettingsDialog* settingsDialog = new SettingsDialog(db);
    if (settingsDialog->exec() == QDialog::Accepted){
        //Database settings
        useDB = settingsDialog->useDB();
        dbFileName = settingsDialog->dbFileName();
        if(!useDB && db->isOpened())
            db->closeDB();
         //LDAP settings
        useLDAP = settingsDialog->useLDAP();
        useAdLdap = settingsDialog->useLdapAd();
        if (useAdLdap){
            settingsDialog->getLdapAdSettings(ldapHost,ldapDomain);
            if (!ldath)
                ldath = new LdapAuth(ldapHost,ldapDomain);
            else
                ldath->setSettings(ldapHost,ldapDomain);
        }
        else {
            settingsDialog->getLdapNoAdSettings(ldapHost,ldapPort,ldapDomain,ldapAdmin,ldapAdminPw);
            if (!ldath)
               ldath = new LdapAuth(ldapHost,ldapPort,ldapDomain,ldapAdmin,ldapAdminPw);
            else
                ldath->setSettings(ldapHost,ldapPort,ldapDomain,ldapAdmin,ldapAdminPw);
        }

        writeSettings(true);
    }
    delete settingsDialog;
}
//---------------------------------------------------------

void QueleaServerUI::showAboutBox()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle(tr("About") + " - Quelea");
    aboutBox.setIconPixmap(QPixmap(":/icon.png"));
    aboutBox.setText("<strong>Quelea Server " + APPLICATION_VERSION + "</strong>");
    aboutBox.setInformativeText("<p>" + tr("Compiled with") + " " + "Qt " + LIB_QT_VERSION + " " + tr("on") + " " + BUILD_DATE +
                                "<br>" + tr("Distributed under") + " <a href=http://www.gnu.org/licenses/gpl/html>GNU GPLv3<a></p>"
                                "<p><strong>" + tr("Developers") + ":</strong><br>" + tr("Alexey Aksenov") + " (aksenoff.a@gmail.com)"
                                "<br>" + tr("Roman Suhov") + " (romsuhov@gmail.com)<br>" + tr("Alexey Topchiy") + " (alextopchiy@gmail.com)</p>"
                                + "<p><a href=http://quelea-im.googlecode.com>http://quelea-im.googlecode.com<a></p>"
                                + tr("В©") + " " + tr("Developers of") + " Quelea, " + COPYRIGHT_YEAR);
    aboutBox.exec();
}

//-----------------------------------------------------------------------

QueleaServerUI::~QueleaServerUI()
{
    if(server) delete server;
    if(db) delete db;
    if(ldath) delete ldath;
}
