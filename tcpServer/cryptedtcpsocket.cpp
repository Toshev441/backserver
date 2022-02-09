#include "cryptedtcpsocket.h"
#include <QSqlQuery>

CryptedTcpSocket::CryptedTcpSocket(qintptr ID, DataBase *db, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
    this->db = db;
}

void CryptedTcpSocket::run()
{
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    exec();
}

void CryptedTcpSocket::readyRead()
{
    int count = 0;
     while((socket->bytesAvailable() < JOIN_RQ_PACKET_SIZE ) && (count < JOIN_RQ_WAIT_MS))
     {
         qDebug() << socket->bytesAvailable();
         QThread::msleep(10);
         count++;
     }
     if(count >= JOIN_RQ_WAIT_MS)
     {
         socket->close();
     }
     else
     {
         ID = QString(socket->read(JOIN_RQ_PACKET_SIZE));
         QSqlQuery query(db->db());
         query.prepare(" \
                       SELECT devices.ID AS ID, \
                       applications.ID AS appID, \
                       devices.session AS session, \
                       applications.app_key AS app_key \
                       FROM devices LEFT JOIN applications \
                       ON devices.app_ID = applications.ID \
                 WHERE devices.ID = '"+ID+"';");
         query.exec();
         qDebug() << query.lastQuery();
         if(query.size() != 1){
             socket->close();
             exec();
         }
         knowClient = true;
         query.next();
         QByteArray iv = QByteArray().fromHex(query.value("session").toByteArray());
         QByteArray key = QByteArray().fromHex(query.value("app_key").toByteArray());
         appID = query.value("app_key").toByteArray().toHex();
         AES_init_ctx_iv(&aesCtx, (const uint8_t*)key.constData(), (const uint8_t*)iv.constData());

         qDebug() << socketDescriptor << "Client connected ID =" << ID << " appID =" << appID;
     }
    qint64 packets = socket->size()/AES_BLOCKLEN;
    if(packets > 0){
        size_t  size = packets*AES_BLOCKLEN;
        uint8_t* data = (uint8_t*)malloc(size);
        memcpy(data, socket->read(packets*AES_BLOCKLEN).constData(), size);
        AES_CBC_decrypt_buffer(&aesCtx, data, (size_t)(packets*AES_BLOCKLEN));
    }
}

void CryptedTcpSocket::disconnected()
{
    if(knowClient){
        QString session = QByteArray((char*)aesCtx.Iv, sizeof(aesCtx.Iv)).toBase64();
        QSqlQuery query(db->db());
        query.prepare("UPDATE devices SET session = '?' WHERE ID = '?';");
        query.bindValue(1, session);
        query.bindValue(2, ID);
        query.exec();
        qDebug() << socketDescriptor << " Disconnect client: " << ID;
    }
    emit disconnect(socketDescriptor);
    socket->deleteLater();
    exit(0);
}
