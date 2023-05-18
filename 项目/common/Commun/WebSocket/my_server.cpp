#include "my_server.h"
#include <QtWebSockets>

static QString getIdentifier(QWebSocket *peer)
{
	return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
		peer->origin());
}

MyServer::MyServer(const QString& serverName, quint16 port) :
	m_pWebSocketServer(nullptr)
	, m_nPort(port)
	, m_strServerName(serverName)
{
}

MyServer::~MyServer()
{
	m_pWebSocketServer->close();
	m_pWebSocketServer->deleteLater();
}

void MyServer::slotSendToAllClients(const QString &strMessage)
{
	for (QWebSocket *pClient : qAsConst(m_clients)) {
		pClient->sendTextMessage(strMessage);
	}
}

bool MyServer::hadClients()
{
	return m_clients.size() > 0;
}

QString MyServer::getLocalHostIP()
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

void MyServer::slotStartServer()
{
	if (m_pWebSocketServer)
		return;

	m_pWebSocketServer = new QWebSocketServer(m_strServerName, QWebSocketServer::NonSecureMode, this);

#if 1 //1-监听特定的ip, 0-监听任意的网络 QHostAddress::Any
	QString strLocalHostIp = getLocalHostIP();
	if (m_pWebSocketServer->listen(QHostAddress(strLocalHostIp), m_nPort))
	{
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &MyServer::slotNewConnection);
	}
#else
	if (m_pWebSocketServer->listen(QHostAddress::Any, m_nPort))
	{
		QTextStream(stdout) << mServerName << " listening on port " << m_nPort << '\n';
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &MyServer::slotNewConnection);
	}
#endif // 1

}

// 有新的连接会进入这个函数
void MyServer::slotNewConnection()
{
	auto pSocket = m_pWebSocketServer->nextPendingConnection();
	QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";

	pSocket->setParent(this);

	connect(pSocket, &QWebSocket::textMessageReceived, this, &MyServer::slotProcessMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &MyServer::slotSocketDisconnected);

	// 进入客户端列表
	m_clients << pSocket;
}

void MyServer::slotProcessMessage(const QString &strMessage)
{
	QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());

	for (QWebSocket *pClient : qAsConst(m_clients))
	{
		emit sigProcessServerMessage(strMessage);//实际只有一个客户端，

		if (pClient != pSender) //don't echo message back to sender
			pClient->sendTextMessage(strMessage);
	}
}

void MyServer::slotSocketDisconnected()
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
	if (pClient)
	{
		m_clients.removeAll(pClient);
		pClient->deleteLater();
	}
}
