#include "my_webSocket.h"

MyWebSocket::MyWebSocket(QObject *parent) :
    QObject(parent)
{

}

MyWebSocket::~MyWebSocket()
{
    m_timer->stop();
    m_timer->deleteLater();
    m_WebSocket->abort();
    m_WebSocket->deleteLater();
}

void MyWebSocket::SetUrl(QString strUrl)
{
    m_strURL = strUrl;
}

//创建连接
void MyWebSocket::slotWebSocketInit()
{
    qDebug() << __LINE__ << __FUNCTION__ << "连接服务器，地址："+m_strURL;

    if(m_WebSocket == nullptr)
    {
        m_WebSocket = new QWebSocket();
        connect(m_WebSocket, &QWebSocket::disconnected, this, &MyWebSocket::slotDisConnected, Qt::AutoConnection);
        connect(m_WebSocket, &QWebSocket::textMessageReceived, this, &MyWebSocket::slotMessageReceived, Qt::AutoConnection);
		connect(m_WebSocket, &QWebSocket::binaryMessageReceived, this, &MyWebSocket::slotBinaryMessageReceived, Qt::AutoConnection);
		connect(m_WebSocket, &QWebSocket::connected, this, &MyWebSocket::slotConnected, Qt::AutoConnection);
		m_WebSocket->open(QUrl(m_strURL));
	}

	if (m_timer == nullptr)
	{
		m_timer = new QTimer();
		connect(m_timer, &QTimer::timeout, this, &MyWebSocket::slotReconnect);
	}
}

//发送数据
void MyWebSocket::slotSendTextMessage(const QString &msg)
{
    if(m_WebSocket != nullptr)
    {
        m_WebSocket->sendTextMessage(msg);
    }
}

//主动重连
void MyWebSocket::slotActiveReconnect()
{
    if(m_WebSocket != nullptr)
    {
        connectStatus = false;
        m_WebSocket->abort();
        m_WebSocket->open(QUrl(m_strURL));
    }
}

//周期重连
void MyWebSocket::slotReconnect()
{
    m_WebSocket->abort();
    m_WebSocket->open(QUrl(m_strURL));
}

//连接成功
void MyWebSocket::slotConnected()
{
    connectStatus = true;
    m_timer->stop();
}

//断开连接：重连放在调用的地方
void MyWebSocket::slotDisConnected()
{
    connectStatus = false;
    //m_timer->start(3000);   //当连接失败时，触发重连计时器，设置计数周期为3秒
}

//接收数据(文本方式)
void MyWebSocket::slotMessageReceived(const QString& msg)
{
    emit sigMessageReceived(msg);
}

//接收数据(binary方式)
void MyWebSocket::slotBinaryMessageReceived(const QByteArray& msg)
{
    emit sigBinaryMessageReceived(msg);
}

// 获取WebSocket 连接的对端地址，即客户端的 IP 地址。
QString MyWebSocket::getPeerAddress()
{
    return m_WebSocket->peerAddress().toString();
}
