#include "zipper.h"
#include "namorzipper.h"

zipper::zipper(bool dir,
               QString path,
               QString archiveName,
               QObject *parent) : QObject(parent),
   isDir(dir),
   path(path),
   archiveName(archiveName)
{

}

void zipper::start()
{
    NamorZipper *zipper = new NamorZipper(this);
    connect(zipper, SIGNAL(onCompressError(QString)), this, SIGNAL(compressError(QString)));
    connect(zipper, SIGNAL(onZipProgress(qint64,qint64)), this, SIGNAL(compressProgress(qint64,qint64)));
    if(isDir){
        connect(zipper, SIGNAL(onCompressDirSucces()), this, SIGNAL(compressFinished()));
        zipper->compressDir(path, archiveName);
    } else {
        connect(zipper, SIGNAL(onCompressFileSucces()), this, SIGNAL(compressFinished()));
        zipper->compressFile(path, archiveName);
    }
}
