#ifndef YDAPI_H
#define YDAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

class YDAPI : public QObject
{
    Q_OBJECT
public:
    explicit YDAPI(QObject *parent = 0);
    QString getErrorMessage();

signals:
    void finished();
    void folderCreated();

    void onError(YDAPI *api);
    void onErrorInRequest(QString error, QString description);

    void uploadProgress(qint64 sent, qint64 total);

public slots:
    void setToken(QString token);
    //void upload(QString fileName);
    void upload(QString fileName);

    void createFolder(QString m_YDfolderName);

private slots:
    void uploadFilePUT();
    void onUploadFilePUT();

    void onSslErrors(QList<QSslError> errors);
    /*****test slots******/
//    void uploadFilePUT(QString href);
//    void checkUpload(QNetworkReply *reply);

private:

    typedef enum {
        namGET,
        namPOST,
        namPUT,
        namDELETE
    } NamorRequestMethod;

    bool startRequest(const QNetworkRequest& request, NamorRequestMethod method = namGET,
                      const QString& data = "", QIODevice* io = NULL);

    bool setLastError(int code, const QString message ="");

    int m_errorCode;
    QString m_errorMessage;

    int m_retry;

    //QString fullFileName;
    QString m_fileName, m_fileFullName;
    QString m_YDfolderName;
    QString m_token;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;

    qint64 m_sent, m_total;
};

#endif // YDAPI_H
