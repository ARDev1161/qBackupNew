#include "backuper.h"

#ifdef Q_OS_WIN32
#include "quazip/JlCompress.h"
#endif

#ifdef Q_OS_LINUX
#include "quazip5/JlCompress.h"
#endif


Backuper::Backuper(QObject *parent) : QObject(parent)
{

}

Backuper::Backuper(QString fileName, QString backupiedDir)
{
    this->fileName = fileName;
    this->backupiedDir = backupiedDir;
}

void Backuper::runBackup()
{
    JlCompress::compressDir(fileName, backupiedDir, true, QDir::AllDirs);
    emit backupFinished();
}
