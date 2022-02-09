#include "apirequest.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <apiapplicationslevel.h>
#include <apideviceslevel.h>
#include <apiserviceslevel.h>
#include <QString>

ApiRequest::ApiRequest(QSslSocket *socket, HttpHeader *header, DataBase *db, QObject *parent) : QObject(parent)
{
    this->socket= socket;
    this->header = header;
    this->db = db;
}

bool ApiRequest::exec()
{
    if(header->getMethod() != HTTP_POST){
        return false;
    }
    QStringList parts = header->getUrlPath().split("/", QString::SkipEmptyParts);
    if (parts.count() == 0) {
        return false;
    }
    if(parts[0].compare("api", Qt::CaseInsensitive) != 0){
        return false;
    }
    QString apiLevel = parts[1].toLower();
    if(apiLevel == "services"){
        ApiServicesLevel * apiAppLevel = new ApiServicesLevel(this);
        apiAppLevel->exec();
        delete apiAppLevel;
    }
    else
    {
        QString token = header->getHeaderValue("Grpc-Metadata-Authorization");
        QJsonArray arr = db->exec("SELECT GUID FROM services WHERE token = '"+token+"';");
        if(arr.count() <= 0){
            setError("wrong token");
            qDebug() << "wrong";
            httpStatus = HTTP_STATUS_BAD_REQUEST;
            writeResult();
            return true;
        }
        serviceGUID = arr[0].toObject()["GUID"].toString();
        if(apiLevel == "applications"){
            ApiApplicationsLevel * apiAppLevel = new ApiApplicationsLevel(this);
            apiAppLevel->exec();
            delete apiAppLevel;
        }
        else if(apiLevel == "devices"){
            ApiDevicesLevel * apiAppLevel = new ApiDevicesLevel(this);
            apiAppLevel->exec();
            delete apiAppLevel;
        }
    }
    writeResult();
    return true;
}

void ApiRequest::writeResult()
{
    QString httpResult = QString().number(httpStatus) + " " + http_status_str(httpStatus);
    QByteArray data;
    if(httpStatus != HTTP_STATUS_OK)
        data = QJsonDocument(httpError).toJson(QJsonDocument::Compact);
    else
        data = QJsonDocument(httpData).toJson(QJsonDocument::Compact);
    QByteArray response;
    response.append(QString("HTTP/1.1 "+httpResult+"\r\n").toLocal8Bit());
    response.append(QString("content-type: application/json\r\n").toLocal8Bit());
    response.append(QString("content-length: "+QString().number(data.size())+"\r\n").toLocal8Bit());
    response.append(QString("Server: BackServer\r\n").toLocal8Bit());
    response.append(QString("\r\n").toLocal8Bit());
    response.append(data);
    socket->write(response);
}

void ApiRequest::setError(QString err, int errCode, QJsonArray details)
{
    httpError = QJsonObject();
    httpError.insert("error", err);
    httpError.insert("code", errCode);
    httpError.insert("message", err);
    httpError.insert("details", details);
}

QString &ApiRequest::pServiceGUID()
{
    return serviceGUID;
}

QJsonObject *ApiRequest::pData()
{
    return &httpData;
}

http_status *ApiRequest::pStatus()
{
    return &httpStatus;
}

HttpHeader *ApiRequest::pHeader()
{
    return header;
}

QJsonObject *ApiRequest::pError()
{
    return &httpError;
}

QString ApiRequest::getUrlParam(QJsonObject &obj, QString key, QString defaultValue)
{
    if(!obj.contains(key))
        return defaultValue;
    return obj.value(key).toString();
}

DataBase *ApiRequest::bBase()
{
    return db;
}


