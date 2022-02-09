#include "httpsclient.h"
#include <QFile>
#include <QNetworkRequest>
#include <httpheader.h>
#include <apirequest.h>

HttpsClient::HttpsClient(qintptr ID, QSqlDatabase *db, SslConf *conf, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
    this->existingDB = db;
    this->sslConf = conf;
}

void HttpsClient::run()
{
    socket = new QSslSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        socket->deleteLater();
        qWarning() << "error setSocketDescriptor("+QString().number(this->socketDescriptor)+")";
        return;
    }
    //    qDebug() << " Connect client: " << socketDescriptor;
    socket->setSslConfiguration(*sslConf->conf);
    socket->setLocalCertificate(*sslConf->cert);
    socket->setPrivateKey(*sslConf->key);
    socket->setProtocol(QSsl::TlsV1_0OrLater);
    socket->startServerEncryption();
    header = new HttpHeader(HTTP_REQUEST);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    if(!socket->waitForEncrypted(3000)) {
        socket->close();
    }
    exec();
}

void HttpsClient::readyRead()
{
    QByteArray rawData = socket->readAll();
    //    qInfo() << rawData;
    if(!header->parse(rawData)){
        qWarning() << "parse http header error - ("
                   << QString().number(static_cast<int>(header->getError()))
                   << ")" << header->getLastError();
        socket->close();
        return;
    }
    if(header->getState() == HttpHeader::OnMessageComplete){
        if(db == nullptr)
            db = new DataBase(existingDB, QString().number(socketDescriptor) + "-httpsServer");
        ApiRequest apiRq(socket, header, db);
        if(!apiRq.exec()){
            QByteArray response;
            response.append(QString("HTTP/1.1 200 OK\r\n").toLocal8Bit());
            response.append(QString("content-type: text/html\r\n").toLocal8Bit());
            response.append(QString("Server: BackServer\r\n").toLocal8Bit());
            response.append(QString("Content-Length: 3\r\n").toLocal8Bit());
            response.append(QString("\r\n").toLocal8Bit());
            response.append(QString("OK!\r\n").toLocal8Bit());
            socket->write(response);
        }
                qInfo() << " ";
                qInfo() << "----------------------------------------------------------";
                qInfo() << "\tmethod :" << header->getMethod();
                qInfo() << "\thost :" << header->getHost();
                qInfo() << "\tbody :" << header->getBody();
                qInfo() << "\tpath :" << header->getUrlPath();
                qInfo() << "\tAuthorization :" << header->getHeaderValue("Grpc-Metadata-Authorization");
                qInfo() << "\tcontent-type :" << header->getHeaderValue("content-type");
                qInfo() << "\tAccept :" << header->getHeaderValue("Accept");
                qInfo() << "----------------------------------------------------------";
                qInfo() << " ";
        socket->close();
    }
}

void HttpsClient::disconnected()
{
    //    qDebug() << " Disconnect client: " << socketDescriptor;
    if(db != nullptr){
        delete db;
        db = nullptr;
    }
    emit disconnect(socketDescriptor);
    header->deleteLater();
    socket->deleteLater();
    exit(0);
}
