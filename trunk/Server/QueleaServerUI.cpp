#include <QtNetwork>
#include <QtGui>
#include "QueleaServerUI.h"
#include "../codes.h"

QueleaServerUI::QueleaServerUI(QWidget* pwgt /*=0*/)
    : QWidget(pwgt), currentIp("")
{
    serverLog = new QTextEdit;
    serverLog->setReadOnly(true);
    startStopButton = new QPushButton;
    startStopButton->setText(tr("Start"));
    aboutButton = new QPushButton(tr("About"));
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(serverLog);
    buttonsLayout = new QHBoxLayout;
    ipBox = new QComboBox(this);
    buttonsLayout->addWidget(startStopButton);
    buttonsLayout->addWidget(ipBox);
    buttonsLayout->addWidget(aboutButton);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
    connect(startStopButton, SIGNAL(clicked()),
            this, SLOT(startServer()));
    connect(aboutButton, SIGNAL(clicked()),
            this, SLOT(showAboutBox()));

    setWindowTitle(tr("Quelea Server"));
    setWindowIcon(QIcon::QIcon("resource.rc"));
    resize(450, 300);
    populateIpBox();
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
    server = new QueleaServer(currentIp, this);
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
    serverLog->append(event);
}

//---------------------------------------------------------

void QueleaServerUI::showAboutBox()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle(tr("About")+" - Quelea");
    aboutBox.setIconPixmap(QPixmap(":/icon.png"));
    aboutBox.setText("<strong>"+tr("Quelea Sever 1.0 beta 2")+"</strong>");
    aboutBox.setInformativeText("<p>" + tr("Used")+" "+"Qt 4.7.0 <br>"+tr("Distributed under license")
                                         +" <a href=http://www.gnu.org/licenses/gpl/html>GNU GPLv3<a></p>"
                                "<p><strong>"+tr("Developers")+":</strong><br>"+tr("Alexey Aksenov")+" (aksenoff.a@gmail.com)"
                                "<br>"+tr("Roman Suhov")+" (romsuhov@gmail.com)<br>"+tr("Alexey Topchiy")+" (alextopchiy@gmail.com)</p>"
                                + "<p><a href=http://quelea-im.googlecode.com>http://quelea-im.googlecode.com<a></p>"
                                + tr("©")+" "+tr("Developers of")+" Quelea, 2011");
    aboutBox.exec();
}

//-----------------------------------------------------------------------

QueleaServerUI::~QueleaServerUI()
{
    if(server) delete server;
}
