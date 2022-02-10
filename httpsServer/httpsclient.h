#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H

#include <QSslSocket>
#include <QObject>
#include <QThread>
#include <QSettings>
#include <QSslConfiguration>
#include <QSslKey>
#include <QList>
#include <QSslError>
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
    explicit HttpsClient(qintptr ID, SslConf *conf, QObject *parent = nullptr);
    void run();

signals:
    void error(QSslSocket::SocketError socketerror);
    void disconnect(quint64 ID);

public slots:
    void readyRead();
    void disconnected();
    void sslErrors(const QList<QSslError> &errors);
private:
    QSslSocket *socket;
    SslConf *sslConf;
    qintptr socketDescriptor;
    HttpHeader *header;
};

#endif // HTTPSCLIENT_H
