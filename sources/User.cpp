#include "User.h"
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include <QCoreApplication>

// Constructors
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
    if (!isValidEmail()) {
        qDebug() << "Invalid email when creating user";
    }
}

// Destructor
User::~User() = default;

// Setters
void User::setUsername(const QString& username) {
    QMutexLocker locker(&m_mutex);
    if (username.isEmpty()) {
        qDebug() << "Error: empty username";
        return;
    }
    m_username = username;
}

void User::setPassword(const QString& password) {
    QMutexLocker locker(&m_mutex);
    if (!isValidPassword(password)) {
        qDebug() << "Password does not meet security requirements";
        return;
    }
    m_passwordHash = hashPassword(password);
}

void User::setFullName(const QString& fullName) {
    QMutexLocker locker(&m_mutex);
    m_fullName = fullName;
}

void User::setEmail(const QString& email) {
    QMutexLocker locker(&m_mutex);
    if (!isValidEmail(email)) {
        qDebug() << "Invalid email";
        return;
    }
    m_email = email;
}

void User::setSetting(const QString& key, const QString& value) {
    QMutexLocker locker(&m_mutex);
    m_userSettings[key] = value;
}

void User::addContact(const QString& contact) {
    QMutexLocker locker(&m_mutex);
    if (!m_contacts.contains(contact)) {
        m_contacts.append(contact);
    }
}

void User::setOnlineStatus(bool status) {
    QMutexLocker locker(&m_mutex);
    m_isOnline = status;
    updateLastActivity();
}

// Password hashing
QString User::hashPassword(const QString& password) const {
    return SecurityManager::hashPassword(password);
}

// Password verification
bool User::verifyPassword(const QString& password) const {
    QMutexLocker locker(&m_mutex);
    return SecurityManager::verifyPassword(m_passwordHash, password);
}

// Email validation
bool User::isValidEmail(const QString& email) const {
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return emailRegex.match(email).hasMatch();
}

// Password validation
bool User::isValidPassword(const QString& password) const {
    return !password.isEmpty() && password.length() >= 6;
}

// Update activity
void User::updateLastActivity() {
    QMutexLocker locker(&m_mutex);
    m_lastActivity = QDateTime::currentDateTime();
}

// Serialization
QVariantMap User::toMap() const {
    QMutexLocker locker(&m_mutex);
    QVariantMap map;
    map["username"] = m_username;
    map["passwordHash"] = m_passwordHash;
    map["fullName"] = m_fullName;
    map["email"] = m_email;
    map["registrationDate"] = m_registrationDate.toString();
    map["lastActivity"] = m_lastActivity.toString();
    // Continuation of the toMap() method

    map["settings"] = QVariantMap(m_userSettings);
    map["contacts"] = m_contacts;
    map["isOnline"] = m_isOnline;
    return map;
}

// Deserialization from QVariantMap
User User::fromMap(const QVariantMap& map) {
    User user;
    user.m_username = map.value("username").toString();
    user.m_passwordHash = map.value("passwordHash").toString();
    user.m_fullName = map.value("fullName").toString();
    user.m_email = map.value("email").toString();

    // Parsing dates
    user.m_registrationDate = QDateTime::fromString(
        map.value("registrationDate").toString(), Qt::ISODate);
    user.m_lastActivity = QDateTime::fromString(
        map.value("lastActivity").toString(), Qt::ISODate);

    // Restoring settings
    QVariantMap settings = map.value("settings").toMap();
    for (const auto& key : settings.keys()) {
        user.m_userSettings[key] = settings[key].toString();
    }

    // Restoring contacts
    QVariantList contacts = map.value("contacts").toList();
    for (const auto& contact : contacts) {
        user.m_contacts << contact.toString();
    }

    user.m_isOnline = map.value("isOnline").toBool();
    return user;
}

// Saving the user to a file
bool User::saveToFile(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error saving user to file";
        return false;
    }

    QDataStream out(&file);
    out << toMap();
    file.close();
    return true;
}

// Loading the user from a file
bool User::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error loading user from file";
        return false;
    }

    QDataStream in(&file);
    QVariantMap data;
    in >> data;
    *this = fromMap(data);
    file.close();
    return true;
}

// Checking contact existence
bool User::hasContact(const QString& contact) const {
    QMutexLocker locker(&m_mutex);
    return m_contacts.contains(contact);
}

// Removing a contact
void User::removeContact(const QString& contact) {
    QMutexLocker locker(&m_mutex);
    m_contacts.removeAll(contact);
}

// Clearing all settings
void User::clearSettings() {
    QMutexLocker locker(&m_mutex);
    m_userSettings.clear();
}

// Clearing the contact list
void User::clearContacts() {
    QMutexLocker locker(&m_mutex);
    m_contacts.clear();
}
