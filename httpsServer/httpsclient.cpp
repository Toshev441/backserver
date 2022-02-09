#include "httpsclient.h"
#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QNetworkRequest>
#include <httpheader.h>
#include <apirequest.h>

HttpsClient::HttpsClient(qintptr ID, DataBase *db, QSettings *settings, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
    this->db = db;
    this->settings = settings;
}

void HttpsClient::run()
{
    socket = new QSslSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }
//    QSslConfiguration sslConfiguration;
//    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
//    socket->setSslConfiguration(sslConfiguration);

//    QFile certFile(settings->value("httpsserver/cert", "/Users/san/ssl/cert.crt").toString());
//    if(!certFile.open(QIODevice::ReadOnly)){
//        socket->close();
//        qCritical() <<certFile.fileName() << " - " << certFile.errorString();
//        return;
//    }
//    QSslCertificate cert(certFile.readAll());
//    socket->setLocalCertificate(cert);
//    certFile.close();

//    QFile keyFile(settings->value("httpsserver/privkey", "/Users/san/ssl/key.key").toString());
//    if(!keyFile.open(QIODevice::ReadOnly)){
//        socket->close();
//        qCritical() <<keyFile.fileName() << " - " << keyFile.errorString();
//        return;
//    }
//    QSslKey privKey(keyFile.readAll(),  QSsl::Rsa);
//    socket->setPrivateKey(privKey);
//    keyFile.close();
//    socket->setProtocol(QSsl::TlsV1_0OrLater);
//    socket->startServerEncryption();
//    if (socket->waitForEncrypted(3000)) {
        qDebug() << " Connect client: " << socketDescriptor ;
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
        exec();
//    }
}

void HttpsClient::readyRead()
{
//    qInfo() << socket->readAll();
    HttpHeader header(HTTP_REQUEST, socket->readAll());
    ApiRequest apiRq(socket, &header, db);
//    qInfo() << "method :" << header.getMethod();
//    qInfo() << "host :" << header.getHost();
    qInfo() << "body :" << header.getBody();
//    qInfo() << "path :" << header.getUrlPath();
//    qInfo() << header.getHeaderValue("Grpc-Metadata-Authorization");
//    qInfo() << header.getHeaderValue("content-type");
//    qInfo() << header.getHeaderValue("Accept");
    apiRq.exec();
    socket->close();
}

void HttpsClient::disconnected()
{
    qDebug() << " Disconnect client: " << socketDescriptor ;
    emit disconnect(socketDescriptor);
    socket->deleteLater();
    exit(0);
}
