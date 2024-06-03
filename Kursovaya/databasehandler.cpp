// databasehandler.cpp
#include "databasehandler.h"

DatabaseHandler::DatabaseHandler(QObject *parent) : QObject(parent)
{
}

// Метод для подключения к базе данных
bool DatabaseHandler::connectToDatabase(const QString &dbPath, const QString &dbName)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE"); // Устанавливаем драйвер для работы с базой данных SQLite
    m_db.setDatabaseName(dbPath + dbName); // Соединяем путь и имя файла базы данных

    if (!m_db.open()) { // Пытаемся открыть соединение с базой данных
        qDebug() << "Не удалось подключиться к базе данных";
        return false;
    }

    return true;
}

// Метод для создания таблицы Users
bool DatabaseHandler::createUsersTable()
{
    QSqlQuery queryUsers(m_db);
    if (!queryUsers.exec("CREATE TABLE IF NOT EXISTS Users ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "name TEXT NOT NULL,"
                         "family TEXT NOT NULL,"
                         "patronymic TEXT NOT NULL,"
                         "role TEXT NOT NULL,"
                         "degree TEXT NOT NULL,"
                         "date_registered TEXT NOT NULL"
                         ")")) {
        qDebug() << "Не удалось создать таблицу Users:" << queryUsers.lastError().text();
            return false;
    }

    return true;
}

// Метод для создания таблицы Works
bool DatabaseHandler::createWorksTable()
{
    QSqlQuery queryCheckTable(m_db);
    queryCheckTable.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='works'");
    if (!queryCheckTable.exec() || !queryCheckTable.next()) {
        QSqlQuery queryCreateWorks(m_db);
        if (!queryCreateWorks.exec("CREATE TABLE works ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "namework TEXT,"
                                   "id_user INTEGER,"
                                   "payment REAL,"
                                   "complexity INTEGER,"
                                   "hard INTEGER,"
                                   "FOREIGN KEY (id_user) REFERENCES users(id)"
                                   ")")) {
            qDebug() << "Ошибка при создании таблицы works:" << queryCreateWorks.lastError().text();
                return false;
        }
    } else {
        qDebug() << "Таблица 'works' уже существует";
    }

    return true;
}

// Метод для создания таблицы WorksConfirmed
bool DatabaseHandler::createWorksConfirmedTable()
{
    QSqlQuery queryCheckTable(m_db);
    queryCheckTable.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='works_confirmed'");
    if (!queryCheckTable.exec() || !queryCheckTable.next()) {
        QSqlQuery queryCreateWorksConfirmed(m_db);
        if (!queryCreateWorksConfirmed.exec("CREATE TABLE works_confirmed ("
                                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                            "id_namework INTEGER,"
                                            "confirmed_employee TEXT,"
                                            "confirmed_leader TEXT,"
                                            "FOREIGN KEY (id_namework) REFERENCES works(id)"
                                            ")")) {
            qDebug() << "Ошибка при создании таблицы works_confirmed:" << queryCreateWorksConfirmed.lastError().text();
                return false;
        }
    } else {
        qDebug() << "Таблица 'works_confirmed' уже существует";
    }

    return true;
}

// Метод для создания таблицы WorksComment
bool DatabaseHandler::createWorksCommentTable()
{
    QSqlQuery queryCheckTable(m_db);
    queryCheckTable.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='works_comment'");
    if (!queryCheckTable.exec() || !queryCheckTable.next()) {
        QSqlQuery queryCreateWorksComment(m_db);
        if (!queryCreateWorksComment.exec("CREATE TABLE works_comment ("
                                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                          "id_namework INTEGER,"
                                          "comment TEXT,"
                                          "comment_employee TEXT,"
                                          "comment_leader TEXT,"
                                          "FOREIGN KEY (id_namework) REFERENCES works(id)"
                                          ")")) {
            qDebug() << "Ошибка при создании таблицы works_comment:" << queryCreateWorksComment.lastError().text();
                return false;
        }
    } else {
        qDebug() << "Таблица 'works_comment' уже существует";
    }

    return true;
}

// Метод для создания таблицы Password
bool DatabaseHandler::createPasswordTable()
{
    QSqlQuery queryCheckTable(m_db);
    queryCheckTable.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='Password'");

    if (!queryCheckTable.exec() || !queryCheckTable.next()) {
        QSqlQuery queryCreatePassword(m_db);
        if (!queryCreatePassword.exec("CREATE TABLE Password ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "login TEXT NOT NULL,"
                                      "password TEXT NOT NULL,"
                                      "id_users INTEGER NOT NULL,"
                                      "FOREIGN KEY (id_users) REFERENCES Users(id)"
                                      ")")) {
            qDebug() << "Ошибка при создании таблицы Password:" << queryCreatePassword.lastError().text();
                return false;
        }
    } else {
        qDebug() << "Таблица Password уже существует";
    }

    return true;
}

// Метод для создания пользователя
bool DatabaseHandler::createInitialData()
{
    QSqlQuery queryCheckTable(m_db);
    queryCheckTable.prepare("SELECT COUNT(*) FROM Users");
    if (!queryCheckTable.exec() || !queryCheckTable.next()) {
        qDebug() << "Ошибка при проверке наличия данных в таблице Users:" << queryCheckTable.lastError().text();
            return false;
    }
    int userCount = queryCheckTable.value(0).toInt();

    if (userCount == 0) {
        QSqlQuery query(m_db);

        if (!query.exec("INSERT INTO Users (name, family, patronymic, role, degree, date_registered) "
                        "VALUES ('Андрей', 'Федченко', 'Олегович', 'Администратор', 'Системный аналитик', '2024-01-01')")) {
            qDebug() << "Не удалось вставить пользователя в таблицу Users:" << query.lastError().text();
            return false;
        }

        int userId = query.lastInsertId().toInt();

        if (!query.exec(QString("INSERT INTO Password (login, password, id_users) VALUES ('admin', 'admin', %1)").arg(userId))) {
            qDebug() << "Не удалось вставить запись в таблицу Password:" << query.lastError().text();
            return false;
        }
    }

    return true;
}
