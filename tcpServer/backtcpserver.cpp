#include "backtcpserver.h"
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <cryptedtcpsocket.h>

BackTcpServer::BackTcpServer(QSqlDatabase *db, QSettings *settings, QObject *parent) : QTcpServer(parent)
{
    this->db = db;
    this->settings = settings;
}

void BackTcpServer::start()
{
    while (1) {
        QString sHost = settings->value("tcpserver/host", "").toString();
        if(sHost == "")
            host = QHostAddress(QHostAddress::Any);
        else host = QHostAddress(sHost);
        port = settings->value("tcpserver/port", 26667).toUInt();
        if(this->listen(host, port))
            break;
        qCritical() << QString("Unable to start the tcpServer: %1.").arg(this->errorString());
        QThread::sleep(3000);
    }
    qInfo() << " -----------tcpServer started--------------\n" <<
               "\thost: " << host.toString() << "\n" <<
               "\tport: " << port << "\n" <<
               "------------------------------------------";
}

void BackTcpServer::incomingConnection(qintptr socketDescriptor)
{
    CryptedTcpSocket *client = new CryptedTcpSocket(socketDescriptor, db, this);
    connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
    connect(client, &CryptedTcpSocket::disconnect, this, &BackTcpServer::onDisconnect);
    SClients[socketDescriptor] = client;
    client->start();
}

void BackTcpServer::onDisconnect(qintptr ID)
{
    SClients.remove(ID);
}

