#include <QApplication>
#include <QTreeWidgetItem>
#include <QDir>
#include <QStringList>
#include <QList>
#include <QThread>
#include <QProgressBar>
#include <QMessageBox>
#include <QTextStream>

#include "Zipper/zipper.h"
#include "taskqueue.h"
#include "ui_taskqueue.h"
#include "Uploader/taskuploader.h"

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

    QStringList params;
    params.append(task->getName());
    params.append("wait");
    params.append(task->getUpload() ? "wait" : "-");

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget, params);
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
    isWorking 	= true;

    currentFileName = genFileName(currentTask->getOutputFolder(), currentTask->getName());
    writeToLog("File name " + currentFileName);

    zipper *backuper = new zipper(true, currentTask->getInputFolder(), currentFileName);
    QThread *thread  = new QThread();
    backuper->moveToThread(thread);

    connect(thread,	  SIGNAL(started()), backuper, SLOT(start()));
    connect(backuper, SIGNAL(compressProgress(qint64,qint64)),
            this, 	  SLOT(updateUploadProgressBar(qint64,qint64)));
    connect(backuper, SIGNAL(compressFinished()), this, SLOT(upload()));
    connect(backuper, SIGNAL(compressFinished()), thread, SLOT(quit()));

    connect(backuper, SIGNAL(compressError(QString)), backuper, SLOT(deleteLater()));
    connect(backuper, SIGNAL(compressError(QString)), thread, SLOT(deleteLater()));

    connect(backuper, SIGNAL(compressFinished()), backuper, SLOT(deleteLater()));
    connect(backuper, SIGNAL(compressFinished()), thread, SLOT(deleteLater()));

    thread->start();

    writeToLog("Backuping " + currentTask->getInputFolder() +" to " + currentFileName);
}

void taskQueue::upload()
{
    writeToLog("Backuping complete");

    updateProgressBar();
    ui->treeWidget->topLevelItem(currentIndex)->setText(1, "Done");

    if(currentTask->getUpload()){
        taskUploader *uploader = new taskUploader(currentTask, currentFileName);

        connect(uploader, SIGNAL(finished()), this, SLOT(onUploadFinished()));
        connect(uploader, SIGNAL(finished()), uploader, SLOT(deleteLater()));
        connect(uploader, SIGNAL(onError(YDAPI*)), this, SLOT(uploadError(YDAPI*)));
        connect(uploader, SIGNAL(onErrorInRequest(QString,QString)),
                this, SLOT(uploadErrorInRequest(QString,QString)));
        connect(uploader, SIGNAL(uploadProgress(qint64,qint64)),
                this, SLOT(updateUploadProgressBar(qint64,qint64)));

        uploader->upload();

        writeToLog("Start uploading");

    } else {
        isWorking = false;
        if(completeOps == numberOfOps) emit backupFinished();
        writeToLog("Uploading is disabled");
        start();
    }
}

void taskQueue::onUploadFinished()
{
    writeToLog("Uploading complete");
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

    writeToLog("Upload error: " + api->getErrorMessage());

    if(completeOps == numberOfOps) emit backupFinished();

    start();
}

void taskQueue::uploadErrorInRequest(QString error, QString description)
{
    QMessageBox::critical(this, error, description);
    updateProgressBar();
    ui->treeWidget->topLevelItem(currentIndex)->setText(2, "Error");
    isWorking = false;

    writeToLog("Upload error: " + error + ": " + description);

    if(completeOps == numberOfOps) emit backupFinished();

    start();

}

void taskQueue::compressError(QString errorMsg)
{
    //QMessageBox::critical(this, "Error", errorMsg);
//    updateProgressBar();
//    ui->treeWidget->topLevelItem(currentIndex)->setText(1, "Error");
//
//    writeToLog("Compress error: "+ errorMsg);
//
//    if(completeOps == numberOfOps) emit backupFinished();
//
//    start();
}

QString taskQueue::genFileName(QString path, QString name)
{
     QString tempFileName = path + QDir::separator() + name +
             "_" + QDate::currentDate().toString("dd.MM.yyyy");

    if(QFile(tempFileName + ".zip").exists()){
        bool exist = true;
        for(int i=1; exist != false; i++){
            QString buffFileName = tempFileName + QString("(%1)").arg(i);
            if(!QFile(buffFileName + ".zip").exists()){
                exist = false;
                tempFileName = buffFileName+".zip";
            }
        }
    } else {
        tempFileName += ".zip";
    }
    return tempFileName;
}

void taskQueue::writeToLog(QString logInfo)
{
    QString buffText = "[ " + QDate::currentDate().toString("dd.MM.yyyy") + " " +
            QTime::currentTime().toString("hh:mm:ss") + " " + currentTask->getName() + "] " + logInfo + "\n";

    QFile *logFile = new QFile(QApplication::applicationDirPath() + QDir::separator() + "log.txt");
    logFile->open(QIODevice::Append | QIODevice::Text);
    //QTextStream logStream(logFile);
    logFile->write(buffText.toUtf8());
    logFile->flush();
    logFile->close();
    logFile->deleteLater();
}
