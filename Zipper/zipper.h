#ifndef ZIPPER_H
#define ZIPPER_H

#include <QObject>

class zipper : public QObject
{
    Q_OBJECT
public:
    explicit zipper(bool dir, QString path, QString archiveName, QObject *parent = nullptr);

signals:
    void compressFinished();
    void compressError();
    void compressProgress(qint64 compressed, qint64 total);

public slots:
    void start();

private:
    bool isDir;
    QString path;
    QString archiveName;

    void compressDir();
    void compressFie();
};

#endif // ZIPPER_H
