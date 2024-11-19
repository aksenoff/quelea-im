#include "application.h"
#include "QueleaClientUI.h"

int main(int argc, char** argv)
{
    Application app(argc, argv);

    if(!app.lock())
        return 1;

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    QTranslator translator;
    if(!translator.load(QCoreApplication::applicationDirPath() + "/translations/QueleaClient_" + QLocale::system().name()))
        qWarning() << "Translation file not found for locale:" << QLocale::system().name() << "in" << QCoreApplication::applicationDirPath() + "/translations/";
    app.installTranslator(&translator);

    QueleaClientUI clientUI;

    clientUI.show();
    return app.exec();
}
