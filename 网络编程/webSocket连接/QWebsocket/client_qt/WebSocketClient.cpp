#include "WebSocketClient.h"
#include "GlobalDefine.h"
#include "iostream"
#include <QJsonObject>
#include <QJsonDocument>

WebSocketClient::WebSocketClient():
    m_strIp(""),
    m_nPort(0),
    m_bIsConnect(false)
{
    m_pSocket = new QWebSocket();

    connect(m_pSocket, &QWebSocket::connected, this, &WebSocketClient::slotConnected, Qt::AutoConnection);
    connect(m_pSocket, &QWebSocket::disconnected, this, &WebSocketClient::slotDisconnected, Qt::AutoConnection);
    connect(m_pSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::slotRecv, Qt::AutoConnection);

}

WebSocketClient::~WebSocketClient()
{
    m_pSocket->abort();

    delete m_pSocket;
}

void WebSocketClient::connectSvr()
{
    if(m_bIsConnect)
    {
        m_pSocket->abort();
        m_pSocket->close();
    }
    QThread::msleep(100);

    QString strUrl = QString("ws://%1:%2").arg(m_strIp).arg(m_nPort);
    m_pSocket->open(QUrl(strUrl));
}

void WebSocketClient::disconnect()
{
    m_pSocket->abort();
    m_pSocket->close();
}

int WebSocketClient::send(QString strData)
{
    if(!m_bIsConnect)
    {
        qDebug()<<"WebSocket 未连接，无法发送数据";
        return Enum_NoConnect;
    }

    if(strData.isEmpty())
    {
        qWarning()<<"发送的数据为空";
        return Enum_DataNull;
    }

    if(!m_mutex.tryLock(10))
        return Enum_LockFailed;

    qDebug()<<"发送数据： " + strData;
    int nRet = m_pSocket->sendTextMessage(strData);

    m_mutex.unlock();

    return nRet;
}

void WebSocketClient::slotConnected()
{
    qDebug()<<QString("已连接到服务器 %1:%2").arg(m_strIp).arg(m_nPort);

    m_bIsConnect = true;

    emit signalConnectStatus(1);
}

void WebSocketClient::slotDisconnected()
{
    qDebug()<<"WebSocket 连接断开";

    m_bIsConnect = false;

    emit signalConnectStatus(-1);
}

void WebSocketClient::slotRecv(QString strData)
{
    qDebug()<<"接收数据：" + strData;
    emit signalRecvJsonData(strData);
}
