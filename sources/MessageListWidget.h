#pragma once

#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include "Message.h"

class MessageListWidget : public QListWidget
{
    Q_OBJECT

private:
    QMenu* contextMenu;
    Message currentMessage;

    // Styles for different message types
    QString messageStyle;
    QString systemMessageStyle;
    QString privateMessageStyle;

public:
    explicit MessageListWidget(QWidget* parent = nullptr);
    ~MessageListWidget();

    void addMessage(const Message& message);
    Message getCurrentMessage() const;
    void clearMessages();

signals:
    void messageSelected(const Message& message);
    void contextMenuRequested(const Message& message);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onCopyMessage();
    void onDeleteMessage();
    void onShowDetails();
};

// Implementation of context menu
class MessageListMenu : public QMenu
{
    Q_OBJECT

public:
    explicit MessageListMenu(QWidget* parent = nullptr);

public slots:
    void copyMessage();
    void deleteMessage();
    void showDetails();
};
