#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMutex>
#include <QFile>

class Logger : public QObject
{
    Q_OBJECT

public:
    enum eventType{
        INF = 0x00,
        WRN,
        ERR
    };

    explicit Logger(QObject *parent = nullptr);

    void setFilePath(const QString &path);
    bool start(const QString path = "");
    void stop();
    void write(QString msg, Logger::eventType type = Logger::ERR, QString module = "");
private:
    static QMutex mutex;
    QString filePath;
    static QFile f;
    static bool opened;
signals:

};

extern Logger *logger;

#endif // LOGGER_H
