#ifndef HTTPSSERVER_H
#define HTTPSSERVER_H

#include <QObject>
#include <QTcpServer>
#include "database.h"
#include <QSettings>
#include "httpsclient.h"
#include "QTimer"

class HttpsServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HttpsServer(QSqlDatabase *db, QSettings *settings, QObject *parent = 0);
    void start();
private:
    QSqlDatabase *db;
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
protected:
        void incomingConnection(qintptr socketDescriptor);

private slots:
        void onDisconnect(quint64 ID);
        void updateSsl();
public slots:


};

#endif // HTTPSSERVER_H
