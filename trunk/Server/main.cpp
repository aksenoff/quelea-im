#include "QueleaServerUI.h"

int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    QueleaServerUI serverUI;
    serverUI.show();
    return app.exec();
}
