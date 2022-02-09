#ifndef BACKTCPSERVER_H
#define BACKTCPSERVER_H

#include <QObject>
#include "cryptedtcpsocket.h"
#include "database.h"
#include <QTcpServer>
#include <QSettings>



class BackTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    BackTcpServer(DataBase *db, QSettings *settings, QObject *parent = 0);
    void start();
private:
    DataBase *db;
    QTcpServer *tcpServer;
    QSettings *settings;
    QHostAddress host;
    quint16 port;
    QMap<int,CryptedTcpSocket *> SClients;
protected:
        void incomingConnection(qintptr socketDescriptor);

private slots:
        void onDisconnect(qintptr ID);
public slots:

};

#endif // BACKTCPSERVER_H
