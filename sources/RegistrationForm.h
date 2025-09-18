#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QValidator>
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "User.h"
#include "MainLoginForm.h"

namespace Ui {
    class RegistrationForm;
}

class RegistrationForm : public QDialog
{
    Q_OBJECT

private:
    Ui::RegistrationForm* ui;
    SecurityManager* securityManager;
    User newUser;

    // Validators
    QValidator* usernameValidator;
    QValidator* passwordValidator;
    QValidator* emailValidator;

public:
    explicit RegistrationForm(QWidget* parent = nullptr);
    ~RegistrationForm();

    void setSecurityManager(SecurityManager* manager);
    User getRegisteredUser() const;

signals:
    void registrationCompleted(const User& user);

private slots:
    void onRegisterClicked();
    void onCancelClicked();

private:
    bool validateInput() const;
    void showErrorMessage(const QString& message);
    void clearFields();
};

// Additional validation classes

class EmailValidator : public QValidator {
    Q_OBJECT
public:
    explicit EmailValidator(QObject* parent = nullptr);

    QValidator::State validate(QString& input, int& pos) const override {
        // Email validation
        QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
        QRegularExpressionMatch match = regex.match(input);

        if (match.hasMatch()) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};

class UsernameValidator : public QValidator {
    Q_OBJECT
public:
    explicit UsernameValidator(QObject* parent = nullptr);

    QValidator::State validate(QString& input, int& pos) const override {
        // Validation rules:
        // - length from 3 to 20 characters
        // - only letters, numbers and symbols _+
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
        // Validation rules:
        // - minimum length of 6 characters
        if (input.length() >= 6) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};
