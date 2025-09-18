#pragma once

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include <QVector>
#include "SecurityManager.h"

class User {
private:
    QString m_username;
    QString m_passwordHash;
    QString m_fullName;
    QString m_email;
    QDateTime m_registrationDate;
    QDateTime m_lastActivity;
    QMap<QString, QString> m_userSettings;
    QVector<QString> m_contacts;
    bool m_isOnline;

    mutable QMutex m_mutex;

public:
    // Constructors
    User();
    User(const QString& username, const QString& password, const QString& fullName, const QString& email);

    // Destructor
    ~User();

    // Setters
    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setFullName(const QString& fullName);
    void setEmail(const QString& email);
    void setSetting(const QString& key, const QString& value);
    void addContact(const QString& contact);
    void setOnlineStatus(bool status);

    // Getters
    QString getUsername() const;
    QString getFullName() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;
    QDateTime getLastActivity() const;
    QString getSetting(const QString& key) const;
    QVector<QString> getContacts() const;
    bool isOnline() const;

    // Password verification
    bool verifyPassword(const QString& password) const;

    // Update activity
    void updateLastActivity();

    // Serialization
    QVariantMap toMap() const;
    static User fromMap(const QVariantMap& map);

protected:
    // Password hashing
    QString hashPassword(const QString& password) const;
};

// Methods implementation

User::User()
    : m_registrationDate(QDateTime::currentDateTime())
    , m_lastActivity(QDateTime::currentDateTime())
    , m_isOnline(false)
{
}

User::User(const QString& username, const QString& password, const QString& fullName, const QString& email)
    : m_username(username)
    , m_passwordHash(hashPassword(password))
    , m_fullName(fullName)
    , m_email(email)
    , m_registrationDate(QDateTime::currentDateTime())
    , m_lastActivity(QDateTime::currentDateTime())
    , m_isOnline(false)
{
}

User::~User() = default;

void User::setUsername(const QString& username) {
    QMutexLocker locker(&m_mutex);
    m_username = username;
}

void User::setPassword(const QString& password) {
    QMutexLocker locker(&m_mutex);
    m_passwordHash = hashPassword(password);
}

QString User::hashPassword(const QString& password) const {
    return SecurityManager::hashPassword(password);
}

bool User::verifyPassword(const QString& password) const {
    QMutexLocker locker(&m_mutex);
    return SecurityManager::verifyPassword(m_passwordHash, password);
}

void User::updateLastActivity() {
    QMutexLocker locker(&m_mutex);
    m_lastActivity = QDateTime::currentDateTime();
}

QVariantMap User::toMap() const {
    QMutexLocker locker(&m_mutex);
    QVariantMap map;
    map["username"] = m_username;
    map["passwordHash"] = m_passwordHash;
    map["fullName"] = m_fullName;
    map["email"] = m_email;
    map["registrationDate"] = m_registrationDate.toString();
    map["lastActivity"] = m_lastActivity.toString();
    map["settings"] = QVariantMap(m_userSettings);
    map["contacts"] = m_contacts;
    map["isOnline"] = m_isOnline;
    return map;
}

User User::fromMap(const QVariantMap& map) {
    User user;
    user.m_username = map.value("username").toString();
    user.m_passwordHash = map.value("passwordHash").toString();
    user.m_fullName = map.value("fullName").toString();
    user.m_email = map.value("email").toString();

    // Parsing registration date
    QString registrationStr = map.value("registrationDate").toString();
    user.m_registrationDate = QDateTime::fromString(registrationStr, Qt::ISODate);

    // Parsing last activity date
    QString lastActivityStr = map.value("lastActivity").toString();
    user.m_lastActivity = QDateTime::fromString(lastActivityStr, Qt::ISODate);

    // Loading settings
    QVariantMap settingsMap = map.value("settings").toMap();
    for (auto it = settingsMap.begin(); it != settingsMap.end(); ++it) {
        user.m_userSettings[it.key()] = it.value().toString();
    }

    // Loading contacts
    QVariantList contactsList = map.value("contacts").toList();
    for (auto contact : contactsList) {
        user.m_contacts.append(contact.toString());
    }

    // Online status
    user.m_isOnline = map.value("isOnline").toBool();

    return user;
}

// Getters
QString User::getUsername() const {
    QMutexLocker locker(&m_mutex);
    return m_username;
}

QString User::getFullName() const {
    QMutexLocker locker(&m_mutex);
    return m_fullName;
}

QString User::getEmail() const {
    QMutexLocker(&m_mutex);
    return m_email;
}

QDateTime User::getRegistrationDate() const {
    QMutexLocker locker(&m_mutex);
    return m_registrationDate;
}

QDateTime User::getLastActivity() const {
    QMutexLocker locker(&m_mutex);
    return m_lastActivity;
}

QString User::getSetting(const QString& key) const {
    QMutexLocker locker(&m_mutex);
    return m_userSettings.value(key, QString());
}

QVector<QString> User::getContacts() const {
    QMutexLocker locker(&m_mutex);
    return m_contacts;
}

bool User::isOnline() const {
    QMutexLocker locker(&m_mutex);
    return m_isOnline;
}

// Email validation
bool User::isValidEmail() const {
    QRegExp emailRegExp("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return emailRegExp.exactMatch(m_email);
}

// Password validation
bool User::isValidPassword(const QString& password) const {
    // Minimum length is 6 characters
    return password.length() >= 6;
}
