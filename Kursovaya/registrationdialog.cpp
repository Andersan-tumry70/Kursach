#include "registrationdialog.h"
#include "ui_registrationdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

RegistrationDialog::RegistrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistrationDialog),
    m_userDatabase()
{
    ui->setupUi(this);
    this->setFixedSize(316, 354);

    setWindowTitle("Регистрация пользователя");
    setWindowIcon(QIcon(":/icon/icon.png"));

    connect(ui->roleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){

        if (index == ui->roleComboBox->findText("Преподаватель")) {
            ui->degreeComboBox->clear();
            ui->degreeComboBox->addItem("Доцент");
            ui->degreeComboBox->addItem("Профессор");
            ui->degreeComboBox->addItem("Кандидат наук");
            ui->degreeComboBox->addItem("Доктор наук");
        } else if (index == ui->roleComboBox->findText("Учебно-вспомогательный персонал")) {
            ui->degreeComboBox->clear();
            ui->degreeComboBox->addItem("Технический работник");
            ui->degreeComboBox->addItem("Учёный секретарь");
            ui->degreeComboBox->addItem("Научный сотрудник");
            ui->degreeComboBox->addItem("Ведущий научный сотрудник");
        }
    });

    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}

void RegistrationDialog::on_registerButton_clicked()
{
    QString family = ui->familyLineEdit->text().trimmed();
    QString name = ui->nameLineEdit->text().trimmed();
    QString patronymic = ui->patronymicLineEdit->text().trimmed();
    QString role = ui->roleComboBox->currentText();
    QString degree = ui->degreeComboBox->currentText();
    QString login = ui->loginLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (family.isEmpty() || name.isEmpty() || patronymic.isEmpty() || role.isEmpty() || degree.isEmpty() || login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля.");
        return;
    }

    QRegularExpression nameRegex("^[а-яёА-ЯЁ]+$");
    if (!nameRegex.match(family).hasMatch() || !nameRegex.match(name).hasMatch() || !nameRegex.match(patronymic).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "ФИО должны содержать только русские буквы.");
        return;
    }

    QRegularExpression loginPasswordRegex("^[A-Za-z0-9_!@#$%^&*]+$");
    if (!loginPasswordRegex.match(login).hasMatch() || !loginPasswordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль не должны содержать русские буквы и пробелы.");
        return;
    }

    if (login.length() < 5 || password.length() < 5) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль должны быть не менее 5 символов.");
        return;
    }

    family = family.toLower();
    name = name.toLower();
    patronymic = patronymic.toLower();
    family[0] = family[0].toUpper();
    name[0] = name[0].toUpper();
    patronymic[0] = patronymic[0].toUpper();

    if (m_userDatabase.isLoginTaken(login)) {
        QMessageBox::warning(this, "Ошибка", "Данный логин уже занят.");
        return;
    }

    if (m_userDatabase.addUser(family, name, patronymic, role, degree, login, password)) {
        QMessageBox::information(this, "Успех", "Пользователь успешно зарегистрирован!");
        accept();
    } else {
        qDebug() << "Ошибка при регистрации пользователя. Пожалуйста, проверьте данные.";
        QMessageBox::critical(this, "Ошибка", "Не удалось зарегистрировать пользователя. Пожалуйста, проверьте данные.");
    }
}
