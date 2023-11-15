#include "websocketserverbase.h"

#include<QDebug>

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       peer->origin());
}

WebsocketServerBase::WebsocketServerBase(QString serverName,  quint16 port, QObject *parent)
    : QObject(parent)
    ,m_nPort(port)
    ,m_strServerName(serverName)
{
    qDebug() << "new WebsocketServerBase!";
}

WebsocketServerBase::~WebsocketServerBase()
{
    m_pWebSocketServer->deleteLater();
}

//接收到串口库发来的信息，转发给读卡器
void WebsocketServerBase::slotSendToAllClients(const QByteArray &data)
{
    qDebug() << __FUNCTION__;

    for (QWebSocket *pClient : qAsConst(m_clients))
    {
        qDebug() << __FUNCTION__ << "接收到串口库发来的信息，转发给读卡器";
         pClient->sendBinaryMessage(data);
    }
}

bool WebsocketServerBase::hadClients()
{
    return m_clients.size()>0;
}

void WebsocketServerBase::slotStartServer()
{
    if(m_pWebSocketServer)
        return;

    m_pWebSocketServer = new QWebSocketServer(m_strServerName, QWebSocketServer::NonSecureMode, this);

    if (m_pWebSocketServer->listen(QHostAddress::Any, m_nPort))
    {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &WebsocketServerBase::slotNewConnection);
        qDebug() << "WebSocket服务器已打开,监听端口号:" << m_nPort;
    }
}

void WebsocketServerBase::slotNewConnection()
{
    auto pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";

    qDebug() << "client connected!";

    pSocket->setParent(this);

	//二进制数据的接收
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebsocketServerBase::slotProcessBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebsocketServerBase::slotSocketDisconnected);

    m_clients << pSocket;
}

void WebsocketServerBase::slotSocketDisconnected()
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
	if (pClient)
	{
		m_clients.removeAll(pClient);
		pClient->deleteLater();
	}
}

//接收读卡器发来的数据，并转发给串口库
void WebsocketServerBase::slotProcessBinaryMessage(const QByteArray &data)
{
    qDebug() << "接收读卡器发来的数据，并转发给串口库";
    qDebug() << "recv data : " + data;

    //实际只有一个客户端
    emit sigProcessServerMessage(data);

    //test
    slotSendToAllClients(data);
}
