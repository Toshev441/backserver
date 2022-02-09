#include "apideviceslevel.h"

#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonValue>

inline QString getParam(QJsonObject *obj, QString param, QString defaultValue = ""){
    if(!obj->contains(param))
        return defaultValue;
    return obj->value(param).toString();
}

void ApiDevicesLevel::exec()
{
    QJsonObject body = QJsonDocument::fromJson(rq->pHeader()->getBody()).object();
    if(!body.contains("request")){
        rq->setError("body not contains 'request'");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    if(body["request"].toString() == "search_devices")
    {
        ApiSearchDevices * request = new ApiSearchDevices(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "get_device")
    {
        ApiGetDevice * request = new ApiGetDevice(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "create_device")
    {
        ApiCreateDevices * request = new ApiCreateDevices(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "update_device")
    {
        ApiUpdateDevice * request = new ApiUpdateDevice(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "delete_device")
    {
        ApiDeleteDevice * request = new ApiDeleteDevice(&body, rq);
        request->exec();
        delete request;
        return;
    }
    rq->setError(body["request"].toString() + " - unknow request");
    *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
}

/**
 * @brief ApiSearchDevices::exec
@request
 {
"request":"search_devices",
"limit":"10",
"offset:"2",
"applicationID":"string",
"search":"search by name"
}

@return
 {
 "totalCount":"10",
 "result":[
 {
   "devEUI":"ff00:.....",
   "name":"10",
   "description:"2",
   "applicationID":"string",
   "lastSeenAt":"12:10:2020T12:34:12.123"
  },
  ....
  ]
}
*/
void ApiSearchDevices::exec()
{
    QString queryString;
    *rq->pData() = QJsonObject();
    QString appID = getParam(body, "applicationID");
    QString limit = getParam(body, "limit", "0");
    queryString = "SELECT devices.EUI AS devEUI, \
devices.name AS name, \
            devices.description AS description, \
            devices.app_GUID AS applicationID \
            devices.last_seen_at AS lastSeenAt \
            FROM devices \
            LEFT JOIN applications ON devices.app_GUID = applications.GUID \
            LEFT JOIN services ON applications.service_GUID = services.GUID \
            WHERE services.GUID = '"+rq->pServiceGUID()+"' "+(appID == "" ? "": "AND applications.GUID = '" +appID+"' ") +
            "AND devices.name like '%"+getParam(body, "search")+"%' LIMIT '"+limit+"' OFFSET '"+getParam(body, "offset", "0")+"';";
qWarning() << queryString;
    QJsonArray sqlData = rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    rq->pData()->insert("totalCount", sqlData.count());
    rq->pData()->insert("result", sqlData);
    *rq->pStatus() = HTTP_STATUS_OK;
}
/**
 * @brief ApiGetDevice::exec
@request
 {
"request":"get_device",
"devEUI":"string"
}

@return
 {
   "devEUI":"ff00:.....",
   "name":"10",
   "description:"2",
   "applicationID":"string",
   "lastSeenAt":"12:10:2020T12:34:12.123"
 }
 */
void ApiGetDevice::exec()
{
    QString id = getParam(body, "devEUI");
   QString queryString = "SELECT devices.EUI AS devEUI, \
devices.name AS name, \
            devices.description AS description, \
            devices.app_GUID AS applicationID \
           devices.last_seen_at AS lastSeenAt \
            FROM devices \
           LEFT JOIN applications ON devices.app_GUID = applications.GUID \
           LEFT JOIN services ON applications.service_GUID = service.GUID \
           WHERE devices.GUID = '"+id+"' AND service_GUID = '"+rq->pServiceGUID()+"';";
    QJsonArray sqlData = rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    if(sqlData.count() == 0){
        rq->setError("'"+id+"' - device not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    *rq->pData() = sqlData[0].toObject();
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiCreateDevices::exec
@request
 {
"request":"create_device",
"devEUI":"string"
"name":"10",
"description:"2",
"applicationID":"string",
"session":"string"
}

@return
 {
   "devEUI":"ff00:....."
 }
 */
void ApiCreateDevices::exec()
{
    *rq->pData() = QJsonObject();
    if(!body->contains("name") || !body->contains("applicationID") || !body->contains("devEUI")){
        rq->setError("name, applicationID or devEUI not contains in body");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString serviceID = rq->pServiceGUID();
    QString appID = getParam(body, "applicationID");
    QString queryString = "SELECT COUNT(*) AS cnt FROM applications WHERE GUID = '"+
            appID+"' AND service_GUID = '"+serviceID+"';";

    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() == 0){
        rq->setError("'"+serviceID + "' - service not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    queryString = "SELECT COUNT(*) AS cnt FROM devices WHERE GUID = '"+
            getParam(body, "devEUI")+"';";
    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() != 0){
        rq->setError("'"+getParam(body, "devEUI") + "' - devEUI already exists");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    queryString = "\
INSERT INTO devices (EUI, name, description, app_GUID, session, last_seen_at) \
VALUES ('"+
        getParam(body, "devEUI")+"', '"+
        getParam(body, "name")+"', '"+
        getParam(body, "description")+"', '"+
        getParam(body, "applicationID")+"', '"+
        getParam(body, "sessionKey")+"', '');";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    qWarning() << queryString;
    rq->pData()->insert("devEUI", getParam(body, "devEUI"));
    *rq->pStatus() = HTTP_STATUS_OK;
}
/**
 * @brief ApiUpdateDevice::exec
 {
"request":"update_device",
"devEUI":"string"
"name":"10",
"description:"2",
}

@return
 {
   "devEUI":"ff00:....."
 }
 */
void ApiUpdateDevice::exec()
{
    *rq->pData() = QJsonObject();

    QString id = getParam(body, "devEUI");
    QString queryString = "SELECT COUNT(*) AS cnt FROM devices  \
  LEFT JOIN applications ON devices.app_GUID = applications.GUID \
            WHERE devices.EUI = '"+id+"' AND applications.service_GUID = '"+rq->pServiceGUID()+"';";
    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() == 0){
        rq->setError("'"+id + "' - application not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString name = getParam(body, "name");
    QString description = getParam(body, "description");
    queryString = "UPDATE devices SET "+
            (name != "" ? "name = '"+name+"'" : "") + ((name != "") && (description != "") ? ", " : "") +
            (description != "" ? " description = '"+description+"' " : "") +
            " WHERE EUI = '" +id+"';";
    qWarning() << queryString;
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    rq->pData()->insert("devEUI", id);
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiDeleteDevice::exec
 {
"request":"delete_device",
"devEUI":"string"
}

@return
 {}
*/
void ApiDeleteDevice::exec()
{
    *rq->pData() = QJsonObject();

    QString id = getParam(body, "devEUI");
    QString queryString = "SELECT COUNT(*) AS cnt FROM devices \
LEFT JOIN applications ON applications.GUID = devices.app_GUID \
            WHERE devices.EUI = '"+
            id+"' AND applications.service_GUID = '"+
            rq->pServiceGUID()+"';";
    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() == 0){
        rq->setError("'"+id + "' - device not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    queryString = "DELETE FROM devices WHERE EUI = '"+id+"';";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    *rq->pStatus() = HTTP_STATUS_OK;
}

