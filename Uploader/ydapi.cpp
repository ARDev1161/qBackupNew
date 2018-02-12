#include "ydapi.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
/*
#include <QFileDialog>
#include <QMessageBox>*/

#define NAMOR_API_CONNECT_REPLY_FINISHED(x)    connect(m_reply, SIGNAL(finished()), SLOT(x()))

YDAPI::YDAPI(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

QString YDAPI::getErrorMessage()
{
    return m_errorMessage;
}

void YDAPI::setToken(QString token)
{
    m_token = token;
}

//void YDAPI::upload(QString fileName)
//{
//    m_fileFullName = fileName;
//    m_fileName = QFileInfo(m_fileFullName).fileName();
//    uploadFile();
//}

void YDAPI::createFolder(QString folderName)
{
    this->m_YDfolderName = folderName;
    QString url = "https://cloud-api.yandex.net:443/v1/disk/resources?path=app:/";
    url.append(m_YDfolderName);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(m_token.toUtf8()));

    //connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(folderCreated()));
    //m_manager->put(request, QByteArray(0));
    if(startRequest(request, namPUT)) {
        connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(folderCreated()));
    }
}

void YDAPI::upload(QString fileName)
{

    m_fileFullName = fileName;
    m_fileName = QFileInfo(m_fileFullName).fileName();

    QString url = "https://cloud-api.yandex.net:443/v1/disk/resources/upload?overwrite=true&path=app:/";
    url.append(m_YDfolderName + "/");
    url.append(m_fileName);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(m_token.toUtf8()));

//    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFilePUT(QNetworkReply*)));
//
//    m_manager->get(request);

    if(startRequest(request, namGET))
        NAMOR_API_CONNECT_REPLY_FINISHED(uploadFilePUT);
}

void YDAPI::uploadFilePUT()
{
    //disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFilePUT(QNetworkReply*)));
    QNetworkReply *reply = m_reply;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject jsonObj = jsonDoc.object();

    QString href = jsonObj["href"].toString();          //PUT upload url
    if(href.isEmpty()){
        emit onErrorInRequest(jsonObj.value("error").toString(), jsonObj.value("message").toString());
        return;
    }
    QFile *file = new QFile(m_fileFullName);
    if(!file->open(QIODevice::ReadOnly)){
        qDebug() << "Dont read";
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(href));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(m_token.toUtf8()));

    //connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished()));

    //m_manager->put(request, file);
    if(startRequest(request, namPUT, "", file))
        NAMOR_API_CONNECT_REPLY_FINISHED(onUploadFilePUT);
}

void YDAPI::onUploadFilePUT()
{
    QNetworkReply *reply = m_reply;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject jsonObj = jsonDoc.object();

    if(jsonObj.find("error") != jsonObj.end()){
        emit onErrorInRequest(jsonObj.value("error").toString(), jsonObj.value("message").toString());
        return;
    }
    emit finished();
}

void YDAPI::onSslErrors(QList<QSslError> errors)
{
    QString message;
    for (int i = 0; i < errors.count(); i++)
        message += errors[i].errorString() + "\n";

    setLastError(1, message);
}

bool YDAPI::startRequest(const QNetworkRequest &request, YDAPI::NamorRequestMethod method, const QString &data, QIODevice *io)
{
    QNetworkReply *reply;

    switch (method) {
    case namGET:
        reply = m_manager->get(request);
        break;

    case namPOST:
        if(io != NULL) {
            reply = m_manager->post(request, io);
        } else {
            reply = m_manager->post(request, data.toUtf8());
        }
        break;

     case namPUT:
        if(io != NULL) {
            reply = m_manager->put(request, io);
        } else {
            reply = m_manager->put(request, data.toUtf8());
        }
        break;

    case namDELETE:
        reply = m_manager->deleteResource(request);
        break;

    default:
        return setLastError(1, "Not implemented");
    }
    m_reply = reply;

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
    if(method == namPUT || method == namPOST)
        connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));

    return true;
}

bool YDAPI::setLastError(int code, const QString message)
{
    m_errorCode = code;

    if(code != 0) {
        m_retry++;
        m_errorMessage = QString("[%1]: %2").arg(code).arg(message);
        emit onError(this);
    } else {
        m_retry = 0;
        m_errorMessage = "OK";
    }
    return code == 0;
}

//void YDAPI::fileInfoHandler(QNetworkReply *reply)
//{
//    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileInfoHandler(QNetworkReply*)));
//    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
//    QJsonObject *jsonObj = new QJsonObject(jsonDoc.object());
//
//    emit fileInfoFinished(jsonObj);
//}
//
