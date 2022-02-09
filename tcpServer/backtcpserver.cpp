#include "backtcpserver.h"
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <cryptedtcpsocket.h>

BackTcpServer::BackTcpServer(QSqlDatabase *db, QSettings *settings, QObject *parent) : QThread(parent)
{
    this->db = db;
    this->settings = settings;
}

void BackTcpServer::run()
{
    tcpServer = new QTcpServer();
    while (1) {
        QString sHost = settings->value("tcpserver/host", "").toString();
        if(sHost == "")
            host = QHostAddress(QHostAddress::Any);
        else
            host = QHostAddress(sHost);
        port = settings->value("tcpserver/port", 26667).toUInt();
        if(tcpServer->listen(host, port))
            break;
        qCritical() << QString("Unable to start the tcpServer: %1.").arg(tcpServer->errorString());
        QThread::sleep(3);
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onConnect()));
    connect(this, SIGNAL(finished()), tcpServer, SLOT(deleteLater()));
    qInfo() << " -----------tcpServer started--------------\n" <<
               "\thost: " << host.toString() << "\n" <<
               "\tport: " << port << "\n" <<
               "------------------------------------------";
    exec();
}

void BackTcpServer::onConnect()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    qintptr ID = socket->socketDescriptor();
    CryptedTcpSocket *client = new CryptedTcpSocket(ID, db, this);
    connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
    connect(client, &CryptedTcpSocket::disconnect, this, &BackTcpServer::onDisconnect);
    SClients[ID] = client;
    client->start();
}

void BackTcpServer::onDisconnect(qintptr ID)
{
    SClients.remove(ID);
}

