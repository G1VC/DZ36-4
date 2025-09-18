#include "serveruserlistmodel.h"
#include <QDebug>
#include <QDateTime>

ServerUserListModel::ServerUserListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // Initsializatsiya roley
    m_roleNames[NicknameRole] = "nickname";
    m_roleNames[IpAddressRole] = "ipAddress";
    m_roleNames[StatusRole] = "status";
    m_roleNames[ConnectTimeRole] = "connectTime";
    m_roleNames[BannedRole] = "banned";
}

int ServerUserListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_users.size();
}

QVariant ServerUserListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ServerUser& user = m_users[index.row()];

    switch (role) {
    case NicknameRole:
        return user.nickname();
    case IpAddressRole:
        return user.ipAddress();
    case StatusRole:
        return user.status();
    case ConnectTimeRole:
        return user.connectTime().toString("dd.MM.yyyy hh:mm:ss");
    case BannedRole:
        return user.isBanned();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ServerUserListModel::roleNames() const
{
    return m_roleNames;
}

void ServerUserListModel::addUser(const ServerUser& user)
{
    beginInsertRows(QModelIndex(), m_users.size(), m_users.size());
    m_users.append(user);
    m_userIndexMap[user.nickname()] = m_users.size() - 1;
    endInsertRows();
    emit userAdded(user);
}

void ServerUserListModel::removeUser(const QString& nickname)
{
    int row = m_userIndexMap.value(nickname, -1);
    if (row != -1) {
        beginRemoveRows(QModelIndex(), row, row);
        m_users.removeAt(row);
        m_userIndexMap.remove(nickname);
        endRemoveRows();
        emit userRemoved(nickname);
    }
}

void ServerUserListModel::updateUserStatus(const QString& nickname, bool status)
{
    int row = m_userIndexMap.value(nickname, -1);
    if (row != -1) {
        ServerUser& user = m_users[row];
        if (user.status() != status) {
            user.setStatus(status);
            emit dataChanged(index(row), index(row), { StatusRole });
            emit userStatusChanged(nickname, status);
        }
    }
}

void ServerUserListModel::banUser(const QString& nickname)
{
    int row = m_userIndexMap.value(nickname, -1);
    if (row != -1) {
        ServerUser& user = m_users[row];
        if (!user.isBanned()) {
            user.setBanned(true);
            emit dataChanged(index(row), index(row), { BannedRole });
            emit userBanned(nickname);
        }
    }
}

void ServerUserListModel::unbanUser(const QString& nickname)
{
    int row = m_userIndexMap.value(nickname, -1);
    if (row != -1) {
        ServerUser& user = m_users[row];
        if (user.isBanned()) {
            user.setBanned(false);
            emit dataChanged(index(row), index(row), { BannedRole });
            emit userUnbanned(nickname);
        }
    }
}

ServerUser ServerUserListModel::user(int row) const {
    if (row >= 0 && row < m_users.size())
        return m_users[row];
    return ServerUser();
}

// Realizatsiya klassa ServerUser

ServerUser::ServerUser()
    : m_connectTime(QDateTime::currentDateTime())
    , m_status(false)
    , m_banned(false)
{
}

ServerUser::ServerUser(const QString& nickname, const QString& ipAddress)
    : m_nickname(nickname)
    , m_ipAddress(ipAddress)
    , m_connectTime(QDateTime::currentDateTime())
    , m_status(true)
    , m_banned(false)
{
}

QString ServerUser::nickname() const {
    return m_nickname;
}

void ServerUser::setNickname(const QString& nickname) {
    m_nickname = nickname;
}

QString ServerUser::ipAddress() const {
    return m_ipAddress;
}

void ServerUser::setIpAddress(const QString& ipAddress) {
    m_ipAddress = ipAddress;
}

bool ServerUser::status() const {
    return m_status;
}

void ServerUser::setStatus(bool status) {
    m_status = status;
}

QDateTime ServerUser::connectTime() const {
    return m_connectTime;
}

void ServerUser::setConnectTime(const QDateTime& time) {
    m_connectTime = time;
}

bool ServerUser::isBanned() const {
    return m_banned;
}

void ServerUser::setBanned(bool banned) {
    m_banned = banned;
}

QDataStream& operator<<(QDataStream& out, const ServerUser& user) {
    out << user.m_nickname
        << user.m_ipAddress
        << user.m_status
        << user.m_connectTime
        << user.m_banned;
    return out;
}

QDataStream& operator>>(QDataStream& in, ServerUser& user) {
    in >> user.m_nickname
        >> user.m_ipAddress
        >> user.m_status
        >> user.m_connectTime
        >> user.m_banned;
    return in;
}

// Dopolnitel'nye metody dlya otladki
QString ServerUser::toString() const {
    return QString("Nikneim: %1\nIP: %2\nStatus: %3\nVremya: %4\nZabanen: %5")
        .arg(m_nickname)
        .arg(m_ipAddress)
        .arg(m_status ? "Onlain" : "Oflain")
        .arg(m_connectTime.toString())
        .arg(m_banned ? "Da" : "Net");
}

// Metody dlya sravneniya pol'zovatelei
bool ServerUser::operator==(const ServerUser& other) const {
    return m_nickname == other.m_nickname;
}

bool ServerUser::operator!=(const ServerUser& other) const {
    return !(*this == other);
}
