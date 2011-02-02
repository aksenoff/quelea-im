#ifndef QUELEASERVERUI_H
#define QUELEASERVERUI_H

#include <QtGui>
#include "QueleaServer.h"

class QueleaServer;

class QueleaServerUI : public QWidget {
Q_OBJECT
private:
    QTextEdit* serverLog;
    QComboBox* ipAddressesCombo;
    QueleaServer* server;
    QVBoxLayout* mainLayout;
    QHBoxLayout* buttonsLayout;
    QPushButton* startStopButton;
    QPushButton* aboutButton;
public:
    QueleaServerUI(QWidget* pwgt = 0);
    void log(const QString& string) const;
    ~QueleaServerUI();
private slots:
    void showAboutBox();
    void startServer();
    void stopServer();
};

#endif
