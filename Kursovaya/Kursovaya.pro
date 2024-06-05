QT += sql
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admin.cpp \
    createdeleteuserdatabase.cpp \
    databasehandler.cpp \
    employee.cpp \
    employeedatabase.cpp \
    leader.cpp \
    leaderdatabase.cpp \
    main.cpp \
    mainwindow.cpp \
    registrationdialog.cpp \
    userdatabase.cpp

HEADERS += \
    admin.h \
    createdeleteuserdatabase.h \
    databasehandler.h \
    employee.h \
    employeedatabase.h \
    leader.h \
    leaderdatabase.h \
    mainwindow.h \
    registrationdialog.h \
    userdatabase.h

FORMS += \
    admin.ui \
    employee.ui \
    leader.ui \
    mainwindow.ui \
    registrationdialog.ui

TRANSLATIONS += \
    Kursovaya_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

RC_ICONS += icon.ico


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
