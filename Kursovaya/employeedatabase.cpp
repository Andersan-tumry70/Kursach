#include "employeedatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

EmployeeDatabase::EmployeeDatabase(QObject *parent) : QObject(parent)
{
}

// Выгрузка текущих дополнительных работ
QVector<QStringList> EmployeeDatabase::getCurrentWorks(int userId)
{
    QVector<QStringList> works;

    if (userId == -1) {
        qDebug() << "Invalid userId";
        return works;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT w.namework, w.complexity, w.hard, w.payment, wc.comment, wc.comment_leader
        FROM works w
        LEFT JOIN works_comment wc ON w.id = wc.id_namework
        WHERE w.id_user = ? AND w.id IN (
            SELECT wc.id_namework
            FROM works_confirmed wc
            WHERE coalesce(wc.confirmed_employee,'') <> '+' AND coalesce(wc.confirmed_leader, '') <> '+'
        )
    )");
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Ошибка при получении текущих работ сотрудника:" << query.lastError().text();
        return works;
    }

    while (query.next()) {
        QStringList work;
        work << query.value(0).toString();
        work << query.value(1).toString();
        work << query.value(2).toString();
        work << query.value(3).toString();
        work << query.value(4).toString();
        work << query.value(5).toString();
        works.append(work);
    }

    return works;
}
// Выгрузка текущих дополнительных работ в CurrentWorksComboBox для подтверждения
QVector<QString> EmployeeDatabase::getCurrentWorkNames(int userId)
{
    QVector<QString> workNames;

    if (userId == -1) {
        qDebug() << "Invalid userId";
        return workNames;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT w.namework
        FROM works w
        WHERE w.id_user = ? AND w.id IN (
            SELECT wc.id_namework
            FROM works_confirmed wc
            WHERE coalesce(wc.confirmed_employee,'') <> '+' AND coalesce(wc.confirmed_leader, '') <> '+'
        )
    )");
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Ошибка при получении наименований текущих работ сотрудника:" << query.lastError().text();
        return workNames;
    }

    while (query.next()) {
        QString workName = query.value(0).toString();
        workNames.append(workName);
    }

    return workNames;
}

// Выгрузка текущих дополнительных работ
QVector<QStringList> EmployeeDatabase::getConfirmedWorks(int userId)
{
    QVector<QStringList> works;

    if (userId == -1) {
        qDebug() << "Invalid userId";
        return works;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT w.namework, w.complexity, w.hard, w.payment, wc.comment_leader
        FROM works w
        LEFT JOIN works_comment wc ON w.id = wc.id_namework
        WHERE w.id_user = ? AND w.id IN (
            SELECT wc.id_namework
            FROM works_confirmed wc
            WHERE coalesce(wc.confirmed_employee,'') = '+' AND coalesce(wc.confirmed_leader, '') = '+'
        )
    )");
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Ошибка при получении текущих работ сотрудника:" << query.lastError().text();
        return works;
    }

    while (query.next()) {
        QStringList work;
        work << query.value(0).toString();
        work << query.value(1).toString();
        work << query.value(2).toString();
        work << query.value(3).toString();
        work << query.value(4).toString();
        works.append(work);
    }

    return works;
}

// Метод для подтверждения работы
bool EmployeeDatabase::confirmWork(const QString &workName, const QString &comment)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM works WHERE namework = :workName");
    query.bindValue(":workName", workName);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
            return false;
    }

    if (!query.next()) {
        qDebug() << "Работа не найдена:" << workName;
        return false;
    }

    int workId = query.value(0).toInt();

    query.prepare("UPDATE works_comment SET comment_employee = :comment WHERE id_namework = :workId");
    query.bindValue(":comment", comment);
    query.bindValue(":workId", workId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления комментария:" << query.lastError().text();
            return false;
    }

    query.prepare("UPDATE works_confirmed SET confirmed_employee = '+', confirmed_leader = NULL WHERE id_namework = :workId");
    query.bindValue(":workId", workId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления подтверждения:" << query.lastError().text();
            return false;
    }

    return true;
}
