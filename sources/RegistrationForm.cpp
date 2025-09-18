#include "RegistrationForm.h"
#include "ui_RegistrationForm.h"
#include <QMessageBox>
#include <QDebug>

// Constructors of validators
EmailValidator::EmailValidator(QObject* parent)
    : QValidator(parent)
{
}

UsernameValidator::UsernameValidator(QObject* parent)
    : QValidator(parent)
{
}

PasswordValidator::PasswordValidator(QObject* parent)
    : QValidator(parent)
{
}

// Main form constructor
RegistrationForm::RegistrationForm(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::RegistrationForm),
    usernameValidator(new UsernameValidator(this)),
    passwordValidator(new PasswordValidator(this)),
    emailValidator(new EmailValidator(this))
{
    ui->setupUi(this);

    // Setting validators
    ui->usernameEdit->setValidator(usernameValidator);
    ui->passwordEdit->setValidator(passwordValidator);
    ui->confirmPasswordEdit->setValidator(passwordValidator);
    ui->emailEdit->setValidator(emailValidator);

    // Connecting signals
    connect(ui->registerButton, &QPushButton::clicked, this, &RegistrationForm::onRegisterClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RegistrationForm::onCancelClicked);
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
    delete usernameValidator;
    delete passwordValidator;
    delete emailValidator;
}

void RegistrationForm::setSecurityManager(SecurityManager* manager)
{
    securityManager = manager;
}

User RegistrationForm::getRegisteredUser() const
{
    return newUser;
}

// Handling registration button click
void RegistrationForm::onRegisterClicked()
{
    if (!validateInput())
        return;

    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text();

    if (securityManager->registerUser(username, password, email)) {
        newUser.setUsername(username);
        newUser.setEmail(email);
        emit registrationCompleted(newUser);
        clearFields();
        accept();
    }
    else {
        showErrorMessage(tr("Registration error. User already exists"));
    }
}

// Handling cancel
void RegistrationForm::onCancelClicked()
{
    reject();
}

// Input validation
bool RegistrationForm::validateInput() const
{
    if (ui->usernameEdit->text().isEmpty() ||
        ui->passwordEdit->text().isEmpty() ||
        ui->confirmPasswordEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty()) {
        showErrorMessage(tr("Please fill in all fields"));
        return false;
    }

    if (ui->passwordEdit->text() != ui->confirmPasswordEdit->text()) {
        showErrorMessage(tr("Passwords do not match"));
        return false;
    }

    return true;
}

// Displaying error message
void RegistrationForm::showErrorMessage(const QString& message)
{
    QMessageBox::warning(this, tr("Error"), message);
}

// Clearing input fields
void RegistrationForm::clearFields()
{
    ui->usernameEdit->clear();
    ui->passwordEdit->clear();
    ui->confirmPasswordEdit->clear();
    ui->emailEdit->clear();
}
