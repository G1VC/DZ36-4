#pragma once

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMap>
#include "Logger.h"
#include "NetworkManager.h"
#include "SecurityManager.h"
#include "MainWindow.h"

class ChatManager : public QObject {
    Q_OBJECT

private:
    Logger* logger;
    NetworkManager* network;
    SecurityManager* security;
    MainWindow* mainWindow;

    QMutex chatMutex;
    QMap<QString, QString> messageHistory;  // Istoriya soobsheniy
    QVector<QString> connectedUsers;        // Spisok podklyuchennyh polzovateley

    QString currentUser;                   // Tekushchiy avtorizovannyy polzovatel
    bool isConnected;                     // Status podklyucheniya

public:
    explicit ChatManager(QObject* parent = nullptr);
    ~ChatManager();

    // Initsializatsiya komponentov
    void setLogger(Logger* log);
    void setNetwork(NetworkManager* net);
    void setSecurity(SecurityManager* sec);
    void setMainWindow(MainWindow* window);

    // Upravlenie podklyucheniem
    bool connectToServer(const QString& username, const QString& password);
    void disconnectFromServer();

    // Rabota s soobsheniyami
    bool sendMessage(const QString& recipient, const QString& message);
    QStringList getMessageHistory() const;

    // Upravlenie polzovatelyami
    QStringList getConnectedUsers() const;
    bool isUserConnected(const QString& username) const;

public slots:
    void processIncomingMessage(const QString& message);
    void updateUserList();

signals:
    void newMessageReceived(const QString& message);
    void userListUpdated(const QStringList& users);
    void connectionStatusChanged(bool status);
};

// Realizatsiya metodov

bool ChatManager::connectToServer(const QString& username, const QString& password) {
    if (!security->authenticateUser(username, password)) {
        logger->log("Oshibka autentifikatsii polzovatelya");
        return false;
    }

    currentUser = username;
    isConnected = network->init();

    if (isConnected) {
        logger->log("Uspeøíîå podklyuchenie polzovatelya " + username);
        emit connectionStatusChanged(true);
    }

    return isConnected;
}

bool ChatManager::sendMessage(const QString& recipient, const QString& message) {
    if (!isConnected) return false;

    QString fullMessage = QString("%1: %2").arg(currentUser).arg(message);
    if (!network->sendMessage(fullMessage)) {
        logger->log("Oshibka otpravki soobsheniya");
        return false;
    }

    messageHistory[recipient].append(fullMessage);
    logger->log("Otpravleno soobshenie polzovatelyu " + recipient);
    return true;
}

void ChatManager::processIncomingMessage(const QString& message) {
    QMutexLocker locker(&chatMutex);

    // Parsim soobshenie
    int colonPos = message.indexOf(':');
    if (colonPos > 0) {
        QString sender = message.left(colonPos);
        QString msgText = message.mid(colonPos + 1);

        // Obnovlyaem istoriyu soobsheniy
        messageHistory[sender].append(msgText);

        // Otpravlyaem signal o novom soobshenii
        emit newMessageReceived(message);

        logger->log("Polucheno soobshenie ot " + sender);
    }
}

QStringList ChatManager::getMessageHistory() const {
    QMutexLocker locker(&chatMutex);
    return messageHistory.values().join('\n').split('\n');
}

QStringList ChatManager::getConnectedUsers() const {
    QMutexLocker locker(&chatMutex);
    return connectedUsers;
}
