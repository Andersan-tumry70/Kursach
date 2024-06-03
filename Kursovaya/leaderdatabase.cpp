// leaderdatabase.cpp
#include "leaderdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMessageBox>

LeaderDatabase::LeaderDatabase(QObject *parent) : QObject(parent)
{

}

 // Получение информации о пользователях
QVector<UserInfo> LeaderDatabase::getUsersInfo()
{
    QVector<UserInfo> users;

    QSqlQuery query;
    query.prepare("SELECT family, name, patronymic, role, degree, date_registered FROM users");

    if (query.exec()) {
        while (query.next()) {
            UserInfo user;
            user.family = query.value(0).toString();
            user.name = query.value(1).toString();
            user.patronymic = query.value(2).toString();
            user.role = query.value(3).toString();
            user.degree = query.value(4).toString();
            user.dateRegistered = query.value(5).toDate();
            users.append(user);
         }
    } else {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
    }

    return users;
}

// Получение списка пользователей для выбора
QVector<UserInfo> LeaderDatabase::getSelectableUsers()
{
    QVector<UserInfo> users;

    QSqlQuery query;
    query.prepare("SELECT id, family, name, patronymic FROM users WHERE role != 'Администратор' AND role != 'Представитель кафедры'");

    if (query.exec()) {
        while (query.next()) {
            UserInfo user;
            user.id = query.value(0).toInt();
            user.family = query.value(1).toString();
            user.name = query.value(2).toString();
            user.patronymic = query.value(3).toString();
            users.append(user);
        }
    } else {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
    }

    return users;
}


// Метод взаимодействия с базой при отправлении работы
bool LeaderDatabase::addWork(const QString &name, int userId, double payment, int complexity, int hard, const QString &comment)
{
    QSqlQuery query;

    query.prepare("SELECT COUNT(*) FROM works WHERE namework = :name");
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса проверки наличия работы:" << query.lastError().text();
        qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return false;
    }

    query.next();
    if (query.value(0).toInt() > 0) {
        QMessageBox::warning(nullptr, "Ошибка", "Работа с таким наименованием уже существует.");
        return false;
    }

    query.prepare("INSERT INTO works (namework, id_user, payment, complexity, hard) VALUES (:name, :userId, :payment, :complexity, :hard)");
    query.bindValue(":name", name);
    query.bindValue(":userId", userId);
    query.bindValue(":payment", payment);
    query.bindValue(":complexity", complexity);
    query.bindValue(":hard", hard);
    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return false;
    }

    // Получаем ID новой работы
    int workId = query.lastInsertId().toInt();

    query.prepare("INSERT INTO works_confirmed (id_namework) VALUES (:workId)");
    query.bindValue(":workId", workId);
    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return false;
    }

    query.prepare("INSERT INTO works_comment (id_namework, comment) VALUES (:workId, :comment)");
    query.bindValue(":workId", workId);
    query.bindValue(":comment", comment);
    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return false;
    }
    return true;
}

//Метод для получения выполняющихся работ
QVector<QVector<QString>> LeaderDatabase::getOngoingWorks()
{
    QVector<QVector<QString>> works;

    QSqlQuery query;
    query.prepare(R"(
        SELECT w.namework, u.family, u.name, u.patronymic, w.hard, w.complexity, w.payment
        FROM works w
        JOIN users u ON w.id_user = u.id
        LEFT JOIN works_confirmed wc ON w.id = wc.id_namework
        WHERE wc.confirmed_employee IS NULL OR wc.confirmed_employee != '+'
    )");

    if (!query.exec()) {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return works;
    }

    while (query.next()) {
        QVector<QString> work;
        work.append(query.value(0).toString());
        work.append(query.value(1).toString());
        work.append(query.value(2).toString());
        work.append(query.value(3).toString());
        work.append(query.value(4).toString());
        work.append(query.value(5).toString());
        work.append(query.value(6).toString());
        works.append(work);
    }
        return works;
}

// Метод для получения суммы трудоёмкости и сложности пользователя для назначения работы
int LeaderDatabase::calculateTotalComplexityAndHard(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(complexity + hard) FROM works WHERE id_user = :userId");
    query.bindValue(":userId", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return -1;
    }
}

//Метод для получения выполненных работ
QVector<CompletedWork> LeaderDatabase::getCompletedWorks() {
    QVector<CompletedWork> works;

    QSqlQuery query;
    query.prepare(R"(
        SELECT w.id, u.family, u.name, u.patronymic, w.namework
        FROM works w
        JOIN users u ON w.id_user = u.id
        LEFT JOIN works_confirmed wc ON w.id = wc.id_namework
        WHERE wc.confirmed_employee = '+' AND coalesce(wc.confirmed_leader, '') <> '+'
    )");

    if (query.exec()) {
        while (query.next()) {
            CompletedWork work;
            work.id = query.value(0).toInt();
            work.family = query.value(1).toString();
            work.name = query.value(2).toString();
            work.patronymic = query.value(3).toString();
            work.namework = query.value(4).toString();
            works.append(work);
        }
    } else {
        qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
        qDebug() << "Последний выполненный запрос:" << query.lastQuery();
    }

    return works;
}

// метод для обновления базы полсе подтверждения работы(не подтверждениы)
bool LeaderDatabase::updateWorkConfirmation(int workId, const QString &comment, bool confirmed)
{
    QSqlQuery query;

    query.prepare("UPDATE works_comment SET comment_leader = :comment WHERE id_namework = :workId");
    query.bindValue(":comment", comment);
    query.bindValue(":workId", workId);

    if (!query.exec()) {
        qDebug() << "Ошибка при обновлении комментария:" << query.lastError().text();
        qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return false;
    }

    query.prepare("UPDATE works_confirmed SET confirmed_employee = :confirmed, confirmed_leader = :confirmed WHERE id_namework = :workId");
    query.bindValue(":confirmed", confirmed ? "+" : "-");
    query.bindValue(":workId", workId);

    if (!query.exec()) {
        qDebug() << "Ошибка при обновлении статуса подтверждения:" << query.lastError().text();
        qDebug() << "Последний выполненный запрос:" << query.lastQuery();
        return false;
    }

    return true;
}


