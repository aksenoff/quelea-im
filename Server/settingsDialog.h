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
    QPushButton* okButton;
    QPushButton* cancelButton;
    DBEditor* dbEdit;
public:
    SettingsDialog(Database*, QWidget* pwgt = 0);
    bool useDB() const;
private slots:
    void loadDB();
    void createDB();
    void openDBEditor();
};

#endif
