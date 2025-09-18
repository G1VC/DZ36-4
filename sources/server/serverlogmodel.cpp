#include "serverlogmodel.h"
#include <QDateTime>
#include <QDebug>

ServerLogModel::ServerLogModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // Initsializatsiya roley
    m_roleNames[MessageRole] = "message";
    m_roleNames[TypeRole] = "type";
    m_roleNames[TimeRole] = "time";
    m_roleNames[UserRole] = "user";
}

int ServerLogModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_logs.size();
}

QVariant ServerLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const LogEntry& entry = m_logs[index.row()];

    switch (role) {
    case MessageRole:
        return entry.message;
    case TypeRole:
        return entry.type;
    case TimeRole:
        return entry.time.toString("dd.MM.yyyy hh:mm:ss");
    case UserRole:
        return entry.user;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ServerLogModel::roleNames() const
{
    return m_roleNames;
}

void ServerLogModel::addLogMessage(const QString& message, const QString& type, const QString& user)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    LogEntry newEntry;
    newEntry.message = message;
    newEntry.type = type;
    newEntry.time = QDateTime::currentDateTime();
    newEntry.user = user;

    m_logs.append(newEntry);
    endInsertRows();

    emit logAdded(message);
}

void ServerLogModel::clearLogs()
{
    beginResetModel();
    m_logs.clear();
    endResetModel();

    emit logsCleared();
}

void ServerLogModel::filterLogs(const QString& filter)
{
    m_filter = filter;
    emit filterChanged(filter);
}

// Dopolnitel'nyye metody dlya raboty s fil'tratsiyey
QList<LogEntry> ServerLogModel::filteredLogs() const
{
    QList<LogEntry> filtered;

    for (const LogEntry& entry : m_logs) {
        if (entry.message.contains(m_filter, Qt::CaseInsensitive) ||
            entry.type.contains(m_filter, Qt::CaseInsensitive) ||
            entry.user.contains(m_filter, Qt::CaseInsensitive)) {
            filtered.append(entry);
        }
    }

    return filtered;
}

// Metod dlya sokhraneniya logov v fayl
void ServerLogModel::saveLogsToFile(const QString& fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Oshibka otkrytiya fayla dlya zapisi logov";
        return;
    }

    QTextStream out(&file);

    for (const LogEntry& entry : m_logs) {
        out << entry.time.toString("dd.MM.yyyy hh:mm:ss") << " | "
            << entry.type << " | "
            << entry.user << " | "
            << entry.message << "\n";
    }

    file.close();
}

// Metod dlya zagruzki logov iz fayla
void ServerLogModel::loadLogsFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Oshibka otkrytiya fayla dlya chteniya logov";
        return;
    }

    QTextStream in(&file);
    QString line;

    beginResetModel();
    m_logs.clear();

 

    while (!in.atEnd()) {
        line = in.readLine();
       // Zdes mozhno dobavit parsing stroki i sozdat LogEntry

    }

    endResetModel();
}
