#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include "ChatManager.h"
#include "User.h"
#include "Message.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    ChatManager* chatManager;
    User currentUser;
    QTextEdit* messageList;
    QTextEdit* messageInput;
    QPushButton* sendButton;
    QListWidget* userList;
    QTimer* updateTimer;
    QString currentRecipient;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setChatManager(ChatManager* manager);
    void setCurrentUser(const User& user);
    User getCurrentUser() const;
    void updateUserList();
    void appendMessage(const Message& message);
    void clearMessages();

public slots:
    void onSendMessage();
    void onUserSelected(const QModelIndex& index);
    void updateMessages();
    void handleNewMessage(const QString& message);
    void handleUserListUpdated(const QStringList& users);
    void handleConnectionStatusChanged(bool status);

private slots:
    void onSendButtonClicked();
    void onMessageInputReturnPressed();
    void onUserListItemClicked(QListWidgetItem* item);
};

// Dopolnitelnye klassy interfeysa

class MessageListItem : public QListWidgetItem
{
public:
    MessageListItem(const Message& message, QListWidget* parent = nullptr)
        : QListWidgetItem(parent)
    {
        setText(message.getFormattedMessage());
    }
};

class UserListItem : public QListWidgetItem
{
public:
    UserListItem(const QString& username, bool isOnline, QListWidget* parent = nullptr)
        : QListWidgetItem(parent)
    {
        setText(username);
        if (isOnline) {
            setForeground(Qt::green);
        }
        else {
            setForeground(Qt::gray);
        }
    }
};

// Signaly i sloty

signals:
    void messageSent(const QString& message);
    void userSelected(const QString& username);
    void connectionStatusChanged(bool status);
