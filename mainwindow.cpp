#include <QInputDialog>
#include <QTreeWidget>
#include <QSystemTrayIcon>
#include <QTime>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QTimer>
#include <QProcess>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backuptask.h"
#include "taskdialog.h"
#include "settings.h"
#include "CustomClasses/timedmessagebox.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(showTaskSettings(QTreeWidgetItem*)));            //open task settings window
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


    //------------
    connect(backupQueue, SIGNAL(backupFinished()), this, SLOT(powerOff()));
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
    //disconnectTasks();
    tasks.clear();
    ui->treeWidget->clear();

    QList<QTreeWidgetItem*> items;
    QStringList taskNames = qSett.value("tasks").toStringList();

    foreach(QString name, taskNames){
        backupTask *task = new backupTask(name);
        tasks.push_back(task);
        QStringList params;

        params.append(task->getName());
        params.append(task->getInputFolder());
        params.append(task->getOutputFolder());
        params.append(task->getAutoBackupTime().toString("hh:mm"));
        items.append(new QTreeWidgetItem(ui->treeWidget, params));
    }
    ui->treeWidget->addTopLevelItems(items);
    //connectTasks();
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
    showTaskSettings(name);
}

void MainWindow::showTaskSettings(QString taskName)
{
    foreach (backupTask *task, tasks) {
        if(task->getName() == taskName){
            //task->show();
            taskDialog *taskDialogWindow = new taskDialog(task);
            taskDialogWindow->show();
            connect(taskDialogWindow, SIGNAL(accepted()), this, SLOT(loadTasks()));
            connect(taskDialogWindow, SIGNAL(accepted()), taskDialogWindow, SLOT(deleteLater()));
            connect(taskDialogWindow, SIGNAL(rejected()), taskDialogWindow, SLOT(deleteLater()));

            break;
        }
    }
}

void MainWindow::showTaskSettings(QTreeWidgetItem *item)
{
    QString taskName = item->text(0);
    this->showTaskSettings(taskName);
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
        if(task->getAutoBackup()){
            if(task->getAutoBackupTime().hour() == QTime::currentTime().hour() &&
                    task->getAutoBackupTime().minute() == QTime::currentTime().minute()){
                //task->runBackup();
                backupQueue->enqueue(task);
            }
        }
    }
}

void MainWindow::trayMessageSlot(bool ok, QString messageText)
{
    QString title = ok ? "Succes" : "Error";
    QSystemTrayIcon::MessageIcon messageIcon = ok ? QSystemTrayIcon::Information : QSystemTrayIcon::Critical;

    this->systemTray->showMessage(title, messageText, messageIcon);
}

void MainWindow::powerOff()
{
    if(QTime::currentTime() >= qSett.value("powerOffTime").toTime()){
        TimedMessageBox *msb = new TimedMessageBox(
                    60,
                    TimedMessageBox::Question,
                    tr("Shutdown"),
                    tr("Do you want to shutdown now? (%1)"),
                    TimedMessageBox::No | TimedMessageBox::Yes);
        msb->setDefaultButton(TimedMessageBox::Yes);

        bool res = msb->exec();

        if(res == true){
#ifdef Q_OS_WIN32
        QProcess::startDetached("shutdown -s -f -t 00");
#endif
#ifdef Q_OS_LINUX
        //QProcess::startDetached("shutdown -P now");
#endif
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
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
        if(task->getName() == taskName){
            backupQueue->enqueue(task);
        }
    }
}

void MainWindow::on_actionSettings_triggered()
{
    MWSettings.show();
    MWSettings.loadSettings();
}


void MainWindow::on_actionShow_queue_triggered()
{
    backupQueue->show();
}
