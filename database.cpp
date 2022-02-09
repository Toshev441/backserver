#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QJsonObject>
#include <QRandomGenerator>
#include "database.h"

QMutex DataBase::mutex;

DataBase::DataBase(QObject *parent) : QObject(parent)
{

}

DataBase::DataBase(QSqlDatabase *sqlDataBase, const QString &idConnection, QObject *parent) : QObject(parent)
{
    connections = idConnection;
    mutex.lock();
    dBase = new QSqlDatabase(QSqlDatabase::cloneDatabase(*sqlDataBase, connections));
    if(!dBase->open()){
        qWarning() << dBase->lastError().text();
        delete dBase;
        dBase = nullptr;
    }
    mutex.unlock();
}

DataBase::~DataBase()
{
    mutex.lock();
    if(dBase != nullptr){
        delete dBase;
        QSqlDatabase::removeDatabase(connections);
    }
    mutex.unlock();
}

const QSqlDatabase &DataBase::db() const
{
    return *dBase;
}

QJsonArray DataBase::exec(const QString &queryString)
{
    if(!dBase->isOpen()){
        qWarning() << "data base not opened!";
        return QJsonArray();
    }
    QJsonArray res;
    QSqlQuery query = dBase->exec(queryString);
    if(dBase->lastError().type() != QSqlError::NoError){
        return res;
    }
    if(!query.isSelect()){
        return res;
    }
    while(query.next()){
        QSqlRecord rec = query.record();
        QJsonObject obj;
        for(int i = 0; i < rec.count(); i++){
            obj.insert(rec.field(i).name(), QJsonValue().fromVariant(rec.field(i).value()));
        }
        res.append(obj);
    }
    return res;
}

bool DataBase::checkError(bool debug)
{
    bool res = dBase->lastError().type() != QSqlError::NoError;
    if(debug && res){
        qWarning() << "DB error: " << dBase->lastError().type() << ", text error: " << dBase->lastError().text();
    }
    return res;
}

QSqlDatabase *connectDataBase(QString driver, QString host, QString dbName, QString user, QString pass, QString dbScript)
{
    QSqlDatabase * db = new QSqlDatabase (QSqlDatabase::addDatabase(driver));
    db->setHostName(host);
    db->setUserName(user);
    db->setPassword(pass);
    if(driver != "QSQLITE")
        db->setDatabaseName(dbName);
    bool ok = db->open();
    if(!ok)
        qWarning() << db->lastError().text();
    qInfo() << "db connect " << (ok ? "ok":"not ok");
    if(ok)
    {
        qInfo() << "check db...";
        QFile fScript(dbScript);
        ok = fScript.open(QIODevice::ReadOnly);
        if(!ok)
            qWarning() << dbScript << " " << fScript.errorString();
        else
        {
            QString strScript = fScript.readAll();
            QStringList tables = strScript.split(";");
            QSqlQuery query(*db);
            foreach (QString tab, tables)
            {
                if (tab.trimmed().isEmpty()) {
                    continue;
                }
                if(!query.exec(tab)){
                    ok = false;
                    break;
                }
            }
            if(!ok)
                qWarning() << query.lastError().text();
            fScript.close();
        }
    }
    if(ok)
        qInfo() << "db ready";
    return db;
}
