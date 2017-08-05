#include "taskdialog.h"
#include "ui_taskdialog.h"
#include <QLineEdit>
#include <QTimeEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QDir>

taskDialog::taskDialog(backupTask *inputTask, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskDialog)
{
    ui->setupUi(this);
    this->task = inputTask;
    disconnect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    this->setWindowTitle(task->getName());
    loadSettings();
}

taskDialog::~taskDialog()
{
    delete ui;
}

void taskDialog::on_inputFolderButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this, "Open dir", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->inputFolderLineEdit->setText(path);
}

void taskDialog::on_outputFolderButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this, "Open backup dir", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->outputFolderLineEdit->setText(path);
}

void taskDialog::on_buttonBox_accepted()
{
    saveSettings();
    this->accept();
    this->deleteLater();
}

void taskDialog::loadSettings()
{
    ui->inputFolderLineEdit->setText(task->getInputFolder());
    ui->outputFolderLineEdit->setText(task->getOutputFolder());
    ui->autoBackupTimeEdit->setTime(task->getAutoBackupTime());
    ui->autoBackupCheckBox->setChecked(task->getAutoBackup());
    ui->uploadCheckBox->setChecked(task->getUpload());
}

void taskDialog::saveSettings()
{
    task->setInputFolder(ui->inputFolderLineEdit->text());
    task->setOutputFolder(ui->outputFolderLineEdit->text());
    task->setAutoBackupTime(ui->autoBackupTimeEdit->time());
    task->setAutoBackup(ui->autoBackupCheckBox->isChecked());
    task->setUpload(ui->uploadCheckBox->isChecked());

    qSett.setValue(QString("%1/DirName").arg(task->getName()), task->getInputFolder());
    qSett.setValue(QString("%1/BackupDirName").arg(task->getName()), task->getOutputFolder());
    qSett.setValue(QString("%1/Time").arg(task->getName()), task->getAutoBackupTime());
    qSett.setValue(QString("%1/Enabled").arg(task->getName()), task->getAutoBackup());
    qSett.setValue(QString("%1/Upload").arg(task->getName()), task->getUpload());
}
