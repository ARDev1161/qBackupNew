#include <QTabWidget>
#include <QLineEdit>
#include <QFileDialog>
#include <QDir>
#include <QTimeEdit>
#include <QCheckBox>
#include <ydapi.h>
#include <QProcess>
#include <QThread>

#ifdef Q_OS_WIN32
#include "quazip/JlCompress.h"
#endif

#ifdef Q_OS_LINUX
#include "quazip5/JlCompress.h"
#endif

#include "backuper.h"
#include "backuptask.h"
#include "ui_backuptask.h"


backupTask::backupTask(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::backupTask)
{
    ui->setupUi(this);
}

backupTask::backupTask(QString taskName) :
    QDialog(NULL), ui(new Ui::backupTask)
{
    ui->setupUi(this);
    disconnect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    _name = taskName;
    ui->tabWidget->setTabText(0, _name);
    loadSettings();
}

QString backupTask::name(){
    return _name;
}

QString backupTask::dir(){
    return _dir;
}

QString backupTask::backupDir(){
    return _backupDir;
}

QTime backupTask::backupTime(){
    return _backupTime;
}

backupTask::~backupTask()
{
    delete ui;
}

void backupTask::runBackup()
{
    emit trayMessageSignal(true, tr("Starting backup: ") + _name);

    _fileName = this->_backupDir + QDir::separator() +
            this->_name + "_" + QDate::currentDate().toString("dd.MM.yyyy");

    QString tempFileName = _fileName + ".zip";

    if(QFile(tempFileName).exists()){
        bool exist = true;
        for(int i=0; exist != false; i++){
            QString buffFileName = _fileName+QString("(%1)").arg(i);
            if(!QFile(buffFileName+".zip").exists()){
                exist = false;
                _fileName = buffFileName+".zip";
            }
        }
    } else {
        _fileName += ".zip";
    }

    QThread *thread = new QThread();
    Backuper *threadBackuper = new Backuper(_fileName, this->_dir);
    threadBackuper->moveToThread(thread);

    connect(thread, SIGNAL(started()), threadBackuper, SLOT(runBackup()));
    connect(threadBackuper, SIGNAL(backupFinished()), thread, SLOT(quit()));
    connect(threadBackuper, SIGNAL(backupFinished()), this, SLOT(finishedBackup())); //after runBackup
    connect(threadBackuper, SIGNAL(backupFinished()), threadBackuper, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
    //////////////////////////////////////////////////////

void backupTask::finishedBackup()
{
    //succes = JlCompress::compressDir(_fileName, this->_dir, true, QDir::AllDirs);

    bool succes = QFile(_fileName).exists();

    if(succes)
        emit trayMessageSignal(succes, tr("Backuping succes: ") + _name);
    else
        emit trayMessageSignal(succes, tr("Backuping error: ") + _name);

    if(succes && _enableUpload){
        emit trayMessageSignal(true, tr("Starting upload: ") + _name);
        uploadOnYD();
    } else if(_poweroff){
#ifdef Q_OS_WIN32
        QProcess::startDetached("shutdown -s -f -t 00");
#endif
#ifdef Q_OS_LINUX
        QProcess::startDetached("shutdown -P now");
#endif
    }
}

void backupTask::on_buttonBox_accepted()
{
    _dir = ui->dirLineEdit->text();
    _backupDir = ui->backupDirLineEdit->text();
    _backupTime = ui->taskTimeEdit->time();
    _enabledAutoBackup = ui->enableTaskCB->isChecked();
    _poweroff = ui->poweroffCheckBox->isChecked();
    _enableUpload = ui->YDUpladCB->isChecked();

    qSett.setValue(QString("%1/DirName").arg(_name), _dir);
    qSett.setValue(QString("%1/BackupDirName").arg(_name), _backupDir);
    qSett.setValue(QString("%1/Time").arg(_name), _backupTime);
    qSett.setValue(QString("%1/Enabled").arg(_name), _enabledAutoBackup);
    qSett.setValue(QString("%1/PowerOff").arg(_name), _poweroff);
    qSett.setValue(QString("%1/Upload").arg(_name), _enableUpload);

    this->accept();
}

void backupTask::on_dirButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this, "Open dir", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->dirLineEdit->setText(path);
}

void backupTask::on_backupDirButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this, "Open backup dir", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->backupDirLineEdit->setText(path);
}

void backupTask::loadSettings()
{
    _dir = qSett.value(QString("%1/DirName").arg(_name)).toString();
    _backupDir = qSett.value(QString("%1/BackupDirName").arg(_name)).toString();
    _backupTime = qSett.value(QString("%1/Time").arg(_name)).toTime();
    _enabledAutoBackup = qSett.value(QString("%1/Enabled").arg(_name)).toBool();
    _poweroff = qSett.value(QString("%1/PowerOff").arg(_name)).toBool();
    _enableUpload = qSett.value(QString("%1/Upload").arg(_name)).toBool();

    ui->dirLineEdit->setText(_dir);
    ui->backupDirLineEdit->setText(_backupDir);
    ui->taskTimeEdit->setTime(_backupTime);
    ui->enableTaskCB->setChecked(_enabledAutoBackup);
    ui->poweroffCheckBox->setChecked(_poweroff);
    ui->YDUpladCB->setChecked(_enableUpload);
   // ui->YDBackupCheckBox->setChecked(qSett.value(QString("%1/YDEnabled").arg(name)).toBool());
}

bool backupTask::enabledAutoBackup() const
{
    return _enabledAutoBackup;
}

void backupTask::uploadOnYD()
{
    ydapi = new YDAPI();
    ydapi->setToken(qSett.value("token").toString());

    connect(ydapi, SIGNAL(finished()), this, SLOT(finishedCreateFolder()));
    ydapi->createFolder(this->_name);

}

void backupTask::finishedCreateFolder()
{
    disconnect(ydapi, SIGNAL(finished()), this, SLOT(finishedCreateFolder()));
    ydapi->upload(_fileName);
    connect(ydapi, SIGNAL(finished()), this, SLOT(finishedUpload()));
}

void backupTask::finishedUpload()
{
    disconnect(ydapi, SIGNAL(finished()), this, SLOT(finishedUpload()));
    ydapi->deleteLater();
    emit trayMessageSignal(true, tr("Uploading complete: ") + _name);
    if(_poweroff){
#ifdef Q_OS_WIN32
        QProcess::startDetached("shutdown -s -f -t 00");
#endif
#ifdef Q_OS_LINUX
        QProcess::startDetached("shutdown -P now");
#endif

    }
}
