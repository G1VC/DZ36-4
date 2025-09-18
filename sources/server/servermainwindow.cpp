#include "servermainwindow.h"
#include "ui_servermainwindow.h"
#include "Logger.h"
#include "UserInfoDialog.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDateTime>

ServerMainWindow::ServerMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMainWindow)
    , logger(Logger())
    , updateTimer(new QTimer(this))
    , userInfoDialog(new UserInfoDialog(this))
{
    ui->setupUi(this);

    // Nastroyka taymera obnovleniya
    connect(updateTimer, &QTimer::timeout, this, &ServerMainWindow::updateUserList);
    updateTimer->start(1000);

    // Podklyuchenie signalov
    connect(ui->startServerButton, &QPushButton::clicked, this, &ServerMainWindow::startServer);
    connect(ui->stopServerButton, &QPushButton::clicked, this, &ServerMainWindow::stopServer);
    connect(ui->clearLogsButton, &QPushButton::clicked, this, &ServerMainWindow::clearLogs);
    connect(ui->actionShow_User_Info, &QAction::triggered, this, &ServerMainWindow::showUserInfo);

    // Initsializatsiya logov
    logger.log("Server zapushchen");
}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
    delete updateTimer;
    delete userInfoDialog;
}

void ServerMainWindow::updateUserList()
{
    // Zdes dolzhna byt realizatsiya obnovleniya spiska polzovateley
    // Primer:
    // ui->userListView->setModel(serverModel);
}

void ServerMainWindow::showUserInfo()
{
    if (ui->userListView->selectedIndexes().isEmpty()) {
        QMessageBox::warning(this, "Oshibka", "Vyberite polzovatelya");
        return;
    }

    userInfoDialog->show();
}

void ServerMainWindow::startServer()
{
    // Realizatsiya zapuska servera
    logger.log("Server zapushchen administratorem");
    ui->startServerButton->setEnabled(false);
    ui->stopServerButton->setEnabled(true);
}

void ServerMainWindow::stopServer()
{
    // Realizatsiya ostanovki servera
    logger.log("Server ostanovlen administratorem");
    ui->startServerButton->setEnabled(true);
    ui->stopServerButton->setEnabled(false);
}

void ServerMainWindow::clearLogs()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Podtverzhdenie",
        "Vy uvereny, chto hotite ochistit' logi?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Ochistka logov
        QFile logFile("logs/chat_log.txt");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            logFile.close();
            logger.log("Logi ochishcheny administratorem");
            ui->messageLog->clear();
        }
    }
}

void ServerMainWindow::on_userListView_clicked(const QModelIndex& index)
{
    // Obrabotka klika po polzovatelyu
    // Zdes' mozhno dobavit' dopolnitel'nuyu logiku
}

void ServerMainWindow::on_startServerButton_clicked()
{
    startServer();
}

void ServerMainWindow::on_stopServerButton_clicked()
{
    stopServer();
}

void ServerMainWindow::on_clearLogsButton_clicked()
{
    clearLogs();
}

void ServerMainWindow::banUser()
{
    // Realizatsiya bana polzovatelya
    logger.log("Polzovatel' zabanen administratorem");
}

void ServerMainWindow::unbanUser()
{
    // Realizatsiya razbana polzovatelya
    logger.log("Ban polzovatelya snyat administratorem");
}

void ServerMainWindow::kickUser()
{
    // Realization otklyucheniya polzovatelya

    logger.log("Пользователь отключен администратором");
}
