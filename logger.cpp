#include "logger.h"
#include <QMutex>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDateTime>
#include <QByteArray>

void svcMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
        logger->write(msg, Logger::INF);
        break;
    case QtWarningMsg:
        logger->write(msg, Logger::WRN);
        break;
    case QtFatalMsg:
    case QtCriticalMsg:
        logger->write(msg, Logger::ERR, QString().number(ctx.line) + " line in " + ctx.file);
        break;
    }
}


Logger applog;
Logger *logger = &applog;

QMutex Logger::mutex;
bool Logger::opened = false;
QFile Logger::f;

Logger::Logger(QObject *parent) : QObject(parent)
{

}

void Logger::setFilePath(const QString &path)
{
    if(opened)
    {
        stop();
        filePath = path;
        start();
    }
    else
        filePath = path;
}

bool Logger::start(const QString path)
{
    qInstallMessageHandler(svcMessageHandler);
#ifdef Q_OS_WIN
    QString s = "\\";
#else
    QString s = "/";
#endif
    if(opened)
        stop();
    mutex.lock();
    if(path == "")
        filePath = QCoreApplication::applicationDirPath() + s + QFileInfo(QCoreApplication::applicationFilePath()).fileName() +".log";
    f.setFileName(filePath);
    opened = f.open(QIODevice::ReadWrite | QIODevice::Append);
    mutex.unlock();
    return opened;
}

void Logger::stop()
{
    mutex.lock();
    if(opened)
        f.close();
    opened = false;
    mutex.unlock();
}

void Logger::write(QString msg, Logger::eventType type, QString module)
{
    mutex.lock();
    if(!opened)
        return;
    QString t;
    switch (type)
    {
    case Logger::INF:
        t = "INF";
        break;
    case Logger::WRN:
        t = "WRN";
        break;
    case Logger::ERR:
        t = "ERR";
        break;
    }
    QString m = "";
    if(module != "")
        m = " ("+ module +")";
    QString timeStapm = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    f.write(QByteArray(QString(t + " " + timeStapm + " " + msg + m).toLocal8Bit()) + "\r\n");
    f.flush();
    mutex.unlock();
}
