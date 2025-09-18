#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , updateTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Initsializatsiya elementov interfeysa
    messageList = ui->messageList;
    messageInput = ui->messageInput;
    sendButton = ui->sendButton;
    userList = ui->userList;

    // Nastroika taymera obnovleniya
    updateTimer->setInterval(1000);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateMessages);

    // Podklyuchenie signalov
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    connect(messageInput, &QTextEdit::returnPressed, this, &MainWindow::onSendMessage);
    connect(userList, &QListWidget::itemClicked, this, &MainWindow::onUserSelected);

    // Podklyuchenie signalov ot ChatManager
    connect(chatManager, &ChatManager::newMessageReceived,
        this, &MainWindow::handleNewMessage);
    connect(chatManager, &ChatManager::userListUpdated,
        this, &MainWindow::handleUserListUpdated);
    connect(chatManager, &ChatManager::connectionStatusChanged,
        this, &MainWindow::handleConnectionStatusChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete updateTimer;
}

void MainWindow::setChatManager(ChatManager* manager)
{
    chatManager = manager;
    updateTimer->start();
}

void MainWindow::setCurrentUser(const User& user)
{
    currentUser = user;
    ui->statusBar->showMessage(tr("Polzovatel %1 podklyuchen").arg(user.getUsername()));
}

void MainWindow::updateUserList()
{
    userList->clear();
    QStringList users = chatManager->getConnectedUsers();
    for (const QString& user : users) {
        bool isOnline = chatManager->isUserConnected(user);
        new UserListItem(user, isOnline, userList);
    }
}

void MainWindow::appendMessage(const Message& message)
{
    messageList->append(message.getFormattedMessage());
    messageList->ensureCursorVisible();
}

void MainWindow::clearMessages()
{
    messageList->clear();
}

void MainWindow::onSendMessage()
{
    QString messageText = messageInput->toPlainText();
    if (messageText.isEmpty()) {
        return;
    }

    Message message(QString::number(QRandomGenerator::global()->generate64()),
        &currentUser, messageText, currentRecipient);

    if (chatManager->sendMessage(currentRecipient, messageText)) {
        appendMessage(message);
        messageInput->clear();
    }
    else {
        QMessageBox::warning(this, tr("Oshibka"), tr("Ne udalos otpravit soobshenie"));
    }
}

void MainWindow::handleNewMessage(const QString& message)
{
    appendMessage(Message::fromMap(message));
}

void MainWindow::handleUserListUpdated(const QStringList& users)
{
    updateUserList();
}

void MainWindow::handleConnectionStatusChanged(bool status)
{
    if (status) {
        ui->statusBar->showMessage(tr("Podklyuchenie ustanovleno"));
    }
    else {
        ui->statusBar->showMessage(tr("Otklucheno ot servera"));
    }
}

void MainWindow::onUserSelected(const QModelIndex& index)
{
    QListWidgetItem* item = userList->itemFromIndex(index);
    if (item) {
        currentRecipient = item->text();
        ui->statusBar->showMessage(tr("Vybirano poluchatel: %1").arg(currentRecipient));
    }
}

void MainWindow::updateMessages()
{
    QStringList history = chatManager->getMessageHistory();
    for (const QString& msg : history) {
        appendMessage(Message::fromMap(msg));
    }
}
