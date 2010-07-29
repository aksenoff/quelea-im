#ifndef _settingsDialog_h_
#define _settingsDialog_h_

#include <QDialog>

class QLineEdit;
class QCheckBox;
// ======================================================================
class SettingsDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit* clientNameLe;
    QLineEdit* serverAdrLe;
    QCheckBox* autoconnectCheckBox;

public:
    SettingsDialog(QWidget* pwgt = 0);

    QString clientName() const;
    QString serverAdr () const;
    bool autoconnect() const;
};
#endif  //_settingsDialog_h_
