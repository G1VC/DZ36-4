#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QMap>
#include <QString>
#include <QDateTime>

class ServerUser;

class ServerUserListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum UserRoles {
        NicknameRole = Qt::UserRole + 1,
        IpAddressRole,
        StatusRole,
        ConnectTimeRole,
        BannedRole
    };

    explicit ServerUserListModel(QObject* parent = nullptr);

    // Bazovye metody modeli
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Upravlenie spiskom polzovatelei
    void addUser(const ServerUser& user);
    void removeUser(const QString& nickname);
    void updateUserStatus(const QString& nickname, bool status);
    void banUser(const QString& nickname);
    void unbanUser(const QString& nickname);

    ServerUser user(int row) const;

signals:
    void userAdded(const ServerUser& user);
    void userRemoved(const QString& nickname);
    void userStatusChanged(const QString& nickname, bool status);
    void userBanned(const QString& nickname);
    void userUnbanned(const QString& nickname);

private:
    QList<ServerUser> m_users;
    QMap<QString, int> m_userIndexMap;
};

class ServerUser {
public:
    ServerUser();
    ServerUser(const QString& nickname, const QString& ipAddress);

    QString nickname() const;
    void setNickname(const QString& nickname);

    QString ipAddress() const;
    void setIpAddress(const QString& ipAddress);

    bool status() const;
    void setStatus(bool status);

    QDateTime connectTime() const;
    void setConnectTime(const QDateTime& time);

    bool isBanned() const;
    void setBanned(bool banned);

private:
    QString m_nickname;
    QString m_ipAddress;
    bool m_status;
    QDateTime m_connectTime;
    bool m_banned;
};

QDataStream& operator<<(QDataStream& out, const ServerUser& user);
QDataStream& operator>>(QDataStream& in, ServerUser& user);
