#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>

class DataBase : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase dBase;

public:
    explicit DataBase(QObject *parent = nullptr);
    bool open(QString driver, QString host, QString dbName, QString user, QString pass, QString dbScript);

    const QSqlDatabase &db() const;

    QJsonArray exec(QString queryString);
signals:

};

#endif // DATABASE_H
