#include "settings.h"
#include "ui_settings.h"
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>

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
    this->accept();
}

void settings::loadSettings()
{
    QString Token = qSett.value("token").toString();
    ui->YDlineEdit->setText(qSett.value("token").toString());
}
