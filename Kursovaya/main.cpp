#include "mainwindow.h"
#include "databasehandler.h"
#include <QApplication>
#include <QPalette>
#include <QIcon>

void setLightPalette(QApplication &app) {
    QPalette lightPalette;

    lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::WindowText, Qt::black);
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase, QColor(242, 242, 242));
    lightPalette.setColor(QPalette::ToolTipBase, Qt::black);
    lightPalette.setColor(QPalette::ToolTipText, Qt::white);
    lightPalette.setColor(QPalette::Text, Qt::black);
    lightPalette.setColor(QPalette::Button, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::ButtonText, Qt::black);
    lightPalette.setColor(QPalette::BrightText, Qt::red);
    lightPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    lightPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    lightPalette.setColor(QPalette::HighlightedText, Qt::white);

    app.setPalette(lightPalette);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Установить светлую палитру
    setLightPalette(a);

    DatabaseHandler dbHandler;
    // Подключаемся к базе данных в указанной папке, если её нет, то создаём её (Вместо "database" можете назвать иначе, как хотите, чтобы называлась база данных)
    if (!dbHandler.connectToDatabase("", "database.db")) {
        qDebug() << "Не удалось подключиться к базе данных";
        return 1;
    }

    // Создаем таблицу Users
    if (!dbHandler.createUsersTable()) {
        qDebug() << "Не удалось создать таблицу Users";
        return 1;
    }

    // Создаем таблицу Works
    if (!dbHandler.createWorksTable()) {
        qDebug() << "Не удалось создать таблицу Works";
        return 1;
    }

    // Создаем таблицу WorksConfirmed
    if (!dbHandler.createWorksConfirmedTable()) {
        qDebug() << "Не удалось создать таблицу Works";
        return 1;
    }

    // Создаем таблицу WorksComment
    if (!dbHandler.createWorksCommentTable()) {
        qDebug() << "Не удалось создать таблицу Works";
        return 1;
    }

    // Создаем таблицу Works
    if (!dbHandler.createPasswordTable()) {
        qDebug() << "Не удалось создать таблицу Password";
        return 1;
    }

    // Вносим первого пользователя
    if (!dbHandler.createInitialData()) {
        qDebug() << "Не удалось создать пользователя";
        return 1;
    }

    MainWindow mainWindow;
    mainWindow.show();


    return a.exec();
}
