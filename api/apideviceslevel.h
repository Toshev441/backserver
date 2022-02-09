#ifndef APIDEVICESLEVEL_H
#define APIDEVICESLEVEL_H

#include "apirequest.h"

class ApiSearchDevices
{
public:
    ApiSearchDevices(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiGetDevice
{
public:
    ApiGetDevice(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiUpdateDevice
{
public:
    ApiUpdateDevice(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiDeleteDevice
{
public:
    ApiDeleteDevice(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiCreateDevices
{
public:
    ApiCreateDevices(QJsonObject * body, ApiRequest *rq){this->rq = rq; this->body = body;}
    void exec();
private:
     ApiRequest *rq;
     QJsonObject * body;
};

class ApiDevicesLevel
{

public:
    ApiDevicesLevel(ApiRequest *rq){this->rq = rq;}
    void exec();
private:
    ApiRequest *rq;
};
#endif // APIDEVICESLEVEL_H
