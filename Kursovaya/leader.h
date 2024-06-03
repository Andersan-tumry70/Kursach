// leader.h

#ifndef LEADER_H
#define LEADER_H

#include <QMainWindow>
#include <QTableWidget>
#include "leaderdatabase.h"

namespace Ui {
class Leader;
}

class Leader : public QMainWindow
{
    Q_OBJECT

public:
    explicit Leader(QWidget *parent = nullptr);
    ~Leader();

private slots:
    void on_SendLeaderButton_clicked();
    void on_tableWidget_4_cellClicked(int row);
    void on_tableWidget_3_cellClicked(int row);
    void loadCompletedWorks();
    void on_SendCompleteButton_clicked();
    void on_CloseButton_clicked();

private:
    Ui::Leader *ui;
    QTableWidget *userTableWidget;
    LeaderDatabase m_leaderDatabase;

    void loadUserData();
    void loadSelectableUsers();
    void loadOngoingWorks();
};

#endif // LEADER_H
