// admin.cpp
#include "admin.h"
#include "ui_admin.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QDebug>
#include "createdeleteuserdatabase.h"

Admin::Admin(int userId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Admin)
    , m_userId(userId)

{
    ui->setupUi(this);

    this->setFixedSize(337, 694);

    this->setWindowTitle("Администратор");
    setWindowIcon(QIcon(":/icon/icon.png"));

    // Обработчик события для RoleComboBox
    connect(ui->RoleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        if (index == ui->RoleComboBox->findText("Администратор")) {
            ui->DegreeComboBox->clear();
            ui->DegreeComboBox->addItem("Системный аналитик");
            ui->DegreeComboBox->addItem("Системный администратор");
            ui->DegreeComboBox->addItem("Администратор баз данных");
            ui->DegreeComboBox->addItem("Администратор информационной безопасности");
            ui->DegreeComboBox->addItem("Администратор приложений");
        } else if (index == ui->RoleComboBox->findText("Представитель кафедры")) {
            ui->DegreeComboBox->clear();
            ui->DegreeComboBox->addItem("Почетный доктор наук");
            ui->DegreeComboBox->addItem("Старший преподаватель");
            ui->DegreeComboBox->addItem("Ассистент");
            ui->DegreeComboBox->addItem("Главный научный сотрудник");
            ui->DegreeComboBox->addItem("Почетный профессор");
        } else if (index == ui->RoleComboBox->findText("Преподаватель")) {
            ui->DegreeComboBox->clear();
            ui->DegreeComboBox->addItem("Доцент");
            ui->DegreeComboBox->addItem("Профессор");
            ui->DegreeComboBox->addItem("Кандидат наук");
            ui->DegreeComboBox->addItem("Доктор наук");
        } else if (index == ui->RoleComboBox->findText("Учебно-вспомогательный персонал")) {
            ui->DegreeComboBox->clear();
            ui->DegreeComboBox->addItem("Технический работник");
            ui->DegreeComboBox->addItem("Учёный секретарь");
            ui->DegreeComboBox->addItem("Научный сотрудник");
            ui->DegreeComboBox->addItem("Ведущий научный сотрудник");
        }
    });

    ui->Password_del->setEchoMode(QLineEdit::Password);
    ui->Password->setEchoMode(QLineEdit::Password);
    qDebug() << "Текущий идентификатор пользователя:" << m_userId;
}

Admin::~Admin()
{
    delete ui;
}

// Метод создания нового пользователя
void Admin::on_CreateButton_clicked()
{
    QString family = ui->Family->text();
    QString name = ui->Name->text();
    QString patronymic = ui->Patronymic->text();
    QString role = ui->RoleComboBox->currentText();
    QString degree = ui->DegreeComboBox->currentText();
    QString login = ui->Login->text();
    QString password = ui->Password->text();

    // Проверяем, что все поля заполнены
    if (family.isEmpty() || name.isEmpty() || patronymic.isEmpty() || role.isEmpty() || degree.isEmpty() || login.isEmpty() || password.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Все поля должны быть заполнены.");
        return;
    }

    // Проверка на русские буквы и отсутствие пробелов
    QRegularExpression re("^[А-Яа-яЁё]+$");
    if (!re.match(family).hasMatch() || !re.match(name).hasMatch() || !re.match(patronymic).hasMatch()) {
        QMessageBox::critical(this, "Ошибка", "ФИО должно содержать только русские буквы без пробелов.");
        return;
    }

    // Приведение ФИО к правильному синтаксису
    family[0] = family[0].toUpper();
    family = family.toLower();
    family[0] = family[0].toUpper();

    name[0] = name[0].toUpper();
    name = name.toLower();
    name[0] = name[0].toUpper();

    patronymic[0] = patronymic[0].toUpper();
    patronymic = patronymic.toLower();
    patronymic[0] = patronymic[0].toUpper();

    // Проверка логина и пароля на английские буквы, цифры и спецсимволы (без пробелов)
    QRegularExpression loginPasswordRe("^[a-zA-Z0-9!@#$%^&*()-_+=?/]+$");
    if (!loginPasswordRe.match(login).hasMatch() || !loginPasswordRe.match(password).hasMatch()) {
        QMessageBox::critical(this, "Ошибка", "Логин и пароль могут содержать только английские буквы, цифры и спецсимволы без пробелов.");
        return;
    }

    // Проверяем уникальность логина
    if (m_userDatabase.isLoginTaken(login)) {
        QMessageBox::critical(this, "Ошибка", "Логин уже занят. Пожалуйста, выберите другой логин.");
        return;
    }

    if (login.length() < 5 || password.length() < 5) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль должны быть не менее 5 символов.");
        return;
    }

    if (m_userDatabase.createUser(family, name, patronymic, role, degree, login, password)) {
        QMessageBox::information(this, "Успех", "Пользователь успешно добавлен.");
        ui->Family->clear();
        ui->Name->clear();
        ui->Patronymic->clear();
        ui->Login->clear();
        ui->Password->clear();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка при добавлении пользователя в базу данных.");
    }
}

// Метод удаления пользователя
void Admin::on_DeleteButton_clicked()
{
    QString login = ui->Login_del->text();
    QString password = ui->Password_del->text();
    QString family = ui->Family_del->text().trimmed();
    QString name = ui->Name_del->text().trimmed();
    QString patronymic = ui->Patronymic_del->text().trimmed();

    if (login.isEmpty() || password.isEmpty() || family.isEmpty() || name.isEmpty() || patronymic.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля.");
        return;
    }

    // Приводим ФИО к нужному фрмату (1я буква заглавная, остальные строчные)
    family = family.left(1).toUpper() + family.mid(1).toLower();
    name = name.left(1).toUpper() + name.mid(1).toLower();
    patronymic = patronymic.left(1).toUpper() + patronymic.mid(1).toLower();


    // предупреждающее сообщение о возможных последствиях удаления
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления",
                                  "Вы уверены, что хотите удалить пользователя и его работы?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }



    // Создаем объект класса CreateDeleteUserDataBase
    CreateDeleteUserDataBase database;
    int f_userId = 0;
    if (database.IddeleteUser(login, password, f_userId)) {
        // Проверяем, не пытается ли администратор удалить самого себя
        if (f_userId == m_userId)
        {
            QMessageBox::critical(this, "Ошибка", "Нельзя удалить самого себя.");
            qDebug() << "ID удаялемого пользователя:" << f_userId;
            return;
        }

    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка при удалении пользователя из базы данных, проверьте корректность данных.");
    }

    if (database.deleteUser(login, password, family, name, patronymic, f_userId)) {

        QMessageBox::information(this, "Успех", "Пользователь успешно удален из базы данных.");
        ui->Login_del->clear();
        ui->Password_del->clear();
        ui->Family_del->clear();
        ui->Name_del->clear();
        ui->Patronymic_del->clear();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка при удалении пользователя из базы данных, проверьте корректность данных.");
    }
}

// Метод выхода из системы -> кнопка
void Admin::on_CloseButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Завершение сеанса",
                                  "Вы хотите завершить сеанс?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->close();
        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
    }
}
