#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();
    void loadSettings();

private slots:
    void on_getTokenButton_clicked();

    void on_buttonBox_accepted();

    void on_checkBox_toggled(bool checked);

private:
    QSettings qSett;
    Ui::settings *ui;
};

#endif // SETTINGS_H
