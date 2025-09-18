#include "Message.h"
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include <QCoreApplication>

// Konstruktory
Message::Message()
    : m_timestamp(QDateTime::currentDateTime())
    , m_isRead(false)
    , m_messageType(GroupMessage)
{
    m_messageId = QString::number(QRandomGenerator::global()->generate64());
}

Message::Message(const QString& messageId, User* sender, const QString& content,
    const QString& recipient, MessageType type)
    : m_messageId(messageId)
    , m_sender(sender)
    , m_timestamp(QDateTime::currentDateTime())
    , m_content(content)
    , m_recipient(recipient)
    , m_messageType(type)
    , m_isRead(false)
{
}

// Destruktor
Message::~Message() {
    delete m_sender;
}

// Proverka format message
bool Message::isValidContent() const {
    QRegularExpression regex("^[a-zA-Z0-9\\s!@#$%^&*()_+-=<>?{}[\\]]*$");
    return regex.match(m_content).hasMatch();
}

// Proverka na pustoe soderzhanie
bool Message::isEmpty() const {
    return m_content.isEmpty();
}

// Proverka vremeni zhizni soobsheniya
bool Message::isExpired() const {
    QDateTime now = QDateTime::currentDateTime();
    return now.secsTo(m_timestamp) > 86400; // 1 den
}

// Formatirovanie vremeni
QString Message::getFormattedTimestamp() const {
    return m_timestamp.toString("dd.MM.yyyy HH:mm:ss");
}

// Proverka prav na redaktirovanie
bool Message::canBeDeleted() const {
    QMutexLocker locker(&m_mutex);
    return !m_isRead && QDateTime::currentDateTime().secsTo(m_timestamp) < 300; // 5 minut
}

// Udalenie soobsheniya
void Message::deleteMessage() {
    QMutexLocker locker(&m_mutex);
    if (canBeDeleted()) {
        m_content.clear();
        m_isRead = true;
        emit messageUpdated();
    }
}

// Proverka na sistemnoe soobshenie
bool Message::isSystemMessage() const {
    return m_messageType == SystemMessage;
}

// Proverka na privatnoe soobshenie
bool Message::isPrivateMessage() const {
    return m_messageType == PrivateMessage;
}

// Proverka na gruppovoe soobshenie
bool Message::isGroupMessage() const {
    return m_messageType == GroupMessage;
}

// Formatirovanie soobsheniya dlya vyvoda
QString Message::getFormattedMessage() const {
    QString formatted = m_sender->getUsername() + ": " + m_content;
    if (isPrivateMessage()) {
        formatted = "Privatnoe soobshenie: " + formatted;
    }
    return formatted;
}

// Serializatsiya v JSON
QJsonObject Message::toJson() const {
    QMutexLocker locker(&m_mutex);
    QJsonObject json;
    json["id"] = m_messageId;
    json["sender"] = m_sender->getUsername();
    json["timestamp"] = m_timestamp.toString();
    json["content"] = m_content;
    json["isRead"] = m_isRead;
    json["recipient"] = m_recipient;
    json["type"] = static_cast<int>(m_messageType);
    return json;
}

// Deserializatsiya iz JSON
void Message::fromJson(const QJsonObject& json) {
    QMutexLocker locker(&m_mutex);
    m_messageId = json["id"].toString();
    m_content = json["content"].toString();
    m_recipient = json["recipient"].toString();

        // Vosstanovlenie otpravitelya
    User* sender = new User();
    sender->setUsername(json["sender"].toString());
    m_sender = sender;

    // Vosstanovlenie vremennoi metki
    m_timestamp = QDateTime::fromString(
        json["timestamp"].toString(), Qt::ISODate);

    // Vosstanovlenie tipa soobsheniya
    m_messageType = static_cast<MessageType>(json["type"].toInt());
    m_isRead = json["isRead"].toBool();

    // Proverka validnosti vosstanovlennogo soobsheniya
    if (m_sender && m_sender->getUsername().isEmpty()) {
        delete m_sender;
        m_sender = nullptr;
        qDebug() << "Oshibka vosstanovleniya otpravitelya soobsheniya";
    }

    // Proverka soderzhaniya
    if (m_content.isEmpty()) {
        qDebug() << "Oshibka vosstanovleniya soderzhaniya soobsheniya";
    }
}

// Proverka vozmozhnosti redaktirovaniya
bool Message::canBeEdited() const {
    QMutexLocker locker(&m_mutex);
    return !m_isRead && QDateTime::currentDateTime().secsTo(m_timestamp) < 300;
}

// Redaktirovanie soderzhaniya
void Message::editContent(const QString& newContent) {
    QMutexLocker locker(&m_mutex);
    if (canBeEdited()) {
        m_content = newContent;
        updateTimestamp();
        emit messageUpdated();
    }
    else {
        qDebug() << "Nevozmozhno otredaktirovat soobshenie";
    }
}

// Formatirovanie soobsheniya dlya vyvoda
QString Message::getFormattedMessage() const {
    QMutexLocker locker(&m_mutex);
    QString formatted = m_sender->getUsername() + ": " + m_content;

    if (isPrivateMessage()) {
        formatted = "Privatnoe soobshenie: " + formatted;
    }
    else if (isGroupMessage()) {
        formatted = "Gruppovoe soobshenie: " + formatted;
    }
    else if (isSystemMessage()) {
        formatted = "Sistemnoe soobshenie: " + formatted;
    }

    return formatted;
}

// Proverka na pustoe soderzhanie
bool Message::isEmpty() const {
    QMutexLocker locker(&m_mutex);
    return m_content.isEmpty();
}

// Proverka format soderzhaniya
bool Message::isValidContent() const {
    QRegularExpression regex("^[a-zA-Z0-9\\s!@#$%^&*()_+-=<>?{}\\*]*$");
    return regex.match(m_content).hasMatch();
}

// Poluchenie vremeni v chitaemom formate
QString Message::getReadableTimestamp() const {
    return m_timestamp.toString("dd.MM.yyyy HH:mm:ss");
}

// Proverka na ustarevanie
bool Message::isExpired() const {
    QDateTime now = QDateTime::currentDateTime();
    return now.secsTo(m_timestamp) > 86400; // 1 den
}

// Udalenie soobsheniya
void Message::deleteMessage() {
    QMutexLocker locker(&m_mutex);
    if (canBeDeleted()) {
        m_content.clear();
        m_isRead = true;
        emit messageUpdated();
    }
}

// Proverka prav na udalenie
bool Message::canBeDeleted() const {
    QMutexLocker locker(&m_mutex);
    return !m_isRead && QDateTime::currentDateTime().secsTo(m_timestamp) < 300; // 5 minut
}
