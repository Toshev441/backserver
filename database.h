#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMutex>

class DataBase : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase *dBase = nullptr;
    QString connections = "";
    static QMutex mutex;
public:
    explicit DataBase(QObject *parent = nullptr);
    explicit DataBase(QSqlDatabase *sqlDataBase, const QString &idConnection, QObject *parent = nullptr);
    ~DataBase();
    const QSqlDatabase &db() const;
    QJsonArray exec(const QString &queryString);
    bool checkError(bool debug = false);
signals:

};

QSqlDatabase *connectDataBase(QString driver, QString host, QString dbName, QString user, QString pass, QString dbScript);

#endif // DATABASE_H
