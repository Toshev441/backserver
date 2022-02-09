#ifndef APISERVICESLEVEL_H
#define APISERVICESLEVEL_H
#include "http_parser.h"
#include "apirequest.h"

class ApiCreateService
{
public:
    ApiCreateService(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiSearchServices
{
public:
    ApiSearchServices(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiGetService
{
public:
    ApiGetService(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiUpdateService
{
public:
    ApiUpdateService(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiDeleteService
{
public:
    ApiDeleteService(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiServicesLevel
{

public:
    ApiServicesLevel(ApiRequest *rq){this->rq = rq;}
    void exec();
private:
    ApiRequest *rq;
};

#endif // APISERVICESLEVEL_H
