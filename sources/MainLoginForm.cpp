#include "MainLoginForm.h"
#include "ui_MainLoginForm.h"
#include <QMessageBox>
#include <QDebug>

// Konstruktory validatory
UsernameValidator::UsernameValidator(QObject* parent)
    : QValidator(parent)
{
}

PasswordValidator::PasswordValidator(QObject* parent)
    : QValidator(parent)
{
}

// Osnovnoy konstruktor formy
MainLoginForm::MainLoginForm(QWidget* parent)
    : QWidget(parent),
    ui(new Ui::MainLoginForm),
    usernameValidator(new UsernameValidator(this)),
    passwordValidator(new PasswordValidator(this))
{
    ui->setupUi(this);

    // Ustanovka validatory
    ui->usernameEdit->setValidator(usernameValidator);
    ui->passwordEdit->setValidator(passwordValidator);
    ui->registerUsernameEdit->setValidator(usernameValidator);
    ui->registerPasswordEdit->setValidator(passwordValidator);

    // Podklyuchenie signalov
    connect(ui->loginButton, &QPushButton::clicked, this, &MainLoginForm::onLoginClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &MainLoginForm::onRegisterClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainLoginForm::onCancelClicked);
}

MainLoginForm::~MainLoginForm()
{
    delete ui;
    delete usernameValidator;
    delete passwordValidator;
}

void MainLoginForm::setSecurityManager(SecurityManager* manager)
{
    securityManager = manager;
}

User MainLoginForm::getCurrentUser() const
{
    return currentUser;
}

// Obrabotka nazhatiya knopki vhoda
void MainLoginForm::onLoginClicked()
{
    if (!validateInput())
        return;

    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (securityManager->authenticateUser(username, password)) {
        currentUser.setUsername(username);
        emit loginSuccessful(currentUser);
        clearFields();
    }
    else {
        showErrorMessage(tr("Nevernoe imya polzovatelya ili parol"));
    }
}

// Obrabotka nazhatiya knopki registratsii
void MainLoginForm::onRegisterClicked()
{
    if (!validateInput())
        return;

    QString username = ui->registerUsernameEdit->text();
    QString password = ui->registerPasswordEdit->text();

    if (securityManager->registerUser(username, password)) {
        currentUser.setUsername(username);
        emit registrationSuccessful(currentUser);
        clearFields();
    }
    else {
        showErrorMessage(tr("Oshibka registratsii. Polzovatel uzhe suschestvuet"));
    }
}

// Obrabotka otmeny
void MainLoginForm::onCancelClicked()
{
    close();
}

// Validatsiya vhodnykh dannykh
bool MainLoginForm::validateInput() const
{
    if (ui->loginTab->isVisible()) {
        if (ui->usernameEdit->text().isEmpty() || ui->passwordEdit->text().isEmpty()) {
            showErrorMessage(tr("Pozhaluysta, zapolnite vse polya"));
            return false;
        }
    }
    else {
        if (ui->registerUsernameEdit->text().isEmpty() ||
            ui->registerPasswordEdit->text().isEmpty() ||
            ui->confirmPasswordEdit->text().isEmpty()) {
            showErrorMessage(tr("Pozhaluysta, zapolnite vse polya"));
            return false;
        }

        if (ui->registerPasswordEdit->text() != ui->confirmPasswordEdit->text()) {
            showErrorMessage(tr("Paroli ne sovpadayut"));
            return false;
        }
    }

    return true;
}

// Otobrazhenie soobsheniya ob oshibke
void MainLoginForm::showErrorMessage(const QString& message)
{
    QMessageBox::warning(this, tr("Oshibka"), message);
}

// Ochistka poley vvoda

void MainLoginForm::clearFields()
{
    ui->usernameEdit->clear();
    ui->passwordEdit->clear();
    ui->registerUsernameEdit->clear();
    ui->registerPasswordEdit->clear();
    ui->confirmPasswordEdit->clear();
}
