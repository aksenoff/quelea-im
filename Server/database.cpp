#include "database.h"
#include "QueleaServerUI.h"
#include "versionInfo.h"

Database::Database()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
}

int Database::openDB(QString filename)
{
    db->setDatabaseName(filename);

    if(db->open()){
        QSqlQuery query(*db);
        query.exec("SELECT value FROM quelea WHERE property = 'DBtype';");
        query.next();
        if (query.value(0).toString() == "qdb"){
            query.exec("SELECT value FROM quelea WHERE property = 'DBversion';");
            query.next();
            if (query.value(0).toString() == QDB_VERSION)
                return DB_OPEN_OK;
            else
                return DB_VERSION_ERROR;
        }
        else
            return DB_OPEN_ERROR;
    }
    else
        return DB_OPEN_ERROR;
}

//-------------------------------------------------------------------------------

void Database::closeDB()
{
    db->close();
}

//-------------------------------------------------------------------------------

bool Database::createDB(QString fileName)
{
    closeDB();
    db->setDatabaseName(fileName);
    if (db->open()){
        QSqlQuery query(*db);
        query.exec("CREATE TABLE quelea(property VARCHAR, value VARCHAR);");
        query.exec("INSERT INTO quelea VALUES('DBtype', 'qdb');");
        query.exec("INSERT INTO quelea VALUES('DBversion', '"+QDB_VERSION+"');");
        query.exec("CREATE TABLE clients(name VARCHAR, password VARCHAR);");
        return true;
    }
    else
        return false;
}
