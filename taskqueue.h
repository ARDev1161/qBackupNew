#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QDialog>
#include <QList>
#include "backuptask.h"
#include "Uploader/ydapi.h"

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
    void start();
    void upload();
    void updateProgressBar();
    void updateUploadProgressBar(qint64 sent, qint64 total);
    void onUploadFinished();
    void clear();

    void on_pushButton_clicked();

    void uploadError(YDAPI *api);
    void uploadErrorInRequest(QString error, QString description);

private:
    Ui::taskQueue *ui;

    short int numberOfOps;
    short int completeOps;

    bool isWorking;

    QList<backupTask*> queue;
    backupTask *currentTask;
    short int currentIndex;
    QString currentFileName;

    QString genFileName(QString path, QString name);
    void writeToLog(QString logInfo);

};

#endif // TASKQUEUE_H
