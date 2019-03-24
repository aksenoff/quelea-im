#include <QApplication>
#include "QueleaClientUI.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    QTranslator translator;
    translator.load("translations/QueleaClient_" + QLocale::system().name());
    app.installTranslator(&translator);

    QueleaClientUI clientUI;

    clientUI.show();
    return app.exec();
}
