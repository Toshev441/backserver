#ifndef BACKTCPSERVER_H
#define BACKTCPSERVER_H

#include <QObject>
#include "cryptedtcpsocket.h"
#include "database.h"
#include <QTcpServer>
#include <QSettings>

class BackTcpServer : public QThread
{
    Q_OBJECT
public:
    explicit BackTcpServer(QSqlDatabase *db, QSettings *settings, QObject *parent = 0);

private:
    void run();
    QSqlDatabase *db;
    QTcpServer *tcpServer;
    QSettings *settings;
    QHostAddress host;
    quint16 port;
    QMap<int,CryptedTcpSocket *> SClients;
private slots:
        void onConnect();
        void onDisconnect(qintptr ID);
};

#endif // BACKTCPSERVER_H
