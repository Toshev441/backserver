#ifndef LOGER_H
#define LOGER_H

#include <QObject>
#include <QMutex>
#include <QFile>

class Loger : public QObject
{
    Q_OBJECT

public:
    enum eventType{
        INF = 0x00,
        WRN,
        ERR
    };

    explicit Loger(QObject *parent = nullptr);

    void setFilePath(const QString &path);
    bool start(const QString path = "");
    void stop();
    void write(QString msg, Loger::eventType type = Loger::ERR, QString module = "");
private:
    static QMutex mutex;
    QString filePath;
    static QFile f;
    static bool opened;
signals:

};

extern Loger *loger;

#endif // LOGER_H
