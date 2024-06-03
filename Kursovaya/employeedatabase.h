#ifndef EMPLOYEEDATABASE_H
#define EMPLOYEEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVector>
#include <QStringList>

class EmployeeDatabase : public QObject
{
    Q_OBJECT

public:
    explicit EmployeeDatabase(QObject *parent = nullptr);
    QVector<QStringList> getCurrentWorks(int userId);
    QVector<QString> getCurrentWorkNames(int userId);
    QVector<QStringList> getConfirmedWorks(int userId);
    bool confirmWork(const QString &workName, const QString &comment);


private:
    QSqlDatabase m_database;
};

#endif // EMPLOYEEDATABASE_H
