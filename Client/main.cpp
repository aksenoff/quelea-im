

#include <QApplication>
#include "QueleaClient.h"

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    QueleaClient client;

    client.show();

    return app.exec();
}
