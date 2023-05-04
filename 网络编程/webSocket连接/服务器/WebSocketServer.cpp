#include "WebSocketServer.h"
#include <QtWebSockets>
#include "GeneralHelp.h"

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       peer->origin());
}

WebSocketServer::WebSocketServer(QString serverName, quint16 port):
    m_pWebSocketServer(nullptr)
  ,m_nPort(port)
  ,m_strServerName(serverName)
{

}

WebSocketServer::~WebSocketServer()
{
    m_pWebSocketServer->close();
    m_pWebSocketServer->deleteLater();
}

void WebSocketServer::slotSendToAllClients(const QString &strMessage)
{
    WriteLogInfo("向A节点发送消息" + strMessage.toStdString());
    for (QWebSocket *pClient : qAsConst(m_clients)) {
         pClient->sendTextMessage(strMessage);
    }
}

bool WebSocketServer::hadClients()
{
    return m_clients.size()>0;
}

QString WebSocketServer::getLocalHostIP()
{
    QString cmd = "ifconfig eth0 | grep 'inet ' | awk '{print $2}'";
    char result[512] = { 0 };
    FILE *fp = nullptr;
    if ((fp = popen(cmd.toLocal8Bit().data(), "r")) != nullptr)
    {
        fgets(result, sizeof(result), fp);
        pclose(fp);
        return QString(result).remove('\n');
    }
    return "";
}

void WebSocketServer::slotStartServer()
{
    if(m_pWebSocketServer)
        return;

    m_pWebSocketServer = new QWebSocketServer(m_strServerName, QWebSocketServer::NonSecureMode, this);

    QString strLocalHostIp = getLocalHostIP();

	/**
	QHostAddress::Any表示服务端监听所有可用的网络接口。
	它是一个特殊的IP地址，表示服务端可以接受来自任何IP地址的连接请求。
	这通常用于在一个计算机上运行多个网络服务时，让服务端能够监听所有可用的网络接口，
	以便接受来自不同网络接口的连接请求。
	*/
	//m_pWebSocketServer->listen(QHostAddress::Any, m_Port);
    if (m_pWebSocketServer->listen(QHostAddress(strLocalHostIp), m_nPort))
    {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &WebSocketServer::slotNewConnection);
    }
}

void WebSocketServer::slotNewConnection()
{
    auto pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";

    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSocketServer::slotProcessMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebSocketServer::slotSocketDisconnected);

    m_clients << pSocket;
}

void WebSocketServer::slotProcessMessage(const QString &strMessage)
{
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    for (QWebSocket *pClient : qAsConst(m_clients)) {

        emit sigProcessServerMessage(strMessage);//实际只有一个客户端，

        if (pClient != pSender) //don't echo message back to sender
            pClient->sendTextMessage(strMessage);
    }
}

void WebSocketServer::slotSocketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
