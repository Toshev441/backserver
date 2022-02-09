#include "apiserviceslevel.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QUuid>

inline QString getParam(QJsonObject *obj, QString param, QString defaultValue = ""){
    if(!obj->contains(param))
        return defaultValue;
    return obj->value(param).toString();
}

/**
 * @brief ApiServicesLevel::exec
@request
{
"request":"string",
"user":"string",
"pass":"string",
.....,
 */
void ApiServicesLevel::exec()
{
    QJsonObject body = QJsonDocument::fromJson(rq->pHeader()->getBody()).object();
    if(!body.contains("request") || !body.contains("user") || !body.contains("pass")){
        rq->setError("body not contains 'request' or 'user' or 'pass'");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString queryString;
    *rq->pData() = QJsonObject();
    
    QString user = getParam(&body, "user");
    QString pass = getParam(&body, "pass");
    queryString = "SELECT COUNT(*) AS cnt FROM users WHERE name='"+user+"' AND pass='"+pass+"';";
    int recCount = rq->bBase()->exec(queryString)[0].toObject()["cnt"].toInt();
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    if(recCount == 0){
        rq->setError("wrong user/pass");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    if(body["request"].toString() == "search_services")
    {
        ApiSearchServices * request = new ApiSearchServices(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "get_service")
    {
        ApiGetService * request = new ApiGetService(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "create_service")
    {
        ApiCreateService * request = new ApiCreateService(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "update_service")
    {
        ApiUpdateService * request = new ApiUpdateService(&body, rq);
        request->exec();
        delete request;
        return;
    }
    if(body["request"].toString() == "delete_service")
    {
        ApiDeleteService * request = new ApiDeleteService(&body, rq);
        request->exec();
        delete request;
        return;
    }
    rq->setError(body["request"].toString() + " - unknow request");
    *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
}

/**
 * @brief ApiSearchServices::exec
@request
{
"request":"string",
"user":"string",
"pass":"string",
"limit":"10",
"offset:"2",
"search":"search by name"

@return
 {
 "totalCount":"10",
 "result":[
 {
   "id":"ff00:.....",
   "name":"10",
   "description:"2",
  },
  ....
  ]
}
*/
void ApiSearchServices::exec()
{
    *rq->pData() = QJsonObject();
    QString limit = getParam(body, "limit", "0");
    QString offset = getParam(body, "offset", "0");
    QString queryString = "\
SELECT GUID AS id, name, description FROM services WHERE name LIKE '%"+getParam(body, "search")+"%' "+
    +"LIMIT "+limit+" OFFSET "+offset+";";
    QJsonArray sqlData = rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    *rq->pData()->insert("totalCount", sqlData.count());
    *rq->pData()->insert("result", sqlData);
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiCreateService::exec
@request
{
"request":"string",
"user":"string",
"pass":"string",
"name":"string",
"description:"string"
}

@return
 {
   "id":"ff00:.....",
   "name":"10",
   "description":"2",
   "token":"string"
 }
 */
void ApiCreateService::exec()
{
    *rq->pData() = QJsonObject();
    if(!body->contains("name")){
        rq->setError("name not contains in body");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString guid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString token =
            QUuid::createUuid().toByteArray(QUuid::WithoutBraces).toBase64()+
            QUuid::createUuid().toByteArray(QUuid::WithoutBraces).toBase64()+
            QUuid::createUuid().toByteArray(QUuid::WithoutBraces).toBase64()+
            QUuid::createUuid().toByteArray(QUuid::WithoutBraces).toBase64();

    QString queryString = "INSERT INTO services (GUID, name, description, token) VALUES ('"+
        guid+"', '"+
        getParam(body, "name")+"', '"+
        getParam(body, "description")+"', '"+
        token+"');";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    rq->pData()->insert("id", guid);
    rq->pData()->insert("token", token);
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiGetService::exec
@request
{
"request":"string",
"user":"string",
"pass":"string",
"id":"string"
}

@return
 {
   "id":"ff00:.....",
   "name":"10",
   "description":"2",
 }
 */
void ApiGetService::exec()
{
    if(!body->contains("id")){
        rq->setError("id not contains in body");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString queryString = "\
SELECT GUID AS id, name, description, token FROM services WHERE GUID = '"+getParam(body, "id")+"';";
    QJsonArray sqlData = rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    *rq->pData() = sqlData[0].toObject();
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiGetService::exec
@request
{
"request":"string",
"user":"string",
"pass":"string",
"id":"string",
"name":"10",
"description":"2"
}

@return
 {
   "id":"ff00:....."
 }
 */
void ApiUpdateService::exec()
{
    if(!body->contains("id")){
        rq->setError("id not contains in body");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString id = getParam(body, "id");
    QString name = getParam(body, "name");
    QString description = getParam(body, "description");
    QString queryString = "UPDATE services SET "+
            (name != "" ? "name = '"+name+"'" : "")+
            ((name != "") && (description != "") ? ", " : "")+
            (description != "" ? " description = '"+description+"' " : "")+
            " WHERE GUID = '" +id+"';";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    rq->pData()->insert("id", id);
    *rq->pStatus() = HTTP_STATUS_OK;
}

/**
 * @brief ApiGetService::exec
@request
{
"request":"string",
"user":"string",
"pass":"string",
"id":"string"
}

@return
 {
 }
 */
void ApiDeleteService::exec()
{
    if(!body->contains("id")){
        rq->setError("id not contains in body");
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        return;
    }
    QString queryString = "DELETE FROM services WHERE GUID = '"+getParam(body, "id")+"';";
    rq->bBase()->exec(queryString);
    if(rq->bBase()->db().lastError().type() != QSqlError::NoError){
        rq->setError(rq->bBase()->db().lastError().text());
        *rq->pStatus() = HTTP_STATUS_BAD_REQUEST;
        qWarning() << rq->bBase()->db().lastError().text();
        return;
    }
    *rq->pStatus() = HTTP_STATUS_OK;
}




