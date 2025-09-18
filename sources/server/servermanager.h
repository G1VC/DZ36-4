#pragma once
#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSet>
#include <QMutex>
#include <QHostAddress>
#include "Logger.h"

class ServerManager : public QObject
{
    Q_OBJECT

public:
    explicit ServerManager(QObject* parent = nullptr);
    ~ServerManager();

    void startServer(quint16 port);
    void stopServer();

signals:
    void serverStarted(quint16 port);
    void serverStopped();
    void clientConnected(QTcpSocket* socket);
    void clientDisconnected(QTcpSocket* socket);
    void messageReceived(QTcpSocket* socket, const QString& message);

public slots:
    void sendMessage(QTcpSocket* socket, const QString& message);
    void broadcastMessage(const QString& message);

private slots:
    void handleNewConnection();
    void readClientData();
    void socketError(QAbstractSocket::SocketError error);
    void socketDisconnected();

private:
    QTcpServer* m_server;
    QSet<QTcpSocket*> m_clients;
    QMutex m_mutex;
    Logger m_logger;
};

#endif //  SERVERMANAGER_H
