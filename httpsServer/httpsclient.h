#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H

#include "database.h"
#include <QSslSocket>
#include <QObject>
#include <QThread>
#include <QSettings>

class HttpsClient : public QThread
{
    Q_OBJECT
public:
    explicit HttpsClient(qintptr ID, DataBase *db, QSettings *settings, QObject *parent = nullptr);
    void run();

signals:
    void error(QSslSocket::SocketError socketerror);
    void disconnect(qintptr ID);

public slots:
    void readyRead();
    void disconnected();
private:
    QSslSocket *socket;
    qintptr socketDescriptor;
    DataBase *db;
    QSettings *settings;
};

#endif // HTTPSCLIENT_H
