#ifndef HTTPSSERVER_H
#define HTTPSSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSettings>
#include "httpsclient.h"
#include "QTimer"

class HttpsServer : public QThread
{
    Q_OBJECT
public:
    explicit HttpsServer(QSettings *settings, QObject *parent = 0);
private:
    void run();
    QTcpServer *tcpServer;
    QSettings *settings;
    QHostAddress host;
    quint16 port;
    QMap<int,HttpsClient *> clients;
    SslConf sslConf;
    QSslConfiguration sslConfiguration;
    QSslCertificate sslCert;
    QSslKey sslKey;
    bool sslReady = false;
    QTimer updateSslTimer;
    bool waitSslUpdate(uint ms);

private slots:
    void onConnect();
    void onDisconnect(quint64 ID);
    void updateSsl();
};

#endif // HTTPSSERVER_H
