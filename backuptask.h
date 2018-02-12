#ifndef BACKUPTASK_H
#define BACKUPTASK_H

#include <QObject>
#include <QTime>
#include <QSettings>


class backupTask : public QObject
{
    Q_OBJECT
public:
    explicit backupTask(QString name, QObject *parent = nullptr);
    //backupTask(QString name);

    QString getName() const;

    QString getInputFolder() const;
    void setInputFolder(const QString &value);

    QString getOutputFolder() const;
    void setOutputFolder(const QString &value);

    QTime getAutoBackupTime() const;
    void setAutoBackupTime(const QTime &value);

    bool getAutoBackup() const;
    void setAutoBackup(bool value);

    bool getUpload() const;
    void setUpload(bool value);

private:
    QString name;
    QString inputFolder;
    QString outputFolder;

    QTime autoBackupTime;

    bool autoBackup;
    bool upload;

    QSettings qSett;

    void loadSettings();

};

#endif // BACKUPTASK_H
