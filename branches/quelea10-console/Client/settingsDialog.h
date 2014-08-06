#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>

class SettingsDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit* clientNameEdit;
    QLineEdit* serverAddressEdit;
    QCheckBox* autoConnectCheckBox;
    QCheckBox* enableSoundCheckBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
public:
    SettingsDialog(QWidget* pwgt = 0);
    QString clientName() const;
    QString serverAddress() const;
    bool autoConnect() const;
    bool enableSound() const;
};

#endif
