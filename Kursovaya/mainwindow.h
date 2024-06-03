#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userdatabase.h"
#include "admin.h"
#include "admin.h"
#include "leader.h"
#include "employee.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();
    void on_RegistrationButton_clicked();

private:
    Ui::MainWindow *ui;
    UserDatabase *m_userDatabase;
    Admin *adminWindow;
    Leader *leaderWindow;
    Employee *employeeWindow;
    int currentUserId;

};

#endif // MAINWINDOW_H
