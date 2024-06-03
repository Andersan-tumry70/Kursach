#include "userdatabase.h"
#include <QSqlQuery>
#include <QDate>
#include <QSqlError>
#include <QDebug>


UserDatabase::UserDatabase(QObject *parent) : QObject(parent)
{

}

// Метод для проверки, занят ли логин
bool UserDatabase::isLoginTaken(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM password WHERE login = ?");
    query.addBindValue(login);
    if (!query.exec()) {
        qDebug() << "Ошибка при проверке логина:" << query.lastError().text();
            return true; // Возвращаем true, чтобы предотвратить регистрацию в случае ошибки
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true; // Логин существует
    }

    return false;
}

// Метод для добавления нового пользователя
bool UserDatabase::addUser(const QString &family, const QString &name, const QString &patronymic,
                           const QString &role, const QString &degree, const QString &login,
                           const QString &password)
{
    if (isLoginTaken(login)) {
        qDebug() << "Логин уже занят.";
        return false;
    }

    // Добавляем данные о пользователе в таблицу users
    QSqlQuery query;
    query.prepare("INSERT INTO users (family, name, patronymic, role, degree, date_registered) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(family);
    query.addBindValue(name);
    query.addBindValue(patronymic);
    query.addBindValue(role);
    query.addBindValue(degree);
    query.addBindValue(QDate::currentDate());
    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении пользователя в таблицу users:" << query.lastError().text();
        return false;
    }

    // Получаем ID нового пользователя
    query.exec("SELECT last_insert_rowid()");
    int userId = 0;
    if (query.next()) {
        userId = query.value(0).toInt();
    }

    query.prepare("INSERT INTO password (id_users, login, password) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(userId);
    query.addBindValue(login);
    query.addBindValue(password);
    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении логина и пароля в таблицу password:" << query.lastError().text();
        return false;
    }

    return true;
}

// Метод для аутентификации пользователя
bool UserDatabase::authenticateUser(const QString &login, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT id_users FROM password WHERE login = ? AND password = ?");
    query.addBindValue(login);
    query.addBindValue(password);
    if (!query.exec()) {
        qDebug() << "Ошибка при аутентификации пользователя:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        m_currentUserId = query.value(0).toInt(); // Сохраняем текущий идентификатор пользователя
        return true;
    }

    return false;
}

// Метод для получения роли пользователя по его логину
QString UserDatabase::getUserRole(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT users.role FROM users JOIN password ON users.id = password.id_users WHERE password.login = ?");
    query.addBindValue(login);
    if (!query.exec()) {
        qDebug() << "Ошибка при получении роли пользователя:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return QString();
}

// Метод для получения имени пользователя по его логину
QString UserDatabase::getUserName(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT users.name FROM users JOIN password ON users.id = password.id_users WHERE password.login = ?");
    query.addBindValue(login);
    if (!query.exec()) {
        qDebug() << "Ошибка при получении имени пользователя:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return QString();
}

// Метод для получения текущего идентификатора пользователя
int UserDatabase::getCurrentUserId() const
{
    return m_currentUserId;
}
