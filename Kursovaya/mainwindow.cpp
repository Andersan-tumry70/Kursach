#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "registrationdialog.h"
#include "admin.h"
#include "leader.h"
#include "employee.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_userDatabase(new UserDatabase(this)),
    adminWindow(nullptr),
    leaderWindow(nullptr),
    employeeWindow(nullptr),
    currentUserId(-1)

{
    ui->setupUi(this);
    this->setFixedSize(319, 318);

    this->setWindowTitle("Авторизация");
    setWindowIcon(QIcon(":/icon/icon.png"));

    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    QString login = ui->loginLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль");
        return;
    }

    QRegularExpression regex("[А-Яа-яЁё ]");
    if (regex.match(login).hasMatch() || regex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль не должны содержать русские буквы и пробелы");
        return;
    }

    if (m_userDatabase->authenticateUser(login, password)) {
        QString role = m_userDatabase->getUserRole(login);
        QString name = m_userDatabase->getUserName(login);
        currentUserId = m_userDatabase->getCurrentUserId();

        QMessageBox::information(this, "Здравствуйте", "Здравствуйте, " + name);

        if (role == "Администратор") {
            if (!adminWindow) {
                adminWindow = new Admin(currentUserId);
                adminWindow->setAttribute(Qt::WA_DeleteOnClose);
            }
            adminWindow->show();
            this->hide();
        } else if (role == "Представитель кафедры") {
            if (!leaderWindow) {
                leaderWindow = new Leader();
                leaderWindow->setAttribute(Qt::WA_DeleteOnClose);
            }
            leaderWindow->show();
            this->hide();
        } else {
            if (!employeeWindow) {
                employeeWindow = new Employee(currentUserId);
                employeeWindow->setAttribute(Qt::WA_DeleteOnClose);
            }
            employeeWindow->show();
            this->hide();
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}

void MainWindow::on_RegistrationButton_clicked()
{
    RegistrationDialog *registrationDialog = new RegistrationDialog(this);
    registrationDialog->exec();
}
