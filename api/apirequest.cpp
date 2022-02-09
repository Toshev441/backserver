#include "apirequest.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <apiapplicationslevel.h>
#include <apideviceslevel.h>

ApiRequest::ApiRequest(QSslSocket *socket, HttpHeader *header, DataBase *db, QObject *parent) : QObject(parent)
{
    this->socket= socket;
    this->header = header;
    this->db = db;
}

void ApiRequest::exec()
{
    if(header->getMethod() != HTTP_POST){
        setError("POST request support only!");
        httpStatus = HTTP_STATUS_BAD_REQUEST;
        writeResult();
        return;
    }
    QStringList parts = header->getUrlPath().split("/", Qt::SkipEmptyParts);
    if (parts.count() == 0) {
        setError(header->getUrlPath() + " - bad url request!");
        httpStatus = HTTP_STATUS_BAD_REQUEST;
        writeResult();
        return;
    }
    if(parts[0].compare("api", Qt::CaseInsensitive) != 0){
        setError(header->getUrlPath() + " - bad url request!");
        httpStatus = HTTP_STATUS_BAD_REQUEST;
        writeResult();
        return;
    }
    QString token = header->getHeaderValue("Grpc-Metadata-Authorization");
    QJsonArray arr = db->exec("SELECT ID FROM services WHERE token = '"+token+"';");
    if(arr.count() <= 0){
        setError("wrong token");
        httpStatus = HTTP_STATUS_BAD_REQUEST;
        writeResult();
        return;
    }
    serviceID = arr[0].toObject()["ID"].toString();
    QString apiLevel = parts[1].toLower();
    if(apiLevel == "applications"){
        ApiApplicationsLevel * apiAppLevel = new ApiApplicationsLevel(this);
        apiAppLevel->exec();
        delete apiAppLevel;
    }
    if(apiLevel == "devices"){
        ApiDevicesLevel * apiAppLevel = new ApiDevicesLevel(this);
        apiAppLevel->exec();
        delete apiAppLevel;
    }
    writeResult();
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

QString &ApiRequest::pServiceID()
{
    return serviceID;
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


