#ifndef FRONTHTTPSSERVER_H
#define FRONTHTTPSSERVER_H

#include <QObject>
#include <QTcpServer>
#include "database.h"
#include <QSettings>
#include "httpsclient.h"

class FrontHttpsServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit FrontHttpsServer(DataBase *db, QSettings *settings, QObject *parent = 0);
    void start();
private:
    DataBase *db;
    QTcpServer *tcpServer;
    QSettings *settings;
    QHostAddress host;
    quint16 port;
    QMap<int,HttpsClient *> SClients;
protected:
        void incomingConnection(qintptr socketDescriptor);

private slots:
        void onDisconnect(qintptr ID);
public slots:


};

#endif // FRONTHTTPSSERVER_H
