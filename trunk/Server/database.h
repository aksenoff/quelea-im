#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QtSql>
#include "QueleaServerUI.h"

class QueleaServerUI;
class Database {
private:
    QSqlDatabase* db;
    enum {DB_OPEN_OK, DB_OPEN_ERROR, DB_VERSION_ERROR};
    QString hash(QString password);
    bool opened;
    QueleaServerUI* ui;
public:
    Database(QueleaServerUI*);
    void closeDB();
    int openDB(QString fileName);
    bool createDB(QString fileName);
    bool addClient(QString username, QString password);
    bool isOpened();
    bool authorize(QString name, QString password);
    ~Database();

};

#endif // DATABASE_H
