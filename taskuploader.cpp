#include "taskuploader.h"
#include "YandexDisk/ydapi.h"

taskUploader::taskUploader(backupTask *task, QString fileFullPath, QObject *parent) : QObject(parent)
{
    this->task = task;
    this->fileFullPath = fileFullPath;
    yd = new YDAPI(this);
    yd->setToken(qSett.value("token").toString());
    connect(yd, SIGNAL(onError(YDAPI*)), this, SIGNAL(onError(YDAPI*)));
    connect(yd, SIGNAL(onErrorInRequest(QString,QString)), this, SIGNAL(onErrorInRequest(QString,QString)));
    connect(yd, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
}

void taskUploader::upload()
{
    connect(yd, SIGNAL(folderCreated()), this, SLOT(uploadToFolder()));
    yd->createFolder(task->getName());
}

void taskUploader::uploadToFolder()
{
    disconnect(yd, SIGNAL(folderCreated()), this, SLOT(uploadToFolder()));
    connect(yd, SIGNAL(finished()), this, SIGNAL(finished()));
    yd->upload(fileFullPath);
}
