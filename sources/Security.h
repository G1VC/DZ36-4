#pragma once

#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QDebug>
#include <QCryptographicHash>
#include <vector>
#include <mutex>
#include "Logger.h"

class SecurityManager {
private:
    Logger* logger;
    std::mutex mtx;
    std::vector<std::string> registeredUsers;

    // Password hashing
    QString hashPassword(const QString& password) const;

    // Password verification
    bool verifyPassword(const QString& storedHash, const QString& providedPassword) const;

public:
    SecurityManager(Logger* log);
    ~SecurityManager();

    // Registering a new user
    bool registerUser(const QString& username, const QString& password);

    // User authentication
    bool authenticateUser(const QString& username, const QString& password);

    // Checking user existence
    bool userExists(const QString& username) const;

    // Getting the list of registered users
    std::vector<std::string> getRegisteredUsers() const;

    // Resetting passwords (for debugging)
    void resetPasswords();

    // Username validation
    bool isValidUsername(const QString& username) const;
};

// Implementation of methods

QString SecurityManager::hashPassword(const QString& password) const {
    QByteArray salt = QCryptographicHash::hash(QByteArray("salt"), QCryptographicHash::Sha256);
    QByteArray hash = QCryptographicHash::hash(password.toUtf8() + salt, QCryptographicHash::Sha256);
    return hash.toHex();
}

bool SecurityManager::verifyPassword(const QString& storedHash, const QString& providedPassword) const {
    return storedHash == hashPassword(providedPassword);
}

SecurityManager::SecurityManager(Logger* log) : logger(log) {
    registeredUsers = std::vector<std::string>();
}

SecurityManager::~SecurityManager() {
    // Data cleaning
    registeredUsers.clear();
}

bool SecurityManager::registerUser(const QString& username, const QString& password) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!isValidUsername(username)) {
        logger->log("Invalid username during registration");
        return false;
    }

    if (userExists(username)) {
        logger->log("User already exists");
        return false;
    }

    QString hashedPassword = hashPassword(password);
    registeredUsers.push_back(username.toStdString() + ":" + hashedPassword.toStdString());

    logger->log("User successfully registered");
    return true;
}

bool SecurityManager::authenticateUser(const QString& username, const QString& password) {
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& user : registeredUsers) {
        QStringList parts = QString::fromStdString(user).split(':');
        if (parts.size() == 2 && parts[0] == username) {
            return verifyPassword(parts[1], password);
        }
    }

    logger->log("Authentication error");
    return false;
}

bool SecurityManager::userExists(const QString& username) const {
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& user : registeredUsers) {
        QStringList parts = QString::fromStdString(user).split(':');
        if (parts.size() == 2 && parts[0] == username) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> SecurityManager::getRegisteredUsers() const {
    std::lock_guard<std::mutex> lock(mtx);
    return registeredUsers;
}

bool SecurityManager::isValidUsername(const QString& username) const {
    // Simple validation: length 3-20 characters, only letters and numbers
    return username.size() >= 3 && username.size() <= 20 &&
        username.matches(QRegularExpression("[a-zA-Z0-9]+"));
}
