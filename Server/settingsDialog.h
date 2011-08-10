#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>
#include "database.h"

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
    QGroupBox* ldapGroupBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
public:
    SettingsDialog(Database*, QWidget* pwgt = 0);
    QString clientName() const;
    QString serverAddress() const;
    bool autoConnect() const;
    bool enableSound() const;
private slots:
    void loadDB();
    void createDB();
};

#endif
