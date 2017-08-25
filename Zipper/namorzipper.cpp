#include "namorzipper.h"
#include "quazip5/quazip.h"
#include "quazip5/quazipfile.h"
#include "quazip5/quazipnewinfo.h"

#include <QDir>
#include <QFile>

NamorZipper::NamorZipper(QObject *parent) : QObject(parent)
{

}

void NamorZipper::compressFile(QString file, QString archiveFile)
{
    QuaZip zip(archiveFile);
    QDir().mkpath(QFileInfo(archiveFile).absolutePath());
    if(zip.open(QuaZip::mdCreate) == false){
        QFile(archiveFile).remove();
        emit onCompressFileError();
        return;
    }
    compressFile(&zip, file, QFileInfo(file).fileName());
}

void NamorZipper::compressDir(QString dir, QString archiveFile)
{
    totalSize = NamorZipper::getTotalSize(dir);
    zippedSize = 0;

    QuaZip zip(archiveFile);
    QDir().mkpath(QFileInfo(archiveFile).absolutePath());
    if(zip.open(QuaZip::mdCreate) == false){
        QFile(archiveFile).remove();
        emit onCompressFileError();
        return;
    }
    compressDir(&zip, dir, dir);

    zip.close();
    if(zip.getZipError()!=0){
        QFile::remove(archiveFile);
        emit onCompressFileError();
        return;
    }
    emit onCompressDirSucces();
}

void NamorZipper::compressDir(QuaZip *zip, QString dir, QString zipDir)
{
    if(!zip){
        emit onCompressFileError();
        return;
    }
    if(zip->getMode() != QuaZip::mdAppend &&
            zip->getMode() != QuaZip::mdAdd &&
            zip->getMode() != QuaZip::mdCreate) {
        emit onCompressFileError();
        return;
    }
    QDir directory(dir);
    if(directory.exists() == false){
        emit onCompressFileError();
        return;
    }

    QDir zipDirectory(zipDir);
    if(dir != zipDir){
        QuaZipFile dirZipFile(zip);
        if(!dirZipFile.open(QIODevice::WriteOnly,
                            QuaZipNewInfo(zipDirectory.relativeFilePath(dir) + QDir::separator(), dir),
                            0, 0, 0)){
            emit onCompressFileError();
            return;
        }
        dirZipFile.close();
    }

    QFileInfoList subDirs = directory.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo subDir, subDirs){
        compressDir(zip, subDir.absoluteFilePath(), zipDir);
    }

    QFileInfoList files = directory.entryInfoList(QDir::Files);
    foreach(QFileInfo file, files){
        if(file.isFile() == false || file.absoluteFilePath() == zip->getZipName()) continue;

        QString fileName = zipDirectory.relativeFilePath(file.absoluteFilePath());
        compressFile(zip, file.absoluteFilePath(), fileName);
    }
}

void NamorZipper::compressFile(QuaZip *zip, QString fileToCompress, QString fileName)
{
    if(!zip){
        emit onCompressFileError();
        return;
    }
    if(zip->getMode() != QuaZip::mdAppend &&
       zip->getMode() != QuaZip::mdAdd &&
       zip->getMode() != QuaZip::mdCreate) {
        emit onCompressFileError();
        return;
    }

    QFile inFile;
    inFile.setFileName(fileToCompress);
    if(inFile.open(QFile::ReadOnly) == false){
        emit onCompressFileError();
        return;
    }

    QuaZipFile outFile(zip);
    if(outFile.open(QuaZipFile::WriteOnly, QuaZipNewInfo(fileName, inFile.fileName())) == false){
        emit onCompressFileError();
        return;
    }

    //zipping
    if(!copyData(inFile, outFile) || outFile.getZipError()!=UNZ_OK){
        emit onCompressFileError();
        return;
    }

    outFile.close();
    if(outFile.getZipError()!=UNZ_OK){
        emit onCompressFileError();
        return;
    }
    inFile.close();
    emit onCompressFileSucces();
}

bool NamorZipper::copyData(QIODevice &inFile, QIODevice &outFile)
{
    while (!inFile.atEnd()){
        char buff[4096];
        qint64 readLen = inFile.read(buff, 4096);
        if(readLen <= 0){
            return false;
        }
        if(outFile.write(buff, readLen) != readLen){
            return false;
        }
        zippedSize += readLen;
        emit onZipProgress(zippedSize, totalSize);
    }

    return true;
}

qint64 NamorZipper::getTotalSize(QString dir)
{
    qint64 size = 0;
    QDir directory(dir);
    QFileInfoList dirs = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo dirInfo, dirs) {
        size += getTotalSize(dirInfo.absoluteFilePath());
    }
    QFileInfoList files = directory.entryInfoList(QDir::Files);
    foreach(QFileInfo fileInfo, files){
        size += fileInfo.size();
    }
    return size;
}
