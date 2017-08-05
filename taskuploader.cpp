#include "taskuploader.h"
#include <ydapi.h>

taskUploader::taskUploader(backupTask *task, QString fileFullPath, QObject *parent) : QObject(parent)
{
    this->task = task;
    this->fileFullPath = fileFullPath;
    yd = new YDAPI(this);
    yd->setToken(qSett.value("token").toString());
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
