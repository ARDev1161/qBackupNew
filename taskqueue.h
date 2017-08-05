#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QDialog>
#include <QList>
#include "backuptask.h"
#include "backuper.h"

namespace Ui {
class taskQueue;
}

class taskQueue : public QDialog
{
    Q_OBJECT

public:
    explicit taskQueue(QWidget *parent = 0);
    ~taskQueue();

    void enqueue(backupTask *task);

signals:
    void backupFinished();

private slots:
    void upload();
    void start();
    void updateProgressBar();
    void afterUpload();
    void clear();

    void on_pushButton_clicked();

private:
    Ui::taskQueue *ui;

    short int numberOfOps;
    short int completeOps;

    bool isWorking;

    QList<backupTask*> queue;
    backupTask *currentTask;
    short int currentIndex;
    QString currentFileName;

};

#endif // TASKQUEUE_H
