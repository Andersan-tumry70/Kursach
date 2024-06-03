#include "employee.h"
#include "ui_employee.h"
#include <QMessageBox>
#include "mainwindow.h"

Employee::Employee( int userId, QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::Employee),
    employeeDatabase(new EmployeeDatabase(this)),
    userId(userId)
{
    ui->setupUi(this);
    this->setFixedSize(730, 446);

    this->setWindowTitle("Сотрудник");
    setWindowIcon(QIcon(":/icon/icon.png"));

    loadCurrentWorks();
    populateCurrentWorksComboBox();
    loadConfirmedWorks();
}

Employee::~Employee()
{
    delete ui;
}

//Метод заполнения выполняющихся работ в tableWidgetCurrent
void Employee::loadCurrentWorks()
{
    QVector<QStringList> works = employeeDatabase->getCurrentWorks(userId);

    ui->tableWidgetCurrent->setRowCount(works.size());
    ui->tableWidgetCurrent->setColumnCount(6);

    for (int i = 0; i < works.size(); ++i) {
        QStringList work = works.at(i);
        for (int j = 0; j < work.size(); ++j) {
            ui->tableWidgetCurrent->setItem(i, j, new QTableWidgetItem(work.at(j)));
        }
    }
    ui->tableWidgetCurrent->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// Метод заполнения CurrentWorksComboBox ткущими работами для подтверждения выполнения
void Employee::populateCurrentWorksComboBox()
{
    QVector<QString> workNames = employeeDatabase->getCurrentWorkNames(userId);

    ui->CurrentWorksComboBox->clear();
    ui->CurrentWorksComboBox->addItems(workNames);
}

//Метод заполнения выполненных работ в tableWidgetConfirmed
void Employee::loadConfirmedWorks()
{
    QVector<QStringList> works = employeeDatabase->getConfirmedWorks(userId);

    ui->tableWidgetConfirmed->setRowCount(works.size());
    ui->tableWidgetConfirmed->setColumnCount(5);

    for (int i = 0; i < works.size(); ++i) {
        QStringList work = works.at(i);
        for (int j = 0; j < work.size(); ++j) {
            ui->tableWidgetConfirmed->setItem(i, j, new QTableWidgetItem(work.at(j)));
        }
    }
    ui->tableWidgetConfirmed->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// Слот для обработки нажатия кнопки ConfirmButton
void Employee::on_ConfirmButton_clicked()
{
    QString selectedWork = ui->CurrentWorksComboBox->currentText();
    QString comment = ui->CommentLine->text().trimmed();

    if (selectedWork.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите работу.");
        return;
    }

    // Обновляем данные в базе данных
    if (employeeDatabase->confirmWork(selectedWork, comment)) {
        QMessageBox::information(this, "Успех", "Работа успешно подтверждена.");
        ui->CurrentWorksComboBox->setCurrentIndex(0);
        ui->CommentLine->clear();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка при подтверждении работы.");
    }
    populateCurrentWorksComboBox();
    loadCurrentWorks();
}

// Метод выхода из системы -> кнопка
void Employee::on_CloseButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Завершение сеанса",
                                  "Вы хотите завершить сеанс?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->close();
        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
    }

    deleteLater();
}

