#include "httpsserver.h"
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QFile>

HttpsServer::HttpsServer(QSettings *settings, QObject *parent) :  QThread(parent)
{
    this->settings = settings;
    connect(&updateSslTimer, SIGNAL(timeout()), this, SLOT(updateSsl()));
    updateSslTimer.start(1000*60*60);
}

void HttpsServer::run()
{
    tcpServer = new QTcpServer();
    while (1) {
        host = QHostAddress(settings->value("httpsserver/host", "127.0.0.1").toString());
        port = settings->value("httpsserver/port", 8080).toUInt();
        if(tcpServer->listen(host, port))
            break;
        qCritical() << QString("Unable to start the httpsServer: %1.").arg(tcpServer->errorString());
        QThread::sleep(3);
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onConnect()), Qt::DirectConnection);
    connect(this, SIGNAL(finished()), tcpServer, SLOT(deleteLater()));
    updateSsl();
    qInfo() << " ----------httpsServer started-------------\n" <<
               "\thost: " << host.toString() << "\n" <<
               "\tport: " << port << "\n" <<
               "\tssl ready: " << (sslReady ? "true":"false") << "\n" <<
               "------------------------------------------";
    exec();
}

void HttpsServer::onConnect()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    qintptr ID = socket->socketDescriptor();
    if(waitSslUpdate(3000))
    {
        HttpsClient *client = new HttpsClient(ID, &sslConf);
        connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
        connect(client, &HttpsClient::disconnect, this, &HttpsServer::onDisconnect, Qt::QueuedConnection);
        clients[ID] = client;
        client->start();
    }
    else
        socket->close();
}

void HttpsServer::onDisconnect(quint64 ID)
{
    clients.remove(ID);
}

bool HttpsServer::waitSslUpdate(uint ms)
{
    uint count = 0;
    while(!sslReady && (count < ms/50)){
        QThread::msleep(50);
        count++;
    }
    return sslReady;
}

void HttpsServer::updateSsl()
{
    sslReady = false;
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    QFile certFile(settings->value("httpsserver/cert", "cert.crt").toString());
    if(!certFile.open(QIODevice::ReadOnly)){
        qCritical() << certFile.fileName() << " - " << certFile.errorString();
        return;
    }
    sslCert = QSslCertificate(certFile.readAll());
    certFile.close();

    QFile keyFile(settings->value("httpsserver/privkey", "key.key").toString());
    if(!keyFile.open(QIODevice::ReadOnly)){
        qCritical() << keyFile.fileName() << " - " << keyFile.errorString();
        return;
    }
    sslKey = QSslKey(keyFile.readAll(),  QSsl::Rsa);
    keyFile.close();
    sslConf.conf = &sslConfiguration;
    sslConf.cert = &sslCert;
    sslConf.key = &sslKey;
    sslReady = true;
}
