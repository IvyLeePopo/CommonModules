#ifndef AWAITFORHTTPREQUEST_H
#define AWAITFORHTTPREQUEST_H

#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QEventLoop>
#include <QTimer>
#include <QSharedPointer>
#include <QDebug>

class TDeleteNetworkReply
{
public:
    void operator()(QNetworkReply* pReply)
    {
        if(pReply->isFinished()||pReply->isOpen())
        {
            pReply->abort();
            pReply->close();
        }
        pReply->deleteLater();
    }
};

class TDeleteHttpMultiPart
{
public:
    void operator()(QHttpMultiPart* pMultiPart)
    {
        pMultiPart->deleteLater();
    }
};

using TDataPacket = QMap<QString, QVariantMap>;
using TShareNetworkReply  = QSharedPointer<QNetworkReply>;
using TShareHttpMultiPart = QSharedPointer<QHttpMultiPart>;

class AWaitForHttpRequest : public QObject
{
    Q_OBJECT
public:
    AWaitForHttpRequest(qint32 timeout=1000);

    ~AWaitForHttpRequest();

    bool isTimeout(void){return m_isTimeout;}

    TShareNetworkReply post(const QNetworkRequest& request, const QByteArray& data);
    TShareNetworkReply post(const QNetworkRequest& request, TShareHttpMultiPart multiPart);
    TShareNetworkReply get (const QNetworkRequest& reqeust);
    TShareNetworkReply put (const QNetworkRequest& request, const QByteArray& data);
    TShareNetworkReply put (const QNetworkRequest& request, TShareHttpMultiPart multiPart);
protected:
    void beginPost(void);
    void endPost(void);

    bool checkReplyError(TShareNetworkReply reply);
protected slots:
    void slt_finished(QNetworkReply* reply);
    void slt_timeout(void);
protected:
    QNetworkAccessManager* m_manager;
    QEventLoop m_loop;
    QTimer m_timer;
    qint32 m_timeout;
    bool m_isTimeout;
    bool m_isReady;
};

#endif // AWAITFORHTTPREQUEST_H
