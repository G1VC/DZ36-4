#pragma once

#include <QMainWindow>
#include <QTreeView>
#include <QTextEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>
#include <QListWidget>
#include <QTimer>
#include "Logger.h"
#include "UserInfoDialog.h"

namespace Ui {
    class ServerMainWindow;
}

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerMainWindow(QWidget* parent = nullptr);
    ~ServerMainWindow();

public slots:
    void updateUserList();
    void showUserInfo();
    void startServer();
    void stopServer();
    void clearLogs();
    void banUser();
    void unbanUser();
    void kickUser();

private:
    Ui::ServerMainWindow* ui;
    Logger logger;
    QTimer* updateTimer;
    QTreeView* userListView;
    QTextEdit* messageLog;
    QPushButton* startServerButton;
    QPushButton* stopServerButton;
    QPushButton* clearLogsButton;
    QMenuBar* menuBar;
    QStatusBar* statusBar;
    UserInfoDialog* userInfoDialog;

private slots:
    void on_userListView_clicked(const QModelIndex& index);
    void on_actionShow_User_Info_triggered();
    void on_startServerButton_clicked();
    void on_stopServerButton_clicked();
    void on_clearLogsButton_clicked();
};
