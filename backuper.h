#ifndef BACKUPER_H
#define BACKUPER_H

#include <QObject>
#include <backuptask.h>

class Backuper : public QObject
{
    Q_OBJECT
public:
    explicit Backuper(QString fileCompressed, QString inputFolder, QObject *parent = nullptr);

signals:
    void backupFinished();

public slots:
    void runBackup();

private:
    QString inputFolder;
    QString fileCompressed;
};

#endif // BACKUPER_H
