
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
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(serverLog);
    setLayout(mainLayout);
    setWindowTitle(tr("Quelea Server"));
    setWindowIcon(QIcon::QIcon ("resource.rc"));
    server = new QueleaServer(this);
}

// ----------------------------------------------------------------------

void QueleaServerUI::log(const QString &string)
{
    serverLog->append(string);
}
