// admin.h
#ifndef ADMIN_H
#define ADMIN_H

#include <QMainWindow>
#include "createdeleteuserdatabase.h"
#include "userdatabase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Admin; }
QT_END_NAMESPACE

class Admin : public QMainWindow
{
    Q_OBJECT

public:
    explicit Admin(int userId, QWidget *parent = nullptr);
    ~Admin();

private slots:
    void on_CreateButton_clicked();
    void on_DeleteButton_clicked();
    void on_CloseButton_clicked();

private:
    Ui::Admin *ui;
    CreateDeleteUserDataBase m_userDatabase;
    int m_userId;
};

#endif // ADMIN_H
