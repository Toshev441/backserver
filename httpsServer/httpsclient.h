#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H

#include "database.h"
#include <QSslSocket>
#include <QObject>
#include <QThread>
#include <QSettings>
#include <QSslConfiguration>
#include <QSslKey>
#include "httpheader.h"

typedef struct SslConf{
    QSslConfiguration *conf;
    QSslCertificate *cert;
    QSslKey *key;
}SslConf;

class HttpsClient : public QThread
{
    Q_OBJECT
public:
    explicit HttpsClient(qintptr ID, QSqlDatabase *db, SslConf *conf, QObject *parent = nullptr);
    void run();

signals:
    void error(QSslSocket::SocketError socketerror);
    void disconnect(quint64 ID);

public slots:
    void readyRead();
    void disconnected();
private:
    QSslSocket *socket;
    SslConf *sslConf;
    qintptr socketDescriptor;
    QSqlDatabase *existingDB;
    HttpHeader *header;
    DataBase *db = nullptr;
};

#endif // HTTPSCLIENT_H
