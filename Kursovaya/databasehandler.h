// databasehandler.h
#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QtSql>

class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(QObject *parent = nullptr);
    bool connectToDatabase(const QString &dbPath, const QString &dbName);
    bool createUsersTable();
    bool createWorksTable();
    bool createWorksConfirmedTable();
    bool createWorksCommentTable();
    bool createPasswordTable();
    bool createInitialData();

private:
    QSqlDatabase m_db;
};

#endif // DATABASEHANDLER_H
