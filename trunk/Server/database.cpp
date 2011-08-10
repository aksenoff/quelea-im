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

int Database::createDB(QString fileName)
{
    closeDB();
}
