#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>

class SettingsDialog : public QDialog {
    Q_OBJECT
private:
    QRadioButton* dbRadio;
    QGroupBox* dbGroupBox;
    QLineEdit* dbNameEdit;
    QLineEdit* dbPasswordEdit;
    QRadioButton* guestRadio;
    QLineEdit* clientNameEdit;
    QLineEdit* serverAddressEdit;
    QCheckBox* autoConnectCheckBox;
    QCheckBox* enableSoundCheckBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QString hash(QString password);
    QString oldHashedPass;
    enum {GUEST_AUTH, DB_AUTH, LDAP_AUTH};
public:
    SettingsDialog(QWidget* pwgt = 0);
    QString clientName() const;
    QString serverAddress() const;
    QString dbName() const;
    QString dbPassword();
    bool autoConnect() const;
    bool enableSound() const;
    int authType();

};

#endif
