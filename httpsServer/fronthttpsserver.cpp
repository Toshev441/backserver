#include "fronthttpsserver.h"
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>


FrontHttpsServer::FrontHttpsServer(DataBase *db, QSettings *settings, QObject *parent) : QTcpServer(parent)
{
    this->db = db;
    this->settings = settings;
}

void FrontHttpsServer::start()
{
    while (1) {
        host = QHostAddress(settings->value("httpsserver/host", "127.0.0.1").toString());
        port = settings->value("httpsserver/port", 8080).toUInt();
        if(this->listen(host, port))
            break;
        qCritical() << QString("Unable to start the httpsServer: %1.").arg(tcpServer->errorString());
        QThread::sleep(3000);
    }
    qInfo() << "----------httpsServer started-------------";
}

void FrontHttpsServer::incomingConnection(qintptr socketDescriptor)
{
    HttpsClient *client = new HttpsClient(socketDescriptor, db, settings, this);
    connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
    connect(client, SIGNAL(disconnect(qintptr)), this, SLOT(onDisconnect(qintptr)));
    SClients[socketDescriptor] = client;
    client->start();
}

void FrontHttpsServer::onDisconnect(qintptr ID)
{
    SClients.remove(ID);
}
