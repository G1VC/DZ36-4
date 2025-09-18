#pragma once

#include <QString>
#include <QDateTime>
#include <QObject>
#include "User.h"

class Message : public QObject {
private:
    QString m_messageId;
    User* m_sender;
    QDateTime m_timestamp;
    QString m_content;
    bool m_isRead;
    QString m_recipient;
    QString m_messageType; // "private" or "group"

    mutable QMutex m_mutex;

public:
    enum MessageType {
        PrivateMessage,
        GroupMessage,
        SystemMessage
    };

    // Konstruktory
    Message();
    Message(const QString& messageId, User* sender, const QString& content,
        const QString& recipient = "", MessageType type = GroupMessage);

    // Destruktor
    ~Message();

    // Settery
    void setContent(const QString& content);
    void setRecipient(const QString& recipient);
    void markAsRead();
    void setMessageType(MessageType type);

    // Gettery
    QString getId() const;
    User* getSender() const;
    QDateTime getTimestamp() const;
    QString getContent() const;
    bool isRead() const;
    QString getRecipient() const;
    MessageType getMessageType() const;

    // Serializatsiya
    QVariantMap toMap() const;
    static Message fromMap(const QVariantMap& map);

signals:
    void messageUpdated();

protected:
    void updateTimestamp();
};

// Realizatsiya metodov

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

Message::~Message() = default;

void Message::setContent(const QString& content) {
    QMutexLocker locker(&m_mutex);
    m_content = content;
    emit messageUpdated();
}

void Message::setRecipient(const QString& recipient) {
    QMutexLocker locker(&m_mutex);
    m_recipient = recipient;
}

void Message::markAsRead() {
    QMutexLocker locker(&m_mutex);
    m_isRead = true;
    emit messageUpdated();
}

void Message::setMessageType(MessageType type) {
    QMutexLocker locker(&m_mutex);
    m_messageType = type;
}

QString Message::getId() const {
    QMutexLocker locker(&m_mutex);
    return m_messageId;
}

User* Message::getSender() const {
    QMutexLocker locker(&m_mutex);
    return m_sender;
}

QDateTime Message::getTimestamp() const {
    QMutexLocker locker(&m_mutex);
    return m_timestamp;
}

QString Message::getContent() const {
    QMutexLocker locker(&m_mutex);
    return m_content;
}

bool Message::isRead() const {
    QMutexLocker locker(&m_mutex);
    return m_isRead;
}

QString Message::getRecipient() const {
    QMutexLocker locker(&m_mutex);
    return m_recipient;
}

Message::MessageType Message::getMessageType() const {
    QMutexLocker locker(&m_mutex);
    return m_messageType;
}

void Message::updateTimestamp() {
    QMutexLocker locker(&m_mutex);
    m_timestamp = QDateTime::currentDateTime();
}

QVariantMap Message::toMap() const {
    QMutexLocker locker(&m_mutex);
    QVariantMap map;
    map["id"] = m_messageId;
    map["sender"] = m_sender->getUsername();
    map["timestamp"] = m_timestamp.toString();
    map["content"] = m_content;
    map["isRead"] = m_isRead;
    map["recipient"] = m_recipient;
    map["type"] = m_messageType == PrivateMessage ? "private" :
        (m_messageType == GroupMessage ? "group" : "system");

    return map;
}

Message Message::fromMap(const QVariantMap& map) {
    Message message;
    message.m_messageId = map.value("id").toString();
    message.m_content = map.value("content").toString();
    message.m_recipient = map.value("recipient").toString();

    // Vosstanovlenie otpravitelya
    User* sender = new User();
    sender->setUsername(map.value("sender").toString());
    message.m_sender = sender;

    // Vosstanovlenie vremennoi metki
    message.m_timestamp = QDateTime::fromString(
        map.value("timestamp").toString(), Qt::ISODate);

    // Vosstanovlenie tipa soobsheniya
    QString type = map.value("type").toString();
    if (type == "private") {
        message.m_messageType = PrivateMessage;
    }
    else if (type == "group") {
        message.m_messageType = GroupMessage;
    }
    else {
        message.m_messageType = SystemMessage;
    }

    message.m_isRead = map.value("isRead").toBool();

    return message;
}

// Dopolnitelnye metody

bool Message::isSystemMessage() const {
    return m_messageType == SystemMessage;
}

bool Message::isPrivateMessage() const {
    return m_messageType == PrivateMessage;
}

bool Message::isGroupMessage() const {
    return m_messageType == GroupMessage;
}

void Message::forwardMessage(const QString& newRecipient) {
    QMutexLocker locker(&m_mutex);
    m_recipient = newRecipient;
    updateTimestamp();
}

QString Message::getFormattedContent() const {
    QMutexLocker locker(&m_mutex);
    QString formatted = m_sender->getUsername() + ": " + m_content;
    if (!m_recipient.isEmpty()) {
        formatted = "To " + m_recipient + ": " + formatted;
    }
    return formatted;
}

bool Message::canBeEdited() const {
    QMutexLocker locker(&m_mutex);
    return !m_isRead && QDateTime::currentDateTime().secsTo(m_timestamp) < 60;
}

void Message::editContent(const QString& newContent) {
    if (canBeEdited()) {
        m_content = newContent;
        updateTimestamp();
        emit messageUpdated();
    }
}
