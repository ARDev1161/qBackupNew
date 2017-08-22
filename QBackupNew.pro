#-------------------------------------------------
#
# Project created by QtCreator 2017-04-17T19:50:48
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QBackupNew
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    settings.cpp \
    backuper.cpp \
    backuptask.cpp \
    taskdialog.cpp \
    taskqueue.cpp \
    taskuploader.cpp \
    CustomClasses/timedmessagebox.cpp \
    YandexDisk/ydapi.cpp

HEADERS  += mainwindow.h \
    settings.h \
    backuper.h \
    backuptask.h \
    taskdialog.h \
    taskqueue.h \
    taskuploader.h \
    CustomClasses/timedmessagebox.h \
    YandexDisk/ydapi.h

FORMS    += mainwindow.ui \
    settings.ui \
    taskdialog.ui \
    taskqueue.ui

win32 {
    INCLUDEPATH += D:/Qt/quazip/include
    DEPENDPATH += D:/Qt/quazip/include
    LIBS += -LD:/Qt/quazip/lib/ -lquazip
} else {
    LIBS += -lquazip5
}

