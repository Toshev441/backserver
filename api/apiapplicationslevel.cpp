#include "apiapplicationslevel.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QUuid>

inline QString getParam(QJsonObject *obj, QString param, QString defaultValue = ""){
    if(!obj->contains(param))
        return defaultValue;
    return obj->value(param).toString();
}

void ApiApplicationsLevel::exec()
{
    QJsonObject body = QJsonDocument::fromJson(rq->pHeader()->getBody()).object();
    if(!body.contains("request")){
        rq->setError("body not contains 'request'");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    if(body["request"].toString() == "search_applications")
    {
        ApiSearchApplications * request = new ApiSearchApplications(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "get_application")
    {
        ApiGetApplication * request = new ApiGetApplication(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "create_application")
    {
        ApiCreateApplication * request = new ApiCreateApplication(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "update_application")
    {
        ApiUpdateApplication * request = new ApiUpdateApplication(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "delete_application")
    {
        ApiDeleteApplication * request = new ApiDeleteApplication(&body, rq);
        request->exec();
        delete request;
        return;
    }
    rq->setError(body["request"].toString() + " - unknow request");
    *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
}

/**
 * @brief ApiSearchApplications::exec
@request
 {
"request":"search_applications",
"limit":"10",
"offset:"2",
"search":"search by name"
}

@return
 {
 "totalCount":"10",
 "result":[
 {
   "id":"ff00:.....",
   "name":"10",
   "description:"2",
   "serviceID":"ff00:....."
  },
  ....
  ]
}
*/
void ApiSearchApplications::exec()
{
    QString queryString;
    *rq->pData() = QJsonObject();

    QString limit = getParam(body, "limit", "0");
    if(limit == "0"){
        queryString = "SELECT COUNT(*) AS cnt FROM applications WHERE service_ID = '"+rq->pServiceID()+"';";
        int recCount = rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt();
        if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
            rq->setError(rq->bBase()->db().lastError().text());
            *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
            qWarning() << rq->bBase()->db().lastError().text();
            return;
        }
        rq->pData()->insert("totalCount", QString().number(recCount));
        rq->pData()->insert("result", QJsonArray());
        *rq->pStatus() = HTTP_STATUS_OK;
        return;
    }
    queryString = "SELECT applications.ID AS id, \
applications.name AS name, \
            applications.description AS description, \
            applications.service_ID AS serviceID \
            FROM applications \
            WHERE serviceID = '"+rq->pServiceID()+"' \
            AND applications.name like '%"+getParam(body, "search")+"%' LIMIT '"+limit+"' OFFSET '"+getParam(body, "offset", "0")+"';";

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
 * @brief ApiGetApplication::exec
@request
 {
"request":"get_application",
"id":"ff00ee44..."
}

@return
 {
   "id":"ff00:.....",
   "name":"10",
   "description:"2",
   "serviceID":"ff00:....."
}
*/
void ApiGetApplication::exec()
{
    QString id = getParam(body, "id");
   QString queryString = "SELECT applications.ID AS id, \
applications.name AS name, \
            applications.description AS description, \
            applications.service_ID AS serviceID \
            FROM applications \
            WHERE applications.ID = '"+id+"' AND service_ID = '"+rq->pServiceID()+"';";
   qDebug() << queryString;
    QJsonArray sqlData = rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    if(sqlData.count() == 0){
        rq->setError("'"+id+"' - application not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    *rq->pData() = sqlData[0].toObject();
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiCreateApplication::exec
@request
{
"request":"create_application",
"name:"string",
"description":"string",
"applicationKey":"ff00....",
}

@return
{
"id":"ff00....."
}
*/
void ApiCreateApplication::exec()
{
    *rq->pData() = QJsonObject();
    if(!body->contains("name")){
        rq->setError("name not contains in body");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString serviceID = rq->pServiceID();
    QString queryString = "SELECT COUNT(*) AS cnt FROM services WHERE ID = '"+serviceID+"';";
    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() == 0){
        rq->setError("'"+serviceID + "' - service not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString uid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    queryString = "\
INSERT INTO applications (ID, name, description, app_key, service_ID) \
VALUES ('"+
        uid+"', '"+
        getParam(body, "name")+"', '"+
        getParam(body, "description")+"', '"+
        getParam(body, "applicationKey")+"', '"+
        rq->pServiceID()+"');";

    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    rq->pData()->insert("id", uid);
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiUpdateApplication::exec
@request
{
"request":"update_application",
"id":"ff00.....",
"name:"string",
"description":"string",
"applicationKey":"ff00....",
}

@return
{
"id":"ff00....."
}
*/
void ApiUpdateApplication::exec()
{
    *rq->pData() = QJsonObject();

    QString id = getParam(body, "id");
    QString queryString = "SELECT COUNT(*) AS cnt FROM applications WHERE ID = '"+id+"' AND service_ID = '"+rq->pServiceID()+"';";
    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() == 0){
        rq->setError("'"+id + "' - application not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }

    QString name = getParam(body, "name");
    QString description = getParam(body, "description");
    QString applicationKey = getParam(body, "applicationKey");
    queryString = "UPDATE applications SET \
             name = '"+name+"', "+
             "description = '"+description+"', "+
             "app_key = '"+applicationKey+"' "+
             "WHERE ID = '" +id+"';";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    rq->pData()->insert("id", getParam(body, "id"));
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiDeleteApplication::exec
@request
{
"request":"delete_application",
"id":"ff00....."
}

@return
{}
*/
void ApiDeleteApplication::exec()
{
    *rq->pData() = QJsonObject();

    QString id = getParam(body, "id");
    QString queryString = "SELECT COUNT(*) AS cnt FROM applications WHERE ID = '"+
            id+"' AND service_ID = '"+
            rq->pServiceID()+"';";
    if(rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt() == 0){
        rq->setError("'"+id + "' - application not found");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    queryString = "DELETE FROM devices WHERE app_ID = '"+id+"';";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    queryString = "DELETE FROM applications WHERE ID = '"+id+"';";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    *rq->pStatus() = HTTP_STATUS_OK;
}


