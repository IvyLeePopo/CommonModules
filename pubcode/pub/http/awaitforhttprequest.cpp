#include "awaitforhttprequest.h"

AWaitForHttpRequest::AWaitForHttpRequest(qint32 timeout)
    :m_manager(nullptr)
    ,m_timeout(timeout)
{
    m_manager = new QNetworkAccessManager;
    m_timer.setSingleShot(true);
    QObject::connect(&m_timer,SIGNAL(timeout()),this,SLOT(slt_timeout()));
    QObject::connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slt_finished(QNetworkReply*)));
}

AWaitForHttpRequest::~AWaitForHttpRequest()
{
    m_manager->clearAccessCache();
    m_manager->deleteLater();
}

TShareNetworkReply AWaitForHttpRequest::post(const QNetworkRequest& request, const QByteArray& data)
{
    beginPost();
    auto reply = TShareNetworkReply(m_manager->post(request,data),TDeleteNetworkReply());
    endPost();
    if(false==checkReplyError(reply))
    {
        return TShareNetworkReply(nullptr);
    }
    return reply;
}

TShareNetworkReply AWaitForHttpRequest::post(const QNetworkRequest& request, TShareHttpMultiPart multiPart)
{
    beginPost();
    auto reply = TShareNetworkReply(m_manager->post(request,multiPart.data()),TDeleteNetworkReply());
    endPost();
    if(false==checkReplyError(reply))
    {
        return TShareNetworkReply(nullptr);
    }
    return reply;
}

TShareNetworkReply AWaitForHttpRequest::get(const QNetworkRequest& request)
{
    beginPost();
    auto reply = TShareNetworkReply(m_manager->get(request),TDeleteNetworkReply());
    endPost();
    if(false==checkReplyError(reply))
    {
        return TShareNetworkReply(nullptr);
    }
    return reply;
}

TShareNetworkReply AWaitForHttpRequest::put(const QNetworkRequest& request, const QByteArray& data)
{
    beginPost();
    auto reply = TShareNetworkReply(m_manager->put(request,data),TDeleteNetworkReply());
    endPost();
    if(false==checkReplyError(reply))
    {
        return TShareNetworkReply(nullptr);
    }
    return reply;
}

TShareNetworkReply AWaitForHttpRequest::put(const QNetworkRequest& request, TShareHttpMultiPart multiPart)
{
    beginPost();
    auto reply = TShareNetworkReply(m_manager->put(request,multiPart.data()),TDeleteNetworkReply());
    endPost();
    if(false==checkReplyError(reply))
    {
        return TShareNetworkReply(nullptr);
    }
    return reply;
}

void AWaitForHttpRequest::beginPost(void)
{
    m_isTimeout = false;
    m_isReady = false;
    m_timer.start(m_timeout);
}

void AWaitForHttpRequest::endPost(void)
{
    m_loop.processEvents(QEventLoop::AllEvents);
    if(false==m_isTimeout&&false==m_isReady)
    {
        m_loop.exec();
    }
    if(m_timer.isActive())
    {
        m_timer.stop();
    }
}

bool AWaitForHttpRequest::checkReplyError(TShareNetworkReply reply)
{
    if(true==m_isTimeout)
    {
        qWarning() << QString("http request timeout[>%1 ms]").arg(m_timeout);
        reply->abort();
        return false;
    }
    if(false==reply->isOpen())
    {
        qWarning() << QString("Abnormal received data [no data returned]!");
        return false;
    }
    return true;
}

void AWaitForHttpRequest::slt_timeout(void)
{
    m_isTimeout = true;
    m_loop.quit();
}

void AWaitForHttpRequest::slt_finished(QNetworkReply* reply)
{
    (void)reply;
    m_isReady = true;
    m_loop.quit();
}
