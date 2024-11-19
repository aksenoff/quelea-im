#include "QueleaServerUI.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QTranslator translator;
    if(!translator.load(QCoreApplication::applicationDirPath() + "/translations/QueleaSever_" + QLocale::system().name()))
        qWarning() << "Translation file not found for locale:" << QLocale::system().name() << "in" << QCoreApplication::applicationDirPath() + "/translations/";
    app.installTranslator(&translator);

    QueleaServerUI serverUI;
    serverUI.show();
    return app.exec();
}
