#ifndef DBEDITOR_H
#define DBEDITOR_H

#include <QtGui>
#include "database.h"

class DBEditor : public QDialog {
Q_OBJECT
public:
    DBEditor(Database*, QWidget* pwgt = 0);
private:
    Database* db;
    QTableView* clientsTable;
    QSqlTableModel* model;
    QPushButton* addClientButton;
    QPushButton* delClientButton;
    QDialog* addClientDialog;
private slots:
    void createClientDialog();
};

#endif // DBEDITOR_H
