#include <QtGui>
#include "settingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint)
{
    clientNameEdit = new QLineEdit;
    serverAddressEdit  = new QLineEdit;
    autoconnectCheckBox = new QCheckBox;
    enableSoundCheckBox = new QCheckBox;

    clientNameLabel = new QLabel(tr("&Имя:"));
    serverAddressLabel = new QLabel(tr("&Сервер:"));
    autoConnectLabel = new QLabel(tr("&Подключаться при запуске"));
    enableSoundLabel = new QLabel(tr("&Включить звуки"));

    clientNameLabel->setBuddy(clientNameEdit);
    serverAddressLabel ->setBuddy(serverAddressEdit);
    autoConnectLabel->setBuddy(autoconnectCheckBox);
    enableSoundLabel->setBuddy(enableSoundCheckBox);

    okButton = new QPushButton(tr("&OK"));
    cancelButton = new QPushButton(tr("&Отмена"));

    okButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    //Layout setup
    mainLayout = new QVBoxLayout;
    topLayout = new QHBoxLayout;
    rightLayout = new QVBoxLayout;
    leftLayout = new QVBoxLayout;
    buttonLayout = new QHBoxLayout;
    leftLayout->addWidget(clientNameLabel);
    leftLayout->addWidget(serverAddressLabel);
    leftLayout->addWidget(autoconnectCheckBox);
    leftLayout->addWidget(enableSoundCheckBox);
    rightLayout->addWidget(clientNameEdit);
    rightLayout->addWidget(serverAddressEdit);
    rightLayout->addWidget(autoConnectLabel);
    rightLayout->addWidget(enableSoundLabel);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    topLayout->addLayout(leftLayout);
    topLayout->addLayout(rightLayout);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    setWindowTitle(tr("Настройки - Quelea"));

    QFile file("settings.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        clientNameEdit->setText(stream.readLine());
        serverAddressEdit->setText(stream.readLine());
        if (stream.readLine()=="1")
            autoconnectCheckBox->setChecked(true);
        if (stream.readLine()=="1")
            enableSoundCheckBox->setChecked(true);
        file.close();
    }
}

SettingsDialog::~SettingsDialog()
{
    delete clientNameEdit;
    delete serverAddressEdit;
    delete autoconnectCheckBox;
    delete enableSoundCheckBox;
    delete clientNameLabel;
    delete serverAddressLabel;
    delete autoConnectLabel;
    delete enableSoundLabel;
    delete okButton;
    delete cancelButton;
    delete mainLayout;
    delete topLayout;
    delete rightLayout;
    delete leftLayout;
    delete buttonLayout;
}

// ----------------------------------------------------------------------

QString SettingsDialog::clientName() const
{
    return clientNameEdit->text();
}

// ----------------------------------------------------------------------

QString SettingsDialog::serverAddress() const
{
    return serverAddressEdit->text();
}

// ----------------------------------------------------------------------

bool SettingsDialog::autoconnect() const
{
return autoconnectCheckBox->isChecked();
}

// ----------------------------------------------------------------------

bool SettingsDialog::enableSound() const
{
return enableSoundCheckBox->isChecked();
}
