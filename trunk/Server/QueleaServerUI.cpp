#include <QtNetwork>
#include <QtGui>
#include "QueleaServerUI.h"
#include "../codes.h"

QueleaServerUI::QueleaServerUI(QWidget* pwgt /*=0*/)
    : QWidget(pwgt)
{
    serverLog = new QTextEdit;
    serverLog->setReadOnly(true);
    startStopButton = new QPushButton;
    aboutButton = new QPushButton(tr("О программе..."));
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(serverLog);
    buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(startStopButton);
    buttonsLayout->addWidget(aboutButton);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Quelea Server"));
    setWindowIcon(QIcon::QIcon("resource.rc"));
    startServer();

    connect(startStopButton, SIGNAL(clicked()),
            this, SLOT(stopServer()));
    connect(aboutButton, SIGNAL(clicked()),
            this, SLOT(showAboutBox()));
}

// ----------------------------------------------------------------------

void QueleaServerUI::stopServer()
{
    disconnect(startStopButton, SIGNAL(clicked()),
               this, SLOT(stopServer()));
    connect(startStopButton, SIGNAL(clicked()),
            this, SLOT(startServer()));
    delete server;
    startStopButton->setText(tr("Пуск"));
}

// ----------------------------------------------------------------------

void QueleaServerUI::startServer()
{
    disconnect(startStopButton, SIGNAL(clicked()),
               this, SLOT(startServer()));
    connect(startStopButton, SIGNAL(clicked()),
            this, SLOT(stopServer()));
    server = new QueleaServer(this);
    startStopButton->setText(tr("Стоп"));
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
    aboutBox.setWindowTitle(tr("О программе - Quelea Server"));
    aboutBox.setIconPixmap(QPixmap("/icon.png"));
    aboutBox.setText("<strong>"+tr("Quelea Server 1.0 beta")+"</strong>");
    aboutBox.setInformativeText("<p>" + tr("Используется Qt 4.7.1<br>Распространяется по лизензии "
                                         "<a href=http://www.gnu.org/licenses/gpl/html>GNU GPLv3<a></p>"
                                         "<p><strong>Разработчики:</strong><br>Алексей Аксёнов (aksenoff.a@gmail.com)"
                                         "<br>Роман Сухов (romsuhov@gmail.com)<br>Алексей Топчий (alextopchiy@gmail.com)</p>")
                                + "<p><a href=http://quelea-im.googlecode.com>http://quelea-im.googlecode.com<a></p>"
                                + tr("© Разработчики Quelea, 2011"));
    aboutBox.exec();
}

//-----------------------------------------------------------------------

QueleaServerUI::~QueleaServerUI()
{
    delete server;
}
