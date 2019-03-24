#include "application.h"
#include "QueleaClientUI.h"

int main(int argc, char** argv)
{
    Application app(argc, argv);

    if(!app.lock())
        return 1;

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    QTranslator translator;
    translator.load("translations/QueleaClient_" + QLocale::system().name());
    app.installTranslator(&translator);

    QueleaClientUI clientUI;

    clientUI.show();
    return app.exec();
}
