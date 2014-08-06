#include <QtNetwork>
#include "QueleaServerControl.h"
#include "versionInfo.h"
#include "../codes.h"

QueleaServerControl::QueleaServerControl()

{

}

// ----------------------------------------------------------------------

void QueleaServerControl::stopServer()
{
    delete server;
    server = 0;
}

// ----------------------------------------------------------------------

void QueleaServerControl::startServer()
{
    server = new QueleaServer(QHostAddress::Any, this);
    if(!server->isListening()){
        delete server;
        server = 0;
        QCoreApplication::exit(-1);
    }
}

// ----------------------------------------------------------------------

void QueleaServerControl::log(const QString &event) const
{
    QTextStream logStream(stdout);
    logStream << "[" + QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + "]" + " " + event;
}

//-----------------------------------------------------------------------

QueleaServerControl::~QueleaServerControl()
{
    if(server) delete server;
}
