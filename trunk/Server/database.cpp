#include "database.h"
#include "QueleaServerUI.h"
#include "versionInfo.h"

Database::Database(QueleaServerUI* userInterface):ui(userInterface)
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    opened = false;
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
            if (query.value(0).toString() == QDB_VERSION){
                opened = true;
                ui->log("Database opened");
                return DB_OPEN_OK;
            }
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
    opened = false;
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
        opened = true;
        return true;
    }
    else
        return false;
}

//-------------------------------------------------------------------------------


QString Database::hash(QString password)
{
    QByteArray array;
    array.insert(0,password);
    return QString(QCryptographicHash::hash(array, QCryptographicHash::Md5).toBase64());
}

//-------------------------------------------------------------------------------

bool Database::addClient(QString username, QString password)
{
    QSqlQuery query(*db);
    return query.exec("INSERT INTO clients VALUES('"+username+"','"+hash(password)+"');");
}

//-------------------------------------------------------------------------------

bool Database::isOpened()
{
    return opened;
}

bool Database::authorize(QString name, QString password)
{
    QSqlQuery query(*db);
    query.exec("SELECT password FROM clients WHERE name = '"+name+"'");
    query.next();
    if (query.value(0).toString() == password)
        return true;
    else
        return false;

}
//-------------------------------------------------------------------------------
Database::~Database()
{
    closeDB();
}
