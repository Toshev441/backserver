#include "loger.h"
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
        loger->write(msg, Loger::INF);
        break;
    case QtWarningMsg:
        loger->write(msg, Loger::WRN);
        break;
    case QtFatalMsg:
    case QtCriticalMsg:
        loger->write(msg, Loger::ERR, QString().number(ctx.line) + " line in " + ctx.file);
        break;
    }
}


Loger applog;
Loger *loger = &applog;

QMutex Loger::mutex;
bool Loger::opened = false;
QFile Loger::f;

Loger::Loger(QObject *parent) : QObject(parent)
{

}

void Loger::setFilePath(const QString &path)
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

bool Loger::start(const QString path)
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

void Loger::stop()
{
    mutex.lock();
    if(opened)
        f.close();
    opened = false;
    mutex.unlock();
}

void Loger::write(QString msg, Loger::eventType type, QString module)
{
    mutex.lock();
    if(!opened)
        return;
    QString t;
    switch (type)
    {
    case Loger::INF:
        t = "INF";
        break;
    case Loger::WRN:
        t = "WRN";
        break;
    case Loger::ERR:
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
