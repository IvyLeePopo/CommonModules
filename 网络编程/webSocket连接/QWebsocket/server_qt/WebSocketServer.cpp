#include "WebSocketServer.h"
#include <IniSetting/inisetting.h>
#include <json/json.h>
#include <QtDebug>
#include <qdebug.h>
#include "global.h"

#define CHECKCONTENT(szData) \
    if(nullptr == szData) \
        return false; \
                      \
    QJsonDocument jsonDoc = QJsonDocument::fromJson(szData); \
    QJsonObject jsonObj = jsonDoc.object(); \
    QJsonObject bizContent = jsonObj["bizContent"].toObject(); \
    if(bizContent.empty()) \
    { \
        qDebug()<<"[Warnning] bizContent is empty !"; \
        return false; \
    }

WebSocketServer* WebSocketServer::m_pInstance = nullptr;

WebSocketServer::WebSocketServer()
{
    m_pServer = new QWebSocketServer("server",QWebSocketServer::NonSecureMode);
    connect(m_pServer, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()),this, SLOT(SlotSendSnNumber()));
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
    m_timer->start(500);
    m_IsConnect = true;
    return;
}

void WebSocketServer::Disconnected()
{
    m_IsConnect = false;
    QString strClient = m_pSocket->peerAddress().toString();
    qDebug()<<QString("客户端:") + strClient + "已断开连接!";
}

void WebSocketServer::SendMsg(QString strData)
{
    if(m_pSocket!=nullptr && m_IsConnect)
    {
        m_pSocket->sendTextMessage(strData);
    }
}

void WebSocketServer::MsgReceived(QString strMsg)
{
    QString strClient = m_pSocket->peerAddress().toString();


    QByteArray szData  = strMsg.toUtf8();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(szData , &jsonError);
    if(QJsonParseError::NoError != jsonError.error)
    {
        qDebug()<<"[Warnning]: data is not json !";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    int nCmd = jsonObj["cmd"].toInt();

    QString strText = "收到客户端数据: " + strMsg;
    if (nCmd != 201) {
        qDebug() << strText;
    }

    switch(nCmd)
    {
    case 201:
        parseHeartBeatMsg(szData);
        break;
    default:
        break;
    }
}

/******************************* 公共接口 *******************************/
//心跳包
bool WebSocketServer::parseHeartBeatMsg(QByteArray szData)
{
    CHECKCONTENT(szData);

    QString strDevUniqueId = bizContent["devUniqueID"].toString();

    Json::Value root;
    root["cmd"] = Json::Value(201);
    root["executeCode"] = Json::Value(0);

    //转化为字符串
    Json::FastWriter fast_writer;
    std::string str = fast_writer.write(root).c_str();

    QString Resp = QString::fromStdString(str);

    SendMsg(Resp);
    return true;
}

//Json封装发送
void WebSocketServer::SlotSendLogToWebSocket(int cmd, int executeCode, QString resultMsg)
{
    QString Resp;
    Json::Value root;
    root["cmd"] = Json::Value(cmd);
    root["executeCode"] = Json::Value(executeCode);
    root["resultMsg"] = Json::Value(resultMsg.toStdString());

    //转化为字符串
    Json::FastWriter fast_writer;
    std::string str = fast_writer.write(root).c_str();

    Resp = QString::fromStdString(str);
    qDebug() << "SlotSendLogToWebSocket:" << Resp;

    SendMsg(Resp);
}

void WebSocketServer::slotSendMsg(int ret)
{
    QString Resp;
    Json::Value root;
    root["cmd"] = Json::Value(404);
    if(ret == 0)
        root["executeCode"] = Json::Value(0);
    else
        root["executeCode"] = Json::Value(1006);

    //转化为字符串
    Json::FastWriter fast_writer;
    std::string str = fast_writer.write(root).c_str();

    Resp = QString::fromStdString(str);

    SendMsg(Resp);
}
