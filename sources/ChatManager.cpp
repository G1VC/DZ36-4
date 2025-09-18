#include "ChatManager.h"
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

ChatManager::ChatManager(QObject* parent) : QObject(parent), isConnected(false) {
    // Initsializatsiya komponentov
    logger = nullptr;
    network = nullptr;
    security = nullptr;
    mainWindow = nullptr;
}

ChatManager::~ChatManager() {
    disconnectFromServer();
}

void ChatManager::setLogger(Logger* log) {
    logger = log;
}

void ChatManager::setNetwork(NetworkManager* net) {
    network = net;
}

void ChatManager::setSecurity(SecurityManager* sec) {
    security = sec;
}

void ChatManager::setMainWindow(MainWindow* window) {
    mainWindow = window;
}

bool ChatManager::connectToServer(const QString& username, const QString& password) {
    if (!security || !network) {
        logger->log("Ne nastroyeny komponenty bezopasnosti ili seti");
        return false;
    }

    if (!security->authenticateUser(username, password)) {
        logger->log("Oshibka autentifikatsii polzovatelya");
        emit connectionStatusChanged(false);
        return false;
    }

    currentUser = username;
    isConnected = network->init();

    if (isConnected) {
        logger->log("Uspeøíîå podklyuchenie polzovatelya " + username);
        emit connectionStatusChanged(true);
        updateUserList();
        return true;
    }

    logger->log("Oshibka podklyucheniya k serveru");
    emit connectionStatusChanged(false);
    return false;
}

void ChatManager::disconnectFromServer() {
    if (isConnected) {
        network->stop();
        isConnected = false;
        connectedUsers.clear();
        emit connectionStatusChanged(false);
        logger->log("Otkluchenie ot servera");
    }
}

bool ChatManager::sendMessage(const QString& recipient, const QString& message) {
    if (!isConnected) {
        logger->log("Nevozmozhno otpravit soobshenie: ne podklyucheno k serveru");
        return false;
    }

    QString formattedMessage = QString("%1 -> %2: %3")
        .arg(currentUser)
        .arg(recipient)
        .arg(message);

    if (!network->sendMessage(formattedMessage)) {
        logger->log("Oshibka pri otpravke soobsheniya");
        return false;
    }

    // Sohranyaem v istoriyu
    {
        QMutexLocker locker(&chatMutex);
        messageHistory[currentUser].append(formattedMessage);
    }

    logger->log("Soobshenie uspeshno otpravleno");
    return true;
}

QStringList ChatManager::getMessageHistory() const {
    QMutexLocker locker(&chatMutex);
    QStringList history;
    for (const auto& messages : messageHistory.values()) {
        history << messages;
    }
    return history;
}

void ChatManager::processIncomingMessage(const QString& message) {
    QMutexLocker locker(&chatMutex);

    // Parsim soobshenie
    int separatorPos = message.indexOf(" -> ");
    if (separatorPos > 0) {
        QString sender = message.left(separatorPos);
        QString rest = message.mid(separatorPos + 4);
        int colonPos = rest.indexOf(':');
        if (colonPos > 0) {
            QString recipient = rest.left(colonPos);
            QString msgText = rest.mid(colonPos + 2);

            // Obnovlyaem istoriyu soobsheniy
            messageHistory[sender].append(message);

            // Proveryaem, dlya tekushchego li polzovatelya soobshenie
            if (recipient == currentUser || recipient == "all") {
                emit newMessageReceived(message);
                logger->log("Polucheno novoe soobshenie: " + message);
            }
        }
    }
}

void ChatManager::updateUserList() {
    QMutexLocker locker(&chatMutex);
    connectedUsers = network->getConnectedUsers();
    emit userListUpdated(connectedUsers);
}

QStringList ChatManager::getConnectedUsers() const {
    QMutexLocker locker(&chatMutex);
    return connectedUsers;
}

bool ChatManager::isUserConnected(const QString& username) const {
    QMutexLocker locker(&chatMutex);
    return connectedUsers.contains(username);
}

void ChatManager::clearMessageHistory() {
    QMutexLocker locker(&chatMutex);
    messageHistory.clear();
    logger->log("Istoriya soobsheniy ochishchena");
}

void ChatManager::saveMessageHistory(const QString& filename) const {
    QMutexLocker locker(&chatMutex);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logger->log("Oshibka sohraneniya istorii soobsheniy");
        return;
    }

    QTextStream out(&file);
    for (const auto& messages : messageHistory) {
        for (const auto& message : messages) {
            out << message << "\n";
        }
    }
    file.close();
}

void ChatManager::loadMessageHistory(const QString& filename) {
    QMutexLocker locker(&chatMutex);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger->log("Oshibka zagruzki istorii soobsheniy");
        return;
    }

    QTextStream in(&file);
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        if (!line.isEmpty()) {
            // Parsim i dobavlyaem v istoriyu
            int separatorPos = line.indexOf(" -> ");
            if (separatorPos > 0) {
                QString sender = line.left(separatorPos);
                messageHistory[sender].append(line);
            }
        }
    }
    file.close();
}

void ChatManager::broadcastMessage(const QString& message) {
    if (isConnected) {
        network->broadcastMessage(message);
        logger->log("Shirokoveshchatelnoe soobshenie otpravleno");
    }
}

void ChatManager::handlePrivateMessage(const QString& sender, const QString& recipient, const QString& message) {
    QString formattedMessage = QString("%1 -> %2: %3")
        .arg(sender)
        .arg(recipient)
        .arg(message);

    if (isConnected) {
        network->sendMessage(formattedMessage);
        logger->log("Privatnoe soobshenie otpravleno");
    }
}

void ChatManager::updateUserStatus(const QString& username, const QString& status) {
    QMutexLocker locker(&chatMutex);
    // Logika obnovleniya statusa polzovatelya
    logger->log("Obnovlen status polzovatelya " + username);
}

void ChatManager::handleUserDisconnection(const QString& username) {
    QMutexLocker locker(&chatMutex);
    connectedUsers.removeAll(username);
    emit userListUpdated(connectedUsers);
    logger->log("Polzovatel " + username + " otklyuchilsya");
}
