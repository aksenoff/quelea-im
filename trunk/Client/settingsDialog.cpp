#include <QtGui>
#include "settingsDialog.h"

// ----------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    clientNameLe = new QLineEdit;
    serverAdrLe  = new QLineEdit;
    autoconnectCheckBox = new QCheckBox;

    QLabel* lblclientName    = new QLabel(tr("&Name"));
    QLabel* lblserverAdr     = new QLabel(tr("&Server"));
    QLabel* lblautoconnect    = new QLabel(tr("&Autoconnect"));

    lblclientName->setBuddy(clientNameLe);
    lblserverAdr ->setBuddy(serverAdrLe);
    lblautoconnect->setBuddy(autoconnectCheckBox);

    QPushButton* pcmdOk     = new QPushButton(tr("&Ok"));
    QPushButton* pcmdCancel = new QPushButton(tr("&Cancel"));

    connect(pcmdOk, SIGNAL(clicked()), SLOT(accept()));
    connect(pcmdCancel, SIGNAL(clicked()), SLOT(reject()));

    //Layout setup
    QGridLayout* ptopLayout = new QGridLayout;
    ptopLayout->addWidget(lblclientName, 0, 0);
    ptopLayout->addWidget(lblserverAdr, 1, 0);
    ptopLayout->addWidget(lblautoconnect, 2, 0);
    ptopLayout->addWidget(clientNameLe, 0, 1);
    ptopLayout->addWidget(serverAdrLe, 1, 1);
    ptopLayout->addWidget(autoconnectCheckBox, 2, 1);
    ptopLayout->addWidget(pcmdOk, 3,0);
    ptopLayout->addWidget(pcmdCancel, 3, 1);
    setLayout(ptopLayout);

    QFile file("set.dat");

    if (file.open(QIODevice::ReadOnly)){
        QTextStream stream (&file);
        clientNameLe->setText(stream.readLine());
        serverAdrLe->setText(stream.readLine());
        if (stream.readLine()=="1")
        autoconnectCheckBox->setChecked(true);
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
