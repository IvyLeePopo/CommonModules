#include "WebSocketBase.h"
#include <QWebSocket>
#include <QTimer>
#include <QByteArray>
#include <cstring>

WebSocketBase::WebSocketBase(QObject *parent) : QObject(parent)
  ,m_pDataRecvWS(nullptr)
  ,m_pTimer(nullptr)
  ,m_strURL("")
  ,m_bConnectStatus(false)
  ,m_byteArray("")
{

}

WebSocketBase::~WebSocketBase()
{
    m_pTimer->stop();
    m_pTimer->deleteLater();
    m_pDataRecvWS->abort();
    m_pDataRecvWS->deleteLater();
}

void WebSocketBase::setWebSocketUrl(QString strUrl)
{
    m_strURL = strUrl;
    if(m_strURL.isEmpty())
    {
        m_strURL = "127.0.0.1";
    }
}

bool WebSocketBase::getConnectStatus()
{
    return m_bConnectStatus;
}

int WebSocketBase::RecvFrom(QByteArray &byteArray)
{
    byteArray = m_byteArray;
    m_byteArray.clear();
    return byteArray.size();
}

void WebSocketBase::slotCreateDataRecWS()
{
    if(nullptr == m_pTimer)
    {
        m_pTimer = new QTimer();
    }

    qDebug() << "Server Address" << m_strURL;

    if(m_pDataRecvWS == nullptr)
    {
        m_pDataRecvWS = new QWebSocket();
        connect(m_pDataRecvWS, &QWebSocket::disconnected, this, &WebSocketBase::slotOnDisConnected);
        connect(m_pDataRecvWS, &QWebSocket::binaryMessageReceived, this, &WebSocketBase::slotOnBinaryMessageReceived);
        connect(m_pDataRecvWS, &QWebSocket::connected, this, &WebSocketBase::slotOnConnected);
        connect(m_pTimer, &QTimer::timeout, this, &WebSocketBase::slotReconnect);
        m_pDataRecvWS->open(QUrl(m_strURL));
    }
}

void WebSocketBase::slotSendBinaryMessageMessage(const QByteArray &message)
{
    if (m_pDataRecvWS)
        m_pDataRecvWS->sendBinaryMessage(message);
}

void WebSocketBase::slotActiveReconnect()
{
    qDebug("try to Active Reconnect!!!");
    if(m_pDataRecvWS != nullptr)
    {
        m_bConnectStatus = false;
        m_pDataRecvWS->abort();
        qDebug("Exec Active Reconnect!");
        m_pDataRecvWS->open(QUrl(m_strURL));
    }

    return;
}

void WebSocketBase::slotReconnect()
{
    qDebug() << "try to reconnect:" << m_strURL;

    m_pDataRecvWS->abort();
    m_pDataRecvWS->open(QUrl(m_strURL));
}

void WebSocketBase::slotOnConnected()
{
    qDebug("WebSocketBase websocket is already connect!");

    m_bConnectStatus = true;
    m_pTimer->stop();
    qDebug() << "Address:" << m_strURL;
}

void WebSocketBase::slotOnDisConnected()
{
    qDebug() << "Address is disconnected:" << m_strURL;

    m_bConnectStatus = false;
    m_pTimer->start(3000);/*-<当连接失败时，触发重连计时器，设置计数周期为3秒 */
}

void WebSocketBase::slotOnBinaryMessageReceived(const QByteArray& byteArray)
{
//    QString hex_data = byteArray.toHex();
//    qDebug() << QString("[recv datac from server ] : byteArray[%1] = %2")
//                .arg(QString::number(hex_data.size()))
//                .arg(hex_data);

    m_byteArray = byteArray;
}
