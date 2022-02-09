#include "cryptedtcpsocket.h"
#include <QSqlQuery>
#include <QJsonArray>
#include <QJsonObject>
#include "integrations.h"

CryptedTcpSocket::CryptedTcpSocket(qintptr ID, QSqlDatabase *db, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
    this->existingDB = db;
}

void CryptedTcpSocket::run()
{
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }
    qInfo() << "connect tcp client: " << this->socketDescriptor;
    db = new DataBase(existingDB, QString().number(socketDescriptor) + "-tcpServer");
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    exec();
}

void CryptedTcpSocket::readyRead()
{
//    qInfo() << socket->readAll();
//    return;
    if(!knowClient){
        int count = 0;
        while((socket->bytesAvailable() < JOIN_RQ_PACKET_SIZE ) && (count < JOIN_RQ_WAIT_MS))
        {
//            qDebug() << socket->bytesAvailable();
            QThread::msleep(10);
            count++;
        }
        if(count >= JOIN_RQ_WAIT_MS)
        {
            socket->close();
        }
        else
        {
            devEUI = QString(socket->read(JOIN_RQ_PACKET_SIZE));
            QString queryString = " \
                       SELECT devices.EUI AS ID, \
                    applications.GUID AS appID, \
                    devices.session AS session, \
                    applications.app_key AS app_key \
                    FROM devices LEFT JOIN applications \
                    ON devices.app_GUID = applications.GUID \
                    WHERE devices.EUI = '"+devEUI+"';";

            QJsonArray arr = db->exec(queryString);
            if(arr.count() == 0){
                db->checkError(true);
                socket->close();
                return;
            }
            knowClient = true;
            queryString = "SELECT ";
            QByteArray iv = QByteArray().fromHex(arr[0].toObject()["session"].toString().toLocal8Bit());
            appKey = arr[0].toObject()["app_key"].toString();
            QByteArray key = QByteArray().fromHex(appKey.toLocal8Bit());
            integration = arr[0].toObject()["integration"].toString();
            integrationType = arr[0].toObject()["integrationType"].toString();
            AES_init_ctx_iv(&aesCtx, (const uint8_t*)key.constData(), (const uint8_t*)iv.constData());
            QByteArray data = "JOINED\00000000000000000000000000000000000000000000000000";
            AES_CBC_encrypt_buffer(&aesCtx, (uint8_t*)data.constData(), AES_BLOCKLEN);
            socket->write(data.constData(), AES_BLOCKLEN);
            qDebug() << socketDescriptor << "Client connected ID = " << devEUI << ", appID = " << appKey << ", session = " << iv;
        }
    }
    qint64 packets = socket->size()/AES_BLOCKLEN;
    if(packets > 0){
        size_t  size = packets*AES_BLOCKLEN;
        uint8_t* data = (uint8_t*)malloc(size);
        memcpy(data, socket->read(packets*AES_BLOCKLEN).constData(), size);
        qDebug() << "raw:"<< QByteArray((const char*)data, packets*AES_BLOCKLEN);
        AES_CBC_decrypt_buffer(&aesCtx, data, (size_t)(packets*AES_BLOCKLEN));
        qDebug() << "dec:" << QByteArray((const char*)data, packets*AES_BLOCKLEN);
    }
}

void CryptedTcpSocket::disconnected()
{
    if(knowClient){
        QString session = QByteArray((char*)aesCtx.Iv, sizeof(aesCtx.Iv)).toBase64();
        QSqlQuery query(*existingDB);
        query.prepare("UPDATE devices SET session = '?' WHERE ID = '?';");
        query.bindValue(1, session);
        query.bindValue(2, devEUI);
        query.exec();

    }
    qDebug() << socketDescriptor << " Disconnect client: " << devEUI;
    delete db;
    emit disconnect(socketDescriptor);
    socket->deleteLater();
    exit(0);
}
