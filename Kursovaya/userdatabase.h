#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <QObject>
#include <QSqlDatabase>

class UserDatabase : public QObject
{
    Q_OBJECT
public:
    explicit UserDatabase(QObject *parent = nullptr);

    bool addUser(const QString &family, const QString &name, const QString &patronymic,
                 const QString &role, const QString &degree, const QString &login,
                 const QString &password);

    bool isLoginTaken(const QString &login);

    bool authenticateUser(const QString &login, const QString &password);
    QString getUserRole(const QString &login);
    QString getUserName(const QString &login);
    int getCurrentUserId() const;

signals:

public slots:

private:
    QSqlDatabase m_db;
    int m_currentUserId;
};

#endif // USERDATABASE_H
