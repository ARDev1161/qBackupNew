#include "backuper.h"
#include "backuptask.h"

#ifdef Q_OS_WIN32
#include "quazip/JlCompress.h"
#endif

#ifdef Q_OS_LINUX
#include "quazip5/JlCompress.h"
#endif


Backuper::Backuper(QString fileCompressed, QString inputFolder, QObject *parent) : QObject(parent)
{
    this->fileCompressed = fileCompressed;
    this->inputFolder = inputFolder;
}


void Backuper::runBackup()
{
    bool ok = JlCompress::compressDir(fileCompressed, inputFolder, true, QDir::AllDirs);
    emit backupFinished();
}
