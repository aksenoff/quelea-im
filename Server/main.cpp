#include "QueleaServerControl.h"
#include <iostream>

int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QCoreApplication app(argc, argv);

    QTranslator translator;
    translator.load("translations/QueleaSever_" + QLocale::system().name());
    app.installTranslator(&translator);
    QueleaServerControl serverControl;
    serverControl.startServer();
    return app.exec();
}
