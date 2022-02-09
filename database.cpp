
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QJsonObject>
#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent)
{

}

bool DataBase::open(QString driver, QString host, QString dbName, QString user, QString pass, QString dbScript)
{
    dBase = QSqlDatabase::addDatabase(driver);
    dBase.setHostName(host);
    dBase.setUserName(user);
    dBase.setPassword(pass);
    bool ok = dBase.open();
    if(!ok)
        qWarning() << dBase.lastError().text();
    if(ok)
    {
        QSqlQuery query(dBase);
        if(driver != "QSQLITE")
            ok = query.exec("CREATE DATABASE IF NOT EXISTS "+dbName+";");
        if(!ok)
            qWarning() << query.lastError().text();
        else
        {
            dBase.setDatabaseName(dbName);
            ok = dBase.open();
            if(!ok)
                qWarning() << dBase.lastError().text();
        }
    }
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
            QSqlQuery query(dBase);
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
    return ok;
}

const QSqlDatabase &DataBase::db() const
{
    return dBase;
}

QJsonArray DataBase::exec(QString queryString)
{
    QJsonArray res;
    QSqlQuery query(queryString, dBase);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError){
        return res;
    }
    if(!query.isSelect())
        return res;

    while(query.next()){
        QSqlRecord rec = query.record();
        QJsonObject obj;
        for(int i = 0; i < rec.count(); i++){
            obj.insert(rec.field(i).name(), QJsonValue().fromVariant(rec.field(i).value()));
        }
        res.append(obj);
    }
//    if((query.at() <= 0))
//        return QJsonArray();
    return res;
}
