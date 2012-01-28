#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>
#include "database.h"
#include "dbeditor.h"

class SettingsDialog : public QDialog {
    Q_OBJECT
private:
    Database* db;
    enum {DB_OPEN_OK, DB_OPEN_ERROR, DB_VERSION_ERROR};
    QTabWidget* tabWidget;
    QGroupBox* dbGroupBox;
    QLabel* dbTooltip;
    QLabel* dbState;
    QLineEdit* dbPathEdit;
    QPushButton* dbLoadButton;
    QPushButton* dbCreateButton;
    QPushButton* dbEditButton;
    QGroupBox* ldapGroupBox;
    QRadioButton* adRadio;
    QRadioButton* otherRadio;
    QLabel* ldapState;
    QLineEdit* ldapServerEdit;
    QLineEdit* ldapPortEdit;
    QLineEdit* ldapDomainEdit;
    QLineEdit* ldapAdminDnEdit;
    QLineEdit* ldapAdminPwdEdit;
    QPushButton* ldapConnectButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    DBEditor* dbEdit;
public:
    SettingsDialog(Database*, QWidget* pwgt = 0);
    bool useDB() const;
    QString dbFileName() const;
private slots:
    void loadDB();
    void loadDB(QString fileName);
    void createDB();
    void openDBEditor();
    void dbGroupBoxToggled(bool enabled);
    void ldapTypeToggled(bool ad);
};

#endif
