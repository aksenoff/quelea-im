
#include <QtGui>
#include "QueleaServer.h"

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QueleaServer server;

    server.show();

    return app.exec();
}
