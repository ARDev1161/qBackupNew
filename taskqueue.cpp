#include <QTreeWidgetItem>
#include <QDir>
#include <QStringList>
#include <QList>
#include <QThread>
#include <QProgressBar>
#include <QMessageBox>

#include "backuper.h"
#include "taskqueue.h"
#include "ui_taskqueue.h"
#include "taskuploader.h"

taskQueue::taskQueue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskQueue)
{
    ui->setupUi(this);
    numberOfOps = 0;
    completeOps = 0;
    ui->progressBar->setValue(0);

    isWorking = false;

    currentIndex = -1;
}

taskQueue::~taskQueue()
{
    delete ui;
}

void taskQueue::enqueue(backupTask *task)
{
    queue.append(task);
    //ui->treeWidget->clear();

    QStringList params;
    params.append(task->getName());
    params.append("wait");
    params.append(task->getUpload() ? "wait" : "-");

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget, params);
    //ui->treeWidget->insertTopLevelItem(ui->treeWidget->topLevelItemCount(), item);
    ui->treeWidget->addTopLevelItem(item);

    numberOfOps += task->getUpload() ? 2 : 1;
    completeOps--;
    updateProgressBar();
    if(!isWorking) this->start();
}

void taskQueue::updateProgressBar()
{
    completeOps++;
    ui->progressBar->setValue(100*completeOps/numberOfOps);
}

void taskQueue::updateUploadProgressBar(qint64 sent, qint64 total)
{
    ui->uploadProgressBar->setMaximum(100);
    if(total != 0 )
        ui->uploadProgressBar->setValue((qint64)((100 * sent)/total));
}

void taskQueue::start()
{
    if(queue.empty()) return;
    currentIndex++;
    currentTask = queue.takeFirst();
    isWorking = true;

    currentFileName = currentTask->getOutputFolder() + QDir::separator() +
            currentTask->getName() + "_" + QDate::currentDate().toString("dd.MM.yyyy");

    QString tempFileName = currentFileName + ".zip";

    if(QFile(tempFileName).exists()){
        bool exist = true;
        for(int i=0; exist != false; i++){
            QString buffFileName = currentFileName+QString("(%1)").arg(i);
            if(!QFile(buffFileName+".zip").exists()){
                exist = false;
                currentFileName = buffFileName+".zip";
            }
        }
    } else {
        currentFileName += ".zip";
    }

    QThread *thread = new QThread();
    Backuper *threadBackuper = new Backuper(currentFileName, currentTask->getInputFolder());
    threadBackuper->moveToThread(thread);

    connect(thread, SIGNAL(started()), threadBackuper, SLOT(runBackup()));
    connect(threadBackuper, SIGNAL(backupFinished()), thread, SLOT(quit()));
    connect(threadBackuper, SIGNAL(backupFinished()), this, SLOT(upload())); //after runBackup
    connect(threadBackuper, SIGNAL(backupFinished()), threadBackuper, SLOT(deleteLater()));
    connect(thread, SIGNAL(backupFinished()), thread, SLOT(deleteLater()));

    thread->start();
}

void taskQueue::upload()
{
    updateProgressBar();
    ui->treeWidget->topLevelItem(currentIndex)->setText(1, "Done");

    if(currentTask->getUpload()){
//        QThread *thread = new QThread();
        taskUploader *uploader = new taskUploader(currentTask, currentFileName);

//        uploader->moveToThread(thread);

//        connect(thread, SIGNAL(started()), uploader, SLOT(upload()));
//        connect(uploader, SIGNAL(finished()), thread, SLOT(quit()));
//        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        connect(uploader, SIGNAL(finished()), this, SLOT(onUploadFinished()));
        connect(uploader, SIGNAL(finished()), uploader, SLOT(deleteLater()));
        connect(uploader, SIGNAL(onError(YDAPI*)), this, SLOT(uploadError(YDAPI*)));
        connect(uploader, SIGNAL(onErrorInRequest(QString,QString)),
                this, SLOT(uploadErrorInRequest(QString,QString)));

        //thread->start();
        connect(uploader, SIGNAL(uploadProgress(qint64,qint64)),
                this, SLOT(updateUploadProgressBar(qint64,qint64)));
        uploader->upload();
    } else {
        isWorking = false;
        if(completeOps == numberOfOps) emit backupFinished();
        start();
    }

}

void taskQueue::onUploadFinished()
{
    updateProgressBar();
    ui->treeWidget->topLevelItem(currentIndex)->setText(2, "Done");
    isWorking = false;

    if(completeOps == numberOfOps) emit backupFinished();

    start();
}

void taskQueue::clear()
{
    numberOfOps = 0;
    completeOps = 0;
    isWorking = false;
    queue.clear();
    currentIndex = -1;
    ui->progressBar->setValue(0);
    ui->treeWidget->clear();
}

void taskQueue::on_pushButton_clicked()
{
    if(!isWorking)
        this->clear();
}

void taskQueue::uploadError(YDAPI *api)
{
    QMessageBox::critical(this, tr("Upload error"), api->getErrorMessage());
    updateProgressBar();
    ui->treeWidget->topLevelItem(currentIndex)->setText(2, "Error");
    isWorking = false;

    if(completeOps == numberOfOps) emit backupFinished();

    start();
}

void taskQueue::uploadErrorInRequest(QString error, QString description)
{
    QMessageBox::critical(this, error, description);
    updateProgressBar();
    ui->treeWidget->topLevelItem(currentIndex)->setText(2, "Error");
    isWorking = false;

    if(completeOps == numberOfOps) emit backupFinished();

    start();

}
