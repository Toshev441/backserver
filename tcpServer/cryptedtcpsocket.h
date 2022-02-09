#ifndef CRYPTEDTCPSOCKET_H
#define CRYPTEDTCPSOCKET_H

#include <QTcpSocket>
#include <QObject>
#include <QThread>

#include "database.h"

#define JOIN_RQ_PACKET_SIZE 64
#define JOIN_RQ_WAIT_MS 60*100

extern "C" {
#include "aes.h"
}

class CryptedTcpSocket : public QThread
{
    Q_OBJECT
public:
    explicit CryptedTcpSocket(qintptr ID, DataBase *db, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void disconnect(qintptr ID);

public slots:
    void readyRead();
    void disconnected();
private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    DataBase *db;
    AES_ctx aesCtx;
    QString ID;
    QString appID;
    bool knowClient = false;
};

#endif // CRYPTEDTCPSOCKET_H
