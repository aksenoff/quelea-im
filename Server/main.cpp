
#include <QtGui>
#include "QueleaServer.h"

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    QueleaServer server;

    server.show();

    return app.exec();
}
