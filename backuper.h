#ifndef BACKUPER_H
#define BACKUPER_H

#include <QObject>

class Backuper : public QObject
{
    Q_OBJECT
public:
    explicit Backuper(QObject *parent = nullptr);
    explicit Backuper(QString fileName, QString backupiedDir);

signals:
    void backupFinished();

public slots:
    void runBackup();

private:
    QString fileName;
    QString backupiedDir;
};

#endif // BACKUPER_H
