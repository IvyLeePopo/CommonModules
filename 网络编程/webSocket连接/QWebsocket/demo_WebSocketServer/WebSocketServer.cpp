#include "WebSocketServer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QPair>
#include <QDebug>

WebSocketServer* WebSocketServer::m_pInstance = nullptr;

WebSocketServer::WebSocketServer()
{
    m_pServer = new QWebSocketServer("server",QWebSocketServer::NonSecureMode);
    connect(m_pServer, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
}

WebSocketServer::~WebSocketServer()
{
    delete  m_pSocket;
    m_pSocket = nullptr;

    if(nullptr != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

WebSocketServer* WebSocketServer::getInstance()
{
    if(nullptr == m_pInstance)
    {
        m_pInstance = new WebSocketServer();
        return m_pInstance;
    }

    return m_pInstance;
}

bool WebSocketServer::SetPort(int nPort)
{
    m_nPort = static_cast<quint16>(nPort);
    return true;
}

bool WebSocketServer::StartServer()
{
    m_pServer->listen(QHostAddress::Any, m_nPort);
    qDebug() << "WebSocket服务器已打开,监听端口号:" << m_nPort;

    return true;
}

bool WebSocketServer::StopServer()
{
    m_pServer->close();
    qDebug() << "WebSocket服务器已关闭.";

    return true;
}

void WebSocketServer::OnNewConnection()
{
    m_pSocket = m_pServer->nextPendingConnection();
    connect(m_pSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(MsgReceived(QString)));
    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(Disconnected()));

    QString strClient = m_pSocket->peerAddress().toString();
    qDebug() << QString("客户端:") + strClient + "连接到本服务器.";
    m_IsConnect = true;
    return;
}

void WebSocketServer::Disconnected()
{
    m_IsConnect = false;
    QString strClient = m_pSocket->peerAddress().toString();
    qDebug()<<QString("客户端:") + strClient + "已断开连接!";
}

void WebSocketServer::SlotSendMsg(QString strData)
{
    if(m_pSocket!=nullptr && m_IsConnect)
    {
        m_pSocket->sendTextMessage(strData);
    }
}

void WebSocketServer::MsgReceived(QString strMsg)
{
    QString strClient = m_pSocket->peerAddress().toString();
    QString strText = "接收到客户端" + strClient + "的数据: " + strMsg;
    qDebug() << strText;

}
