#include "settings.h"
#include "ui_settings.h"
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>
#include <QCheckBox>
#include <QTimeEdit>
#include <QDir>

#define APPLICATION_NAME "QBackup2"

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    disconnect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    loadSettings();
}

settings::~settings()
{
    delete ui;
}

void settings::on_getTokenButton_clicked()
{
    QString link =
            "https://oauth.yandex.ru/authorize?response_type=token&client_id=b646ea711501461cb0cdae42f6a673ee";
    QDesktopServices::openUrl(QUrl(link));
}

void settings::on_buttonBox_accepted()
{
    qSett.setValue("token", ui->YDlineEdit->text());
    qSett.setValue("powerOff", ui->poweroffCheckBox->isChecked());
    qSett.setValue("powerOffTime", ui->poweroffTimeEdit->time());
    qSett.setValue("autoStart", ui->autoStartCheckBox->isChecked());
    this->accept();
}

void settings::loadSettings()
{
    QString Token = qSett.value("token").toString();
    ui->YDlineEdit->setText(qSett.value("token").toString());
    ui->poweroffCheckBox->setChecked(qSett.value("powerOff").toBool());
    ui->poweroffTimeEdit->setTime(qSett.value("powerOffTime").toTime());
    ui->autoStartCheckBox->setChecked(qSett.value("autoStart").toBool());
}

void settings::on_checkBox_toggled(bool checked)
{
    qSett.setValue("autoStart", checked);
#ifdef Q_OS_WIN32
    if(checked){
        QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                          QSettings::NativeFormat);
        setting.setValue(APPLICATION_NAME,
                         QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " MINIMIZE");
        setting.sync();
    } else {
        QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                          QSettings::NativeFormat);
        setting.remove(APPLICATION_NAME);
        setting.sync();
    }
#endif

}
