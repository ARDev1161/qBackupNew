#ifndef BACKUPTASK_H
#define BACKUPTASK_H

#include <QDialog>
#include <QSettings>
#include <QTime>
#include <ydapi.h>

namespace Ui {
class backupTask;
}

class backupTask : public QDialog
{
    Q_OBJECT

public:
    explicit backupTask(QWidget *parent = 0);
    backupTask(QString taskName);
    ~backupTask();

    void runBackup();

    QString name();
    QString dir();
    QString backupDir();
    QTime backupTime();

    bool enabledAutoBackup() const;

signals:
    void trayMessageSignal(bool ok, QString messageText);

private slots:
    void finishedUpload();
    void finishedBackup();
    void finishedCreateFolder();

    void on_buttonBox_accepted();

    void on_dirButton_clicked();

    void on_backupDirButton_clicked();

private:
    void uploadOnYD();
    void loadSettings();



    bool _enabledAutoBackup;
    bool _enableUpload;
    bool _poweroff;
    QTime _backupTime;
    QSettings qSett;
    QString _fileName;
    QString _name;
    QString _dir;
    QString _backupDir;
    YDAPI *ydapi;
    Ui::backupTask *ui;
};

#endif // BACKUPTASK_H
