
#include <QtNetwork>
#include <QtGui>
#include "QueleaServerUI.h"
#include "../codes.h"

// ----------------------------------------------------------------------

QueleaServerUI::QueleaServerUI(QWidget* pwgt /*=0*/)
    : QWidget(pwgt)
{
    serverLog = new QTextEdit;
    serverLog->setReadOnly(true);
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(serverLog);
    setLayout(mainLayout);
    setWindowTitle(tr("Quelea Server"));
    setWindowIcon(QIcon::QIcon ("resource.rc"));
    server = new QueleaServer(this);
}

// ----------------------------------------------------------------------

void QueleaServerUI::log(const QString &event) const
{
    serverLog->append(event);
}

//-----------------------------------------------------------------------

QueleaServerUI::~QueleaServerUI()
{
    delete server;
}
