

#include <QApplication>
#include "QueleaClient.h"

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QueleaClient client;

    client.show();

    return app.exec();
}
