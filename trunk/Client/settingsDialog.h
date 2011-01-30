#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>

class SettingsDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit* clientNameEdit;
    QLineEdit* serverAddressEdit;
    QCheckBox* autoconnectCheckBox;
    QCheckBox* enableSoundCheckBox;
    QLabel* clientNameLabel;
    QLabel* serverAddressLabel;
    QLabel* autoConnectLabel;
    QLabel* enableSoundLabel;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QVBoxLayout* mainLayout;
    QHBoxLayout* topLayout;
    QVBoxLayout* rightLayout;
    QVBoxLayout* leftLayout;
    QHBoxLayout* buttonLayout;
public:
    SettingsDialog(QWidget* pwgt = 0);
    QString clientName() const;
    QString serverAddress() const;
    bool autoconnect() const;
    bool enableSound() const;
};

#endif
