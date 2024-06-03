#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QMainWindow>
#include "employeedatabase.h"

namespace Ui {
class Employee;
}

class Employee : public QMainWindow
{
    Q_OBJECT

public:
    explicit Employee(int userId = -1, QMainWindow *parent = nullptr);
    ~Employee();
    void populateCurrentWorksComboBox();

private slots:

    void on_CloseButton_clicked();
    void on_ConfirmButton_clicked();

private:
    Ui::Employee *ui;
    EmployeeDatabase *employeeDatabase;
    int userId;
    void loadCurrentWorks();
    void loadConfirmedWorks();
};

#endif // EMPLOYEE_H
