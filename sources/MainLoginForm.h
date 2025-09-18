#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QValidator>
#include <QMessageBox>
#include "User.h"
#include "SecurityManager.h"

namespace Ui {
    class MainLoginForm;
}

class MainLoginForm : public QWidget
{
    Q_OBJECT

private:
    Ui::MainLoginForm* ui;
    SecurityManager* securityManager;
    User currentUser;

    // Validatory
    QValidator* usernameValidator;
    QValidator* passwordValidator;

public:
    explicit MainLoginForm(QWidget* parent = nullptr);
    ~MainLoginForm();

    void setSecurityManager(SecurityManager* manager);
    User getCurrentUser() const;

signals:
    void loginSuccessful(const User& user);
    void registrationSuccessful(const User& user);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onCancelClicked();

private:
    bool validateInput() const;
    void showErrorMessage(const QString& message);
    void clearFields();
};

// Dopolnitelnye klassy validatsii

class UsernameValidator : public QValidator {
    Q_OBJECT
public:
    explicit UsernameValidator(QObject* parent = nullptr);

    QValidator::State validate(QString& input, int& pos) const override {
        // Pravila validatsii:
        // - dlina ot 3 do 20 simvolov
        // - tolko bukvy, tsifry i simvoly _+
        QRegularExpression regex("^[a-zA-Z0-9_+]{3,20}$");
        QRegularExpressionMatch match = regex.match(input);

        if (match.hasMatch()) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};

class PasswordValidator : public QValidator {
    Q_OBJECT
public:
    explicit PasswordValidator(QObject* parent = nullptr);

    QValidator::State validate(QString& input, int& pos) const override {
        // Pravila validatsii:
        // - dlina minimum 6 simvolov
        if (input.length() >= 6) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};
