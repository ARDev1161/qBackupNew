#ifndef TASKUPLOADER_H
#define TASKUPLOADER_H

#include <QObject>
#include <backuptask.h>
#include <ydapi.h>

class taskUploader : public QObject
{
    Q_OBJECT
public:
    explicit taskUploader(backupTask *task, QString fileFullPath, QObject *parent = nullptr);

signals:
    void finished();

public slots:
    void upload();

private slots:
    void uploadToFolder();

private:

    QString fileFullPath;
    QSettings qSett;
    backupTask *task;
    YDAPI *yd;
};

#endif // TASKUPLOADER_H
