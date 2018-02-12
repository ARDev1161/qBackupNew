#ifndef NAMORZIPPER_H
#define NAMORZIPPER_H

#include <QObject>
#include "quazip5/quazip.h"

class NamorZipper : public QObject
{
    Q_OBJECT
public:
    explicit NamorZipper(QObject *parent = nullptr);
    void compressFile(QString file, QString archiveFile);
    void compressDir(QString dir, QString archiveFile);
    static qint64 getTotalSize(QString dir);

signals:
    void onCompressError(QString errorMsg);
    void onCompressFileSucces();
    void onCompressDirSucces();
    void onZipProgress(qint64 zipped, qint64 total);

public slots:
private:
    void compressFile(QuaZip *zip, QString fileToCompress, QString fileName);
    void compressDir(QuaZip  *zip, QString dir, QString zipDir);

    bool copyData(QIODevice &inFile, QIODevice &outFile);
    bool isDirCompressing;

    qint64 totalSize;
    qint64 zippedSize;

};

#endif // NAMORZIPPER_H
