// createdeleteuserdatabase.cpp
#include "createdeleteuserdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QMessageBox>

CreateDeleteUserDataBase::CreateDeleteUserDataBase(QObject *parent) : QObject(parent)
{

}

// Метод проверка на уникальность логина
bool CreateDeleteUserDataBase::isLoginTaken(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM password WHERE login = ?");
    query.addBindValue(login);
    if (!query.exec()) {
        qDebug() << "Ошибка при проверке логина:" << query.lastError().text();
            return true;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true;
    }

    return false;
}

// Метод создания нового пользователя
bool CreateDeleteUserDataBase::createUser(const QString &family, const QString &name, const QString &patronymic,
                                          const QString &role, const QString &degree, const QString &login,
                                          const QString &password)
{
    // Проверяем уникальность логина
    if (isLoginTaken(login)) {
        qDebug() << "Логин уже занят.";
        return false;
    }

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

// Метод удаления пользователя
bool CreateDeleteUserDataBase::deleteUser(const QString &login, const QString &password,
                                          const QString &family, const QString &name, const QString &patronymic, int &f_userId)
{
    QSqlQuery query;

    query.prepare("SELECT id_users FROM password WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            return false;
    }

    if (!query.next()) {
        qDebug() << "Пользователь с таким логином и паролем не найден.";
        return false;
    }

    f_userId = query.value(0).toInt();

    query.prepare("SELECT id FROM users WHERE id = :userId AND family = :family AND name = :name AND patronymic = :patronymic");
    query.bindValue(":userId", f_userId);
    query.bindValue(":family", family);
    query.bindValue(":name", name);
    query.bindValue(":patronymic", patronymic);
    if (!query.exec()) {
        qDebug() << "Ошибка при проверке ФИО пользователя:" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        qDebug() << "Пользователь с таким ФИО не найден.";
        return false;
    }

    query.prepare("SELECT id FROM works WHERE id_user = :userId");
    query.bindValue(":userId", f_userId);
    if (!query.exec()) {
        qDebug() << "Ошибка при получении id работ пользователя:" << query.lastError().text();
        return false;
    }

    QStringList workIds;
    while (query.next()) {
        workIds.append(query.value(0).toString());
    }

    if (!workIds.isEmpty()) {
        QString workIdsString = workIds.join(",");
        query.prepare("DELETE FROM works_comment WHERE id_namework IN (" + workIdsString + ")");
        if (!query.exec()) {
            qDebug() << "Ошибка при удалении комментариев пользователя из таблицы works_comment:" << query.lastError().text();
            return false;
        }
    }

    if (!workIds.isEmpty()) {
        QString workIdsString = workIds.join(",");
        query.prepare("DELETE FROM works_confirmed WHERE id_namework IN (" + workIdsString + ")");
        if (!query.exec()) {
            qDebug() << "Ошибка при удалении подтверждений работы пользователя из таблицы works_confirmed:" << query.lastError().text();
            return false;
        }
    }

    query.prepare("DELETE FROM works WHERE id_user = :userId");
    query.bindValue(":userId", f_userId);
    if (!query.exec()) {
        qDebug() << "Ошибка при удалении работ пользователя из таблицы works:" << query.lastError().text();
        return false;
    }

    query.prepare("DELETE FROM users WHERE id = :userId");
    query.bindValue(":userId", f_userId);
    if (!query.exec()) {
        qDebug() << "Ошибка при удалении пользователя из таблицы users:" << query.lastError().text();
        return false;
    }

    query.prepare("DELETE FROM password WHERE id_users = :userId");
    query.bindValue(":userId", f_userId);
    if (!query.exec()) {
        qDebug() << "Ошибка при удалении записи пользователя из таблицы password:" << query.lastError().text();
        return false;
    }

    return true;
}

// Метод получения id
int CreateDeleteUserDataBase::IddeleteUser(const QString &login, const QString &password, int &f_userId)
{
    QSqlQuery query;

    query.prepare("SELECT id_users FROM password WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            return false;
    }

    if (!query.next()) {
        qDebug() << "Пользователь с таким логином и паролем не найден.";
        return false;
    }

    f_userId = query.value(0).toInt();
    return true;
}
