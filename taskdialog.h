#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>
#include "backuptask.h"

namespace Ui {
class taskDialog;
}

class taskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit taskDialog(backupTask *inputTask, QWidget *parent = nullptr);
    ~taskDialog();

private slots:
    void on_inputFolderButton_clicked();

    void on_outputFolderButton_clicked();

    void on_buttonBox_accepted();

private:
    void loadSettings();
    void saveSettings();

    backupTask *task;
    QSettings qSett;
    Ui::taskDialog *ui;
};

#endif // TASKDIALOG_H
