#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <database.h>
#include <QDateTime>
#include <QThread>
#include <QObject>
#include "logger.h"
#include "backtcpserver.h"
#include "httpsserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
//    logger->start();
#ifdef Q_OS_WIN
    QString s = "\\";
#else
    QString s = "/";
#endif
    qInfo() << "------start--------";
    QSettings settings(QCoreApplication::applicationDirPath() + s +
                       QFileInfo(QCoreApplication::applicationFilePath()).fileName() +".ini", QSettings::IniFormat);
    settings.setValue(QFileInfo(QCoreApplication::applicationFilePath()).fileName() +
                      "/start", QDateTime().currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));

    QString dbDriver = settings.value("database/driver", "QSQLITE").toString();
    QString dbHost = settings.value("database/host", "localhost").toString();
    QString dbName = settings.value("database/name", "bsrv_db").toString();
    QString dbUser = settings.value("database/user", "bsrv_user").toString();
    QString dbPass = settings.value("database/pass", "bsrv_pass").toString();

    QSqlDatabase *db = connectDataBase(dbDriver, dbHost, dbName, dbUser, dbPass,
                                       QCoreApplication::applicationDirPath() + s + "dbscript.sql");

    BackTcpServer tcpServer(db, &settings);
    QObject::connect(&app, SIGNAL(aboutToQuit()), &tcpServer, SLOT(terminate()));
    QObject::connect(&tcpServer, SIGNAL(finished()), &tcpServer, SLOT(deleteLater()));
    tcpServer.start();

    HttpsServer httpsServer(db, &settings);
    QObject::connect(&app, SIGNAL(aboutToQuit()), &httpsServer, SLOT(terminate()));
    QObject::connect(&httpsServer, SIGNAL(finished()), &tcpServer, SLOT(deleteLater()));
    httpsServer.start();

    QThread udpServerThread;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &udpServerThread, SLOT(terminate()));

    return app.exec();
}
