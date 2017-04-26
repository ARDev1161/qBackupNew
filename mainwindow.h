#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <QVector>
#include <QTreeWidgetItem>
#include <QSystemTrayIcon>

#include "backuptask.h"
#include "settings.h"

#define ORGANIZATION_NAME "romanzm"
#define ORGANIZATION_DOMAIN "none"
#define APPLICATION_NAME "QBackup2"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void loadTasks();
    void addTask();
    void deleteTask();
    void connectTasks();
    void disconnectTasks();
    void showTaskSettings(QString taskName);
    void showTaskSettings(QTreeWidgetItem* item);

    void toggleVisible();

    void startBackup();

    void trayMessageSlot(bool ok, QString messageText);

    //--------------------------------------
    void on_actionAdd_task_triggered();

    void on_actionRemove_task_triggered();

    void on_actionUpdate_triggered();

    void on_actionRun_Backup_triggered();

    void on_actionSettings_triggered();

private:
    settings MWSettings;
    QTimer *backupTimer;
    QSettings qSett;
    QVector<backupTask*> tasks;
    Ui::MainWindow *ui;
    QSystemTrayIcon *systemTray;
    QMenu *trayMenu;
    QAction *toggleVisibleAction;
    QAction *togleVisibleAction;
    //QTimer *timer;
};

#endif // MAINWINDOW_H
