#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QtSql>


class Database {
private:
    QSqlDatabase* db;
    enum {DB_OPEN_OK, DB_OPEN_ERROR, DB_VERSION_ERROR};
public:
    Database();
    void closeDB();
    int openDB(QString fileName);
    bool createDB(QString fileName);
};

#endif // DATABASE_H
