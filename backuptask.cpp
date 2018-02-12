#include "backuptask.h"

backupTask::backupTask(QString name, QObject *parent) : QObject(parent)
{
    this->name = name;
    this->loadSettings();
}

//backupTask::backupTask(QString name) : QObject(NULL)
//{
//}

QString backupTask::getName() const
{
    return name;
}

QString backupTask::getInputFolder() const
{
    return inputFolder;
}

void backupTask::setInputFolder(const QString &value)
{
    inputFolder = value;
}

QString backupTask::getOutputFolder() const
{
    return outputFolder;
}

void backupTask::setOutputFolder(const QString &value)
{
    outputFolder = value;
}

QTime backupTask::getAutoBackupTime() const
{
    return autoBackupTime;
}

void backupTask::setAutoBackupTime(const QTime &value)
{
    autoBackupTime = value;
}

bool backupTask::getAutoBackup() const
{
    return autoBackup;
}

void backupTask::setAutoBackup(bool value)
{
    autoBackup = value;
}

bool backupTask::getUpload() const
{
    return upload;
}

void backupTask::setUpload(bool value)
{
    upload = value;
}

void backupTask::loadSettings()
{
    this->setInputFolder(qSett.value(QString("%1/DirName").arg(this->getName())).toString());
    this->setOutputFolder(qSett.value(QString("%1/BackupDirName").arg(this->getName())).toString());
    this->setAutoBackupTime(qSett.value(QString("%1/Time").arg(this->getName())).toTime());
    this->setAutoBackup(qSett.value(QString("%1/Enabled").arg(this->getName())).toBool());
    this->setUpload(qSett.value(QString("%1/Upload").arg(this->getName())).toBool());
}
