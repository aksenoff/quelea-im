#include <QApplication>
#include "QueleaClientUI.h"

int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    QueleaClientUI clientUI;
    clientUI.show();
    return app.exec();
}
