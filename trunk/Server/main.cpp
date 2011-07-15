#include "QueleaServerUI.h"

int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load("translations/QueleaSever_" + QLocale::system().name());
    app.installTranslator(&translator);

    QueleaServerUI serverUI;
    serverUI.show();
    return app.exec();
}
