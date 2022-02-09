#ifndef APIAPPLICATIONSLEVEL_H
#define APIAPPLICATIONSLEVEL_H
#include "http_parser.h"
#include "apirequest.h"

class ApiSearchApplications
{
public:
    ApiSearchApplications(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiGetApplication
{
public:
    ApiGetApplication(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiCreateApplication
{
public:
    ApiCreateApplication(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiUpdateApplication
{
public:
    ApiUpdateApplication(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiDeleteApplication
{
public:
    ApiDeleteApplication(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiApplicationsLevel
{

public:
    ApiApplicationsLevel(ApiRequest *rq){this->rq = rq;}
    void exec();
private:
    ApiRequest *rq;
};

#endif // APIAPPLICATIONSLEVEL_H
