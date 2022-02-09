#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QObject>
#include "httpheader.h"
#include "database.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSslSocket>

class ApiRequest : public QObject
{
    Q_OBJECT
public:
    explicit ApiRequest(QSslSocket *socket, HttpHeader *header, DataBase *db, QObject *parent = nullptr);
    bool exec();

    QJsonObject *pError();
    QJsonObject *pData();
    http_status *pStatus();
    HttpHeader *pHeader();
    QString getUrlParam(QJsonObject &obj, QString key, QString defaultValue = "");
    DataBase *bBase();
    void setError(QString err, int errCode = 0, QJsonArray details = QJsonArray());
    QString &pServiceGUID();
private:
    HttpHeader *header;
    DataBase *db;
    QSslSocket *socket;
    http_status httpStatus;
    QJsonObject httpError;
    QJsonObject httpData;
    QString serviceGUID;
    void writeResult();
signals:

};

#endif // APIREQUEST_H
