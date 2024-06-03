// leaderdatabase.h
#ifndef LEADERDATABASE_H
#define LEADERDATABASE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QDate>
#include <QSqlDatabase>

// Структура для хранения информации о пользователе
struct UserInfo {
    int id;
    QString family;
    QString name;
    QString patronymic;
    QString role;
    QString degree;
    QDate dateRegistered;
};

// Структура для хранения информации о выполненных рабтах
struct CompletedWork {
    int id;
    QString family;
    QString name;
    QString patronymic;
    QString namework;
};

class QSqlDatabase; // Добавляем предварительное объявление класса QSqlDatabase

class LeaderDatabase : public QObject
{
    Q_OBJECT
public:
    explicit LeaderDatabase(QObject *parent = nullptr);

    QVector<UserInfo> getUsersInfo();
    QVector<UserInfo> getSelectableUsers();
    bool addWork(const QString &nameWork, int userId, double payment, int complexity, int hard, const QString &commentLeader);
    QVector<QVector<QString>> getOngoingWorks();
    int calculateTotalComplexityAndHard(int userId);
    QVector<CompletedWork> getCompletedWorks();
    bool updateWorkConfirmation(int workId, const QString &comment, bool confirmed);

private:
    QSqlDatabase m_database;
};

#endif // LEADERDATABASE_H
