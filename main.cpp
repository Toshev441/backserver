#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>
#include <QThread>
#include <QObject>
#include "logger.h"
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

    HttpsServer httpsServer(&settings);
    QObject::connect(&app, SIGNAL(aboutToQuit()), &httpsServer, SLOT(terminate()));
    QObject::connect(&httpsServer, SIGNAL(finished()), &httpsServer, SLOT(deleteLater()));
    httpsServer.start();

    QThread udpServerThread;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &udpServerThread, SLOT(terminate()));

    return app.exec();
}
