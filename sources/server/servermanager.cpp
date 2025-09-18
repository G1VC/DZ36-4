#include "servermanager.h"
#include "Logger.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QThread>
#include <QMutex>

ServerManager::ServerManager(QObject* parent) :
    QObject(parent),
    m_server(new QTcpServer(this)),
    m_logger(Logger::getInstance())
{
    connect(m_server, &QTcpServer::newConnection,
        this, &ServerManager::handleNewConnection);
}

ServerManager::~ServerManager()
{
    stopServer();
}

void ServerManager::startServer(quint16 port)
{
    if (m_server->isListening())
        return;

    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Ne udalos' zapustit' server: " << m_server->errorString();
        return;
    }

    m_logger.log("Server zapushchen na portu " + QString::number(port));
    emit serverStarted(port);
}

void ServerManager::stopServer()
{
    if (!m_server->isListening())
        return;

    m_server->close();
    qDeleteAll(m_clients);
    m_clients.clear();
    m_logger.log("Server ostanovlen");
    emit serverStopped();
}

void ServerManager::handleNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();
    if (!socket)
        return;

    m_clients.insert(socket);
    connect(socket, &QTcpSocket::readyRead,
        this, &ServerManager::readClientData);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
        this, &ServerManager::socketError);
    connect(socket, &QTcpSocket::disconnected,
        this, &ServerManager::socketDisconnected);

    m_logger.log("Novoe podklyuchenie ot " + socket->peerAddress().toString());
    emit clientConnected(socket);
}

void ServerManager::readClientData()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    QByteArray data = socket->readAll();
    QString message = data.constData();
    m_logger.log("Polucheno ot klienta: " + message);

    emit messageReceived(socket, message);
}

void ServerManager::sendMessage(QTcpSocket* socket, const QString& message)
{
    if (!socket || !socket->isWritable())
        return;

    socket->write(message.toUtf8());
    socket->flush();
    m_logger.log("Otpravleno klientu: " + message);
}

void ServerManager::broadcastMessage(const QString& message)
{
    QMutexLocker locker(&m_mutex);
    for (QTcpSocket* socket : m_clients) {
        if (socket->isWritable())
            socket->write(message.toUtf8());
    }
    m_logger.log("Shirokoveshchatel'noe soobshenie: " + message);
}

void ServerManager::socketError(QAbstractSocket::SocketError error)
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    m_logger.log("Oshibka soketa: " + socket->errorString());
    socket->disconnectFromHost();
}

void ServerManager::socketDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    m_clients.remove(socket);
    socket->deleteLater();
    m_logger.log("Klient otkljuchilsja: " + socket->peerAddress().toString());
    emit clientDisconnected(socket);
}
