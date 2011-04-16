#include <QApplication>
#include "QueleaClientUI.h"
#include "singleapplication\src\singleapplication.h"

int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    QueleaClientUI clientUI;

   // SingleApplication instance("Quelea", &app);
    //if(instance.isRunning())
     //   if(instance.sendMessage("!"))
      //      return 0;

    // QObject::connect(&instance, SIGNAL(messageReceived(const QString&)),
                     //&clientUI, SLOT(slotShow()));

    clientUI.show();
    return app.exec();
}
