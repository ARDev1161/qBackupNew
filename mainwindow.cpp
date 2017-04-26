#include <QInputDialog>
#include <QTreeWidget>
#include <QSystemTrayIcon>
#include <QTime>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backuptask.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(showTaskSettings(QTreeWidgetItem*)));
    loadTasks();

    //----------------------Tray menu
    togleVisibleAction = new QAction(tr("Exit"), this);
    toggleVisibleAction = new QAction(tr("Show/Hide"), this);

    trayMenu = new QMenu(this);
    trayMenu->addAction(toggleVisibleAction);
    trayMenu->addAction(togleVisibleAction);

    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(qApp->applicationDirPath()+QDir::separator()+"backup.png"));
    systemTray->setContextMenu(trayMenu);
    systemTray->setVisible(true);

    connect(togleVisibleAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(toggleVisibleAction, SIGNAL(triggered(bool)), this, SLOT(toggleVisible()));
    //----------------------Tray menu

    backupTimer = new QTimer(this);
    backupTimer->start(50000);
    connect(backupTimer, SIGNAL(timeout()), this, SLOT(startBackup()));
}

void MainWindow::deleteTask()
{
    QString taskName = ui->treeWidget->currentItem()->text(0);
    QStringList taskNames = qSett.value("tasks").toStringList();
    for(int i=0; i<taskNames.size(); i++){
        if(taskNames.at(i) == taskName){
            taskNames.removeAt(i);
            break;
        }
    }
    qSett.remove(taskName);
    qSett.setValue("tasks", taskNames);
    loadTasks();
}

void MainWindow::loadTasks()
{
    disconnectTasks();
    tasks.clear();
    ui->treeWidget->clear();

    QList<QTreeWidgetItem*> items;
    QStringList taskNames = qSett.value("tasks").toStringList();

    foreach(QString name, taskNames){
        backupTask *task = new backupTask(name);
        tasks.push_back(task);
        QStringList params;

        params.append(task->name());
        params.append(task->dir());
        params.append(task->backupDir());
        params.append(task->backupTime().toString("hh:mm"));
        items.append(new QTreeWidgetItem(ui->treeWidget, params));
    }
    ui->treeWidget->addTopLevelItems(items);
    connectTasks();
}

void MainWindow::addTask()
{
    QStringList taskNames = qSett.value("tasks").toStringList();
    QString name;

    bool succes;
    do {
        name = QInputDialog::getText(
                this, tr("Name"),
                tr("Enter new backup name:"),
                QLineEdit::Normal,"",&succes);
    } while(succes && name.isEmpty());

    taskNames.append(name);		//adding new task name in qstringlist
    tasks.push_back(new backupTask(name));	//creating backupTask object and adding to vector
    qSett.setValue("tasks", taskNames);
    loadTasks();
}

void MainWindow::showTaskSettings(QString taskName)
{
    foreach (backupTask *task, tasks) {
        if(task->name() == taskName){
            task->show();
            break;
        }
    }
}

void MainWindow::showTaskSettings(QTreeWidgetItem *item)
{
    QString taskName = item->text(0);
    foreach (backupTask *task, tasks) {
        if(task->name() == taskName){
            task->show();
            break;
        }
    }
}

void MainWindow::connectTasks()
{
    foreach (backupTask *task, tasks) {
        connect(task, SIGNAL(accepted()), this, SLOT(loadTasks()));
        connect(task, SIGNAL(trayMessageSignal(bool,QString)), this, SLOT(trayMessageSlot(bool,QString)));
    }
}

void MainWindow::disconnectTasks()
{
    foreach (backupTask *task, tasks) {
        disconnect(task, SIGNAL(accepted()), this, SLOT(loadTasks()));
        disconnect(task, SIGNAL(trayMessageSignal(bool,QString)),
                   this, SLOT(trayMessageSlot(bool,QString)));
    }
}

void MainWindow::toggleVisible()
{
    this->setVisible(!(this->isVisible()));
}

void MainWindow::startBackup()
{
    foreach (backupTask *task, tasks) {
        if(task->enabledAutoBackup()){
            if(task->backupTime().hour() == QTime::currentTime().hour() &&
                    task->backupTime().minute() == QTime::currentTime().minute()){
                task->runBackup();
            }
        }
    }
}

void MainWindow::trayMessageSlot(bool ok, QString messageText)
{
    QString title = ok ? "Succes" : "Error";
    QSystemTrayIcon::MessageIcon messageIcon =
            ok ? QSystemTrayIcon::Information : QSystemTrayIcon::Critical;
    this->systemTray->showMessage(title, messageText, messageIcon);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAdd_task_triggered()
{
    addTask();
}

void MainWindow::on_actionRemove_task_triggered()
{
    deleteTask();
}

void MainWindow::on_actionUpdate_triggered()
{
    loadTasks();
}

void MainWindow::on_actionRun_Backup_triggered()
{
    QString taskName = ui->treeWidget->currentItem()->text(0);
    foreach (backupTask *task, tasks) {
        if(task->name() == taskName){
            task->runBackup();
        }
    }
}

void MainWindow::on_actionSettings_triggered()
{
    MWSettings.show();
    MWSettings.loadSettings();
}

