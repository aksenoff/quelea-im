#include <QtGui>
#include "settingsDialog.h"

// ----------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    clientNameLe = new QLineEdit;
    serverAdrLe  = new QLineEdit;
    autoconnectCheckBox = new QCheckBox;
    enableSoundCheckBox = new QCheckBox;

    QLabel* lblclientName    = new QLabel(tr("&Имя:"));
    QLabel* lblserverAdr     = new QLabel(tr("&Сервер:"));
    QLabel* lblautoconnect    = new QLabel(tr("&Подключаться при запуске"));
    QLabel* lblenableSound    = new QLabel(tr("&Включить звуки"));

    lblclientName->setBuddy(clientNameLe);
    lblserverAdr ->setBuddy(serverAdrLe);
    lblautoconnect->setBuddy(autoconnectCheckBox);
    lblenableSound->setBuddy(enableSoundCheckBox);

    QPushButton* pcmdOk     = new QPushButton(tr("&OK"));
    QPushButton* pcmdCancel = new QPushButton(tr("&Отмена"));

    pcmdOk->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    pcmdCancel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(pcmdOk, SIGNAL(clicked()), SLOT(accept()));
    connect(pcmdCancel, SIGNAL(clicked()), SLOT(reject()));

    //Layout setup
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* topLayout = new QHBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    leftLayout->addWidget(lblclientName);
    leftLayout->addWidget(lblserverAdr);
    leftLayout->addWidget(autoconnectCheckBox);
    leftLayout->addWidget(enableSoundCheckBox);
    rightLayout->addWidget(clientNameLe);
    rightLayout->addWidget(serverAdrLe);
    rightLayout->addWidget(lblautoconnect);
    rightLayout->addWidget(lblenableSound);
    buttonLayout->addWidget(pcmdOk);
    buttonLayout->addWidget(pcmdCancel);
    topLayout->addLayout(leftLayout);
    topLayout->addLayout(rightLayout);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Настройки - Quelea"));

    QFile file("set.dat");

    if (file.open(QIODevice::ReadOnly)){
        QTextStream stream (&file);
        clientNameLe->setText(stream.readLine());
        serverAdrLe->setText(stream.readLine());
        if (stream.readLine()=="1")
            autoconnectCheckBox->setChecked(true);
        if (stream.readLine()=="1")
            enableSoundCheckBox->setChecked(true);
        file.close();
}
}

// ----------------------------------------------------------------------
QString SettingsDialog::clientName() const
{
    return clientNameLe->text();
}

// ----------------------------------------------------------------------
QString SettingsDialog::serverAdr() const
{
    return serverAdrLe->text();
}

bool SettingsDialog::autoconnect() const
{
return autoconnectCheckBox->isChecked();
}

bool SettingsDialog::enableSound() const
{
return enableSoundCheckBox->isChecked();
}
