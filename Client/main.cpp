#include <QApplication>
#include "QueleaClientUI.h"
#include "singleapplication/src/singleapplication.h"
#define DEBUG

int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    QTranslator translator;
    translator.load("translations/QueleaClient_" + QLocale::system().name());
    app.installTranslator(&translator);

    QueleaClientUI clientUI;

    SingleApplication instance("Quelea", &app);

    #ifndef DEBUG
    if(instance.isRunning())
        if(instance.sendMessage("!"))
            return 0;
    #endif

    QObject::connect(&instance, SIGNAL(messageReceived(const QString&)),
                     &clientUI, SLOT(slotShow()));

    clientUI.show();
    return app.exec();
}
