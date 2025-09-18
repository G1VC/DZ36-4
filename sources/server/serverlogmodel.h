#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QDateTime>
#include <QString>

class ServerLogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum LogRoles {
        MessageRole = Qt::UserRole + 1,
        TypeRole,
        TimeRole,
        UserRole
    };

    explicit ServerLogModel(QObject* parent = nullptr);

    // Bazovye metody modeli
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Upravlenie logami
    void addLogMessage(const QString& message, const QString& type, const QString& user);
    void clearLogs();
    void filterLogs(const QString& filter);

signals:
    void logAdded(const QString& message);
    void logsCleared();
    void filterChanged(const QString& filter);

private:
    struct LogEntry {
        QString message;
        QString type;
        QDateTime time;
        QString user;
    };

    QList<LogEntry> m_logs;
    QString m_filter;
};
