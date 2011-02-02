#include <QtGui>
#include "settingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint)
{
    clientNameEdit = new QLineEdit;
    serverAddressEdit  = new QLineEdit;
    autoConnectCheckBox = new QCheckBox;
    enableSoundCheckBox = new QCheckBox;

    QLabel* clientNameLabel = new QLabel(tr("&Имя:"));
    QLabel* serverAddressLabel = new QLabel(tr("&Сервер:"));
    QLabel* autoConnectLabel = new QLabel(tr("&Подключаться при запуске"));
    QLabel* enableSoundLabel = new QLabel(tr("&Включить звуки"));

    clientNameLabel->setBuddy(clientNameEdit);
    serverAddressLabel ->setBuddy(serverAddressEdit);
    autoConnectLabel->setBuddy(autoConnectCheckBox);
    enableSoundLabel->setBuddy(enableSoundCheckBox);

    okButton = new QPushButton(tr("&OK"));
    cancelButton = new QPushButton(tr("&Отмена"));

    okButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    // Layout setup
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* topLayout = new QHBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    leftLayout->addWidget(clientNameLabel);
    leftLayout->addWidget(serverAddressLabel);
    leftLayout->addWidget(autoConnectCheckBox);
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

    // Reading settings to populate the dialog
    QFile file("settings.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        clientNameEdit->setText(stream.readLine());
        serverAddressEdit->setText(stream.readLine());
        if (stream.readLine() == "1")
            autoConnectCheckBox->setChecked(true);
        if (stream.readLine() == "1")
            enableSoundCheckBox->setChecked(true);
        file.close();
    }
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

bool SettingsDialog::autoConnect() const
{
    return autoConnectCheckBox->isChecked();
}

// ----------------------------------------------------------------------

bool SettingsDialog::enableSound() const
{
    return enableSoundCheckBox->isChecked();
}
