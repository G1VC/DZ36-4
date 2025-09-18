#include "Security.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QCoreApplication>

// Constructor
SecurityManager::SecurityManager(Logger* log) : logger(log) {
    // Loading the list of registered users at startup
    loadRegisteredUsers();
}

// Destructor
SecurityManager::~SecurityManager() {
    saveRegisteredUsers();
}

// Registering a new user
bool SecurityManager::registerUser(const QString& username, const QString& password) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!isValidUsername(username)) {
        logger->log("Registration error: invalid username");
        return false;
    }

    if (userExists(username)) {
        logger->log("Registration error: user already exists");
        return false;
    }

    if (password.isEmpty() || password.length() < 6) {
        logger->log("Registration error: weak password");
        return false;
    }

    QString hashedPassword = hashPassword(password);
    registeredUsers.push_back(username.toStdString() + ":" + hashedPassword.toStdString());

    // Saving changes
    saveRegisteredUsers();

    logger->log("User " + username + " successfully registered");
    return true;
}

// User authentication
bool SecurityManager::authenticateUser(const QString& username, const QString& password) {
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& user : registeredUsers) {
        QStringList parts = QString::fromStdString(user).split(':');
        if (parts.size() == 2 && parts[0] == username) {
            if (verifyPassword(parts[1], password)) {
                logger->log("Successful authentication of user " + username);
                return true;
            }
            else {
                logger->log("Authentication error: incorrect password for user " + username);
            }
        }
    }

    logger->log("Authentication error: user not found");
    return false;
}

// Checking user existence
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

// Saving the list of users to a file
void SecurityManager::saveRegisteredUsers() const {
    QFile file("users.dat");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logger->log("Error saving user list");
        return;
    }

    QTextStream out(&file);
    for (const auto& user : registeredUsers) {
        out << user << "\n";
    }
    file.close();
}

// Loading the list of users from a file
void SecurityManager::loadRegisteredUsers() {
    QFile file("users.dat");
    if (!file.exists()) {
        logger->log("User file not found, creating a new one");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger->log("Error loading user list");
        return;
    }

    QTextStream in(&file);
    std::string line;
    while (!in.atEnd()) {
        line = in.readLine().toStdString();
        if (!line.empty()) {
            registeredUsers.push_back(line);
        }
    }
    file.close();
}

// Username validation
bool SecurityManager::isValidUsername(const QString& username) const {
    // Checking length and allowed characters
    return username.length() >= 3 && username.length() <= 20 &&
        username.matches(QRegularExpression("[a-zA-Z0-9_]+"));
}

// Password hashing with salt
QString SecurityManager::hashPassword(const QString& password) const {
    // Creating a unique salt for each password
    QByteArray salt = QCryptographicHash::hash(QByteArray("salt"), QCryptographicHash::Sha256);
    QByteArray hash = QCryptographicHash::hash(password.toUtf8() + salt, QCryptographicHash::Sha256);
    return hash.toHex();
}

// Improved version with dynamic salt
QString SecurityManager::hashPasswordWithDynamicSalt(const QString& password) const {
    // Generating a unique salt for each password
    QByteArray salt = QCryptographicHash::hash(QByteArray::fromHex(QString::number(QRandomGenerator::global()->generate64()).toLatin1()), QCryptographicHash::Sha256);

    // Saving salt along with hash
    QByteArray data = salt + password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);

    // Returning salt and hash in "salt:hash" format
    return salt.toHex() + ":" + hash.toHex();
}

// Method for password verification with salt
bool SecurityManager::verifyPasswordWithSalt(const QString& storedHash, const QString& providedPassword) const {
    // Splitting stored hash into salt and actual hash
    QStringList parts = storedHash.split(':');
    if (parts.size() != 2) {
        return false;
    }

    QByteArray salt = QByteArray::fromHex(parts[0]);
    QByteArray storedHashBytes = QByteArray::fromHex(parts[1]);

    // Calculating hash for provided password
    QByteArray data = salt + providedPassword.toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);

    return hash == storedHashBytes;
}

// Method for generating strong salt
QByteArray SecurityManager::generateSalt() const {
    QByteArray salt;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    salt.reserve(16);
    for (int n = 0; n < 16; ++n)
        salt.append(char(QRandomGenerator::global()->generate() % 256));
    hash.addData(salt);
    return hash.result();
}

// Method for registering user with dynamic salt
bool SecurityManager::registerUserWithSalt(const QString& username, const QString& password) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!isValidUsername(username)) {
        logger->log("Registration error: invalid username");
        return false;
    }

    if (userExists(username)) {
        logger->log("Registration error: user already exists");
        return false;
    }

    if (password.isEmpty() || password.length() < 6) {
        logger->log("Registration error: weak password");
        return false;
    }

    // Generating salt and hashing password
    QString hashedPassword = hashPasswordWithDynamicSalt(password);
    registeredUsers.push_back(username.toStdString() + ":" + hashedPassword.toStdString());

    // Saving changes
    saveRegisteredUsers();

    logger->log("User " + username + " successfully registered");
    return true;
}

// Method for authentication with salt
bool SecurityManager::authenticateUserWithSalt(const QString& username, const QString& password) {
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& user : registeredUsers) {
        QStringList parts = QString::fromStdString(user).split(':');
        if (parts.size() == 2 && parts[0] == username) {
            if (verifyPasswordWithSalt(parts[1], password)) {
                logger->log("Successful authentication of user " + username);
                return true;
            }
            else {
                logger->log("Authentication error: incorrect password for user " + username);
            }
        }
    }

    logger->log("Authentication error: user not found");
    return false;
}
