#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <database.h>
#include <QDateTime>
#include <QThread>
#include <QObject>
#include "loger.h"
#include "backtcpserver.h"
#include "fronthttpsserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
//    loger->start();
#ifdef Q_OS_WIN
    QString s = "\\";
#else
    QString s = "/";
#endif
    qInfo() << "------start--------";
    QSettings settings(QCoreApplication::applicationDirPath() + s + QFileInfo(QCoreApplication::applicationFilePath()).fileName() +".ini", QSettings::IniFormat);
    settings.setValue(QFileInfo(QCoreApplication::applicationFilePath()).fileName() +"/start", QDateTime().currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));
    QString dbDriver = settings.value("database/driver", "QSQLITE").toString();
    QString dbHost = settings.value("database/host", "localhost").toString();
    QString dbName = settings.value("database/name", "bsrv_db").toString();
    QString dbUser = settings.value("database/user", "bsrv_user").toString();
    QString dbPass = settings.value("database/pass", "bsrv_pass").toString();

    DataBase db;
    db.open(dbDriver, dbHost, dbName, dbUser, dbPass, QCoreApplication::applicationDirPath() + s + "dbscript.sql");
//    db.open(dbDriver, dbHost, dbName, dbUser, dbPass, "D:/OneDrive/Projects/backserver/dbscript.sql");
    BackTcpServer tcpServer(&db, &settings);
    QThread tcpServerThread;
    tcpServer.moveToThread(&tcpServerThread);
    QObject::connect(&app, SIGNAL(aboutToQuit()), &tcpServerThread, SLOT(terminate()));
    QObject::connect(&tcpServerThread, &QThread::finished, &tcpServer, &BackTcpServer::deleteLater);
    QObject::connect(&tcpServerThread, &QThread::started, &tcpServer, &BackTcpServer::start);
    tcpServerThread.start();

    FrontHttpsServer httpsServer(&db, &settings);
    QThread httpsServerThread;
    httpsServer.moveToThread(&httpsServerThread);
    QObject::connect(&app, SIGNAL(aboutToQuit()), &httpsServerThread, SLOT(terminate()));
    QObject::connect(&httpsServerThread, &QThread::finished, &httpsServer, &FrontHttpsServer::deleteLater);
    QObject::connect(&httpsServerThread, &QThread::started, &httpsServer, &FrontHttpsServer::start);
    httpsServerThread.start();


    QThread udpServerThread;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &udpServerThread, SLOT(terminate()));

    return app.exec();
}
