// createdeleteuserdatabase.h
#ifndef CREATEDELETEUSERDATABASE_H
#define CREATEDELETEUSERDATABASE_H

#include <QObject>
#include <QSqlDatabase>

class CreateDeleteUserDataBase : public QObject
{
    Q_OBJECT
public:
    explicit CreateDeleteUserDataBase(QObject *parent = nullptr);

    bool isLoginTaken(const QString &login);
    bool createUser(const QString &family, const QString &name, const QString &patronymic,
                    const QString &role, const QString &degree,
                    const QString &login, const QString &password);
    bool deleteUser(const QString &login, const QString &password,
                                              const QString &family, const QString &name, const QString &patronymic, int &userId);
    int IddeleteUser(const QString &login, const QString &password,int &f_userId);

private:
    QSqlDatabase m_database;
};

#endif // CREATEDELETEUSERDATABASE_H
