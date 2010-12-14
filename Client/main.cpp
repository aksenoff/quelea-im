

#include <QApplication>
#include "QueleaClient.h"
#include"systemTray.h"


// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    QueleaClient client;
    QApplication::setQuitOnLastWindowClosed(false);
    client.show();
    SystemTray st(0,&client);
    return app.exec();
}
