// leader.cpp

#include "leader.h"
#include "ui_leader.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QRegularExpression>
#include "mainwindow.h"

Leader::Leader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Leader),
    userTableWidget(new QTableWidget(this))
{
    ui->setupUi(this);
    this->setFixedSize(904, 695);

    this->setWindowTitle("Представитель кафедры");
    setWindowIcon(QIcon(":/icon/icon.png"));

    // Инициализируем таблицу
    userTableWidget = ui->UserTableWidget;

    loadUserData();
    loadSelectableUsers();
    loadOngoingWorks();
    loadCompletedWorks();

    // Подключаем сигнал нажатия на ячейку в таблице к слоту on_tableWidget_4_cellClicked
    connect(ui->tableWidget_4, &QTableWidget::cellClicked, this, &Leader::on_tableWidget_4_cellClicked);
    // Подключаем сигнал нажатия на ячейку в таблице к слоту on_tableWidget_3_cellClicked
    connect(ui->tableWidget_3, &QTableWidget::cellClicked, this, &Leader::on_tableWidget_3_cellClicked);

}


Leader::~Leader()
{
    delete ui;
}

// Метод для загрузки данных пользователей
void Leader::loadUserData()
{
    QVector<UserInfo> users = m_leaderDatabase.getUsersInfo();

    // Проверка успешности выполнения запроса
    if (users.isEmpty()) {
        qDebug() << "Ошибка при загрузке данных о пользователях";
        return;
    }

    userTableWidget->setRowCount(users.size());
    userTableWidget->setColumnCount(6);

    for (int i = 0; i < users.size(); ++i) {
        const UserInfo &user = users.at(i);
        userTableWidget->setItem(i, 0, new QTableWidgetItem(user.family));
        userTableWidget->setItem(i, 1, new QTableWidgetItem(user.name));
        userTableWidget->setItem(i, 2, new QTableWidgetItem(user.patronymic));
        userTableWidget->setItem(i, 3, new QTableWidgetItem(user.role));
        userTableWidget->setItem(i, 4, new QTableWidgetItem(user.degree));
        userTableWidget->setItem(i, 5, new QTableWidgetItem(user.dateRegistered.toString("dd.MM.yyyy")));

        for (int j = 0; j < 6; ++j) {
            userTableWidget->item(i, j)->setFlags(userTableWidget->item(i, j)->flags() & ~Qt::ItemIsEditable);
        }
    }
}

// Метод для загрузки выбираемых пользователей
void Leader::loadSelectableUsers()
{
    QVector<UserInfo> users = m_leaderDatabase.getSelectableUsers();

    // Проверка успешности выполнения запроса
    if (users.isEmpty()) {
        qDebug() << "Ошибка при загрузке данных о пользователях";
        return;
    }

    ui->tableWidget_4->setRowCount(users.size());
    ui->tableWidget_4->setColumnCount(4);

    for (int i = 0; i < users.size(); ++i) {
        const UserInfo &user = users.at(i);
        ui->tableWidget_4->setItem(i, 0, new QTableWidgetItem(QString::number(user.id)));
        ui->tableWidget_4->setItem(i, 1, new QTableWidgetItem(user.family));
        ui->tableWidget_4->setItem(i, 2, new QTableWidgetItem(user.name));
        ui->tableWidget_4->setItem(i, 3, new QTableWidgetItem(user.patronymic));

        for (int j = 0; j < 4; ++j) {
            ui->tableWidget_4->item(i, j)->setFlags(ui->tableWidget_4->item(i, j)->flags() & ~Qt::ItemIsEditable);
        }
    }
}

// Метод для выгрузки выполннныхя работ
void Leader::loadCompletedWorks() {
    QVector<CompletedWork> works = m_leaderDatabase.getCompletedWorks();

    // Проверка успешности выполнения запроса
    if (works.isEmpty()) {
        qDebug() << "Ошибка при загрузке подтвержденных работ или нет подтвержденных работ";
        return;
    }

    ui->tableWidget_3->setRowCount(works.size());
    ui->tableWidget_3->setColumnCount(5);

    for (int i = 0; i < works.size(); ++i) {
        const CompletedWork &work = works.at(i);
        ui->tableWidget_3->setItem(i, 0, new QTableWidgetItem(QString::number(work.id)));
        ui->tableWidget_3->setItem(i, 1, new QTableWidgetItem(work.family));
        ui->tableWidget_3->setItem(i, 2, new QTableWidgetItem(work.name));
        ui->tableWidget_3->setItem(i, 3, new QTableWidgetItem(work.patronymic));
        ui->tableWidget_3->setItem(i, 4, new QTableWidgetItem(work.namework));

        for (int j = 0; j < 5; ++j) {
            ui->tableWidget_3->item(i, j)->setFlags(ui->tableWidget_3->item(i, j)->flags() & ~Qt::ItemIsEditable);
        }
    }
}

// Метод для выгрузки текущих выполнябщихся работ
void Leader::loadOngoingWorks()
{
    QVector<QVector<QString>> works = m_leaderDatabase.getOngoingWorks();

    // Проверка успешности выполнения запроса
    if (works.isEmpty()) {
        qDebug() << "Ошибка при загрузке выполняющихся работ или нет выполняющихся работ";
        return;
    }

    ui->tableWidget_2->setRowCount(works.size());
    ui->tableWidget_2->setColumnCount(7);

    for (int i = 0; i < works.size(); ++i) {
        const QVector<QString> &work = works.at(i);
        for (int j = 0; j < work.size(); ++j) {
            ui->tableWidget_2->setItem(i, j, new QTableWidgetItem(work.at(j)));
            ui->tableWidget_2->item(i, j)->setFlags(ui->tableWidget_2->item(i, j)->flags() & ~Qt::ItemIsEditable);
        }
    }
}

// Метод, вызываемый при нажатии кнопки "Отправить"
void Leader::on_SendLeaderButton_clicked()
{
    QString nameWork = ui->NameWorkLine->text();
    QString paymentText = ui->PaymentLine->text();
    double payment = paymentText.toDouble();
    int complexity = ui->ComplexitySpinBox->value();
    int hard = ui->HardSpinBox->value();
    QString commentLeader = ui->CommentLeaderLine->text();

    if (payment <= 0) {
        QMessageBox::critical(this, "Ошибка", "Неверный формат оплаты. Пожалуйста, введите положительное число.");
        return;
    }

    if (complexity < 1 || complexity > 10) {
        QMessageBox::critical(this, "Ошибка", "Пожалуйста, введите число трудоёмкости от 1 до 10.");
        return;
    }

    if (hard < 1 || hard > 10) {
        QMessageBox::critical(this, "Ошибка", "Пожалуйста, введите число сложности от 1 до 10.");
        return;
    }

    nameWork = nameWork.trimmed();
    QRegularExpression re("^[a-zA-Zа-яА-Я0-9\\s]+$");
    QRegularExpressionMatch match = re.match(nameWork);
    if (nameWork.isEmpty() || nameWork.length() < 3 || !match.hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Наименование работы должно содержать только буквы и быть длиной не менее 3 символов.");
        return;
    }

    nameWork = nameWork.toLower();
    nameWork[0] = nameWork[0].toUpper();

    // Получаем ID выбранного пользователя
    int selectedRow = ui->tableWidget_4->currentRow();
    if (selectedRow == -1) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите пользователя.");
        return;
    }

    int userId = ui->tableWidget_4->item(selectedRow, 0)->text().toInt();

    int totalComplexityAndHard = m_leaderDatabase.calculateTotalComplexityAndHard(userId);
    if (totalComplexityAndHard != -1 && totalComplexityAndHard + complexity + hard > 40) {
        QMessageBox::critical(this, "Ошибка", "Выбранный работник уже загружен.");
        return;
    }

    if (m_leaderDatabase.addWork(nameWork, userId, payment, complexity, hard, commentLeader)) {
        ui->NameWorkLine->clear();
        ui->PaymentLine->clear();
        ui->ComplexitySpinBox->setValue(0);
        ui->HardSpinBox->setValue(0);
        ui->CommentLeaderLine->clear();


        QMessageBox::information(this, "Успех", "Работа успешно добавлена.");
        loadOngoingWorks(); // Перезагружаем данные в tableWidget_2
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка при добавлении работы в базу данных.");
        qDebug() << "Ошибка при добавлении работы в базу данных";
    }
}

// Метод, вызываемый при нажатии на кнопку "Отправить" (подтверждение работы)
void Leader::on_SendCompleteButton_clicked() {
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget_3->selectedItems();
    if (selectedItems.size() != ui->tableWidget_3->columnCount()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите одну работу.");
        return;
    }

    // Получаем идентификатор работы из выбранной строки
    int workId = selectedItems.at(0)->text().toInt();

    QString comment = ui->CommentLine->text();

    bool confirmed;
    if (ui->RadioButtonYes->isChecked()) {
        confirmed = true;
    } else if (ui->RadioButtonNo->isChecked()) {
        confirmed = false;
    } else {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите подтверждение (Да или Нет).");
        return;
    }

    if (m_leaderDatabase.updateWorkConfirmation(workId, comment, confirmed)) {
        ui->CommentLine->clear();
        ui->RadioButtonYes->setChecked(false);
        ui->RadioButtonNo->setChecked(false);

        QMessageBox::information(this, "Успех", "Информация успешно обновлена.");

        ui->tableWidget_3->clearContents();
        ui->tableWidget_3->setRowCount(0);
        loadCompletedWorks();
        loadOngoingWorks();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка при обновлении информации в базе данных.");
        qDebug() << "Ошибка при обновлении информации в базе данных";
    }
}

// Метод, вызываемый при клике на ячейку в таблице tableWidget_4
void Leader::on_tableWidget_4_cellClicked(int row)
{
    ui->tableWidget_4->selectRow(row); // Выделяем всю строку, соответствующую выбранной ячейке
}

// Метод, вызываемый при клике на ячейку в таблице tableWidget_3
void Leader::on_tableWidget_3_cellClicked(int row)
{
    ui->tableWidget_3->selectRow(row); // Выделяем всю строку, соответствующую выбранной ячейке
}

void Leader::on_CloseButton_clicked()
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
}

