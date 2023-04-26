#include "WebSocketClient.h"
#include <QWebSocket>
#include <QTimer>
#include "GeneralHelp.h"

WebSocketClient::WebSocketClient(QObject *parent) : QObject(parent)
  ,m_bConnectStatus(false)
  ,m_timer(nullptr)
  ,m_pDataRecvWS(nullptr)
  ,m_strURL("")
  ,m_nWsType(0)
{

}

WebSocketClient::~WebSocketClient()
{
    m_timer->stop();
    m_timer->deleteLater();
    m_pDataRecvWS->abort();
    m_pDataRecvWS->deleteLater();
}

void WebSocketClient::setWebSocketUrl(QString strUrl)
{
    m_strURL = strUrl;
    if(m_strURL.isEmpty())
    {
        m_strURL = "127.0.0.1";
    }
}

void WebSocketClient::setWebSocketType(int iWsType)
{
    m_nWsType = iWsType;
}

void WebSocketClient::slotCreateDataRecWS()
{
    if(nullptr == m_timer)
    {
        m_timer = new QTimer();
    }

    WriteLogInfo("连接服务器，地址：" + m_strURL.toStdString());

	if (m_pDataRecvWS == nullptr)
	{
		m_pDataRecvWS = new QWebSocket();
		connect(m_pDataRecvWS, &QWebSocket::disconnected, this, &WebSocketClient::slotOnDisConnected, Qt::AutoConnection);
		connect(m_pDataRecvWS, &QWebSocket::textMessageReceived, this, &WebSocketClient::slotOnTextReceived, Qt::AutoConnection);
		connect(m_pDataRecvWS, &QWebSocket::connected, this, &WebSocketClient::slotOnConnected, Qt::AutoConnection);
		connect(m_timer, &QTimer::timeout, this, &WebSocketClient::slotReconnect, Qt::AutoConnection);
		m_pDataRecvWS->open(QUrl(m_strURL));
	}
}

void WebSocketClient::slotSendTextMessage(const QString &message)
{
    if (m_pDataRecvWS)
    {
        m_pDataRecvWS->sendTextMessage(message);
        if(1 == m_nWsType)
        {
            if( message.contains("ROBOT_DEVICE_STATU" ) ) //如果是心跳，省略打印内容。
            {
//                WriteLogInfo("发送[牌识服务]数据:  心跳数据,内容省略===");  //屏蔽心跳
            }
            else
            {
                WriteLogInfo("发送[牌识服务]数据:  " + message.toStdString());
            }

        }
        else if (2 == m_nWsType)
        {
            if (message != "{\"apiKey\":\"CMD_HEART_BEAT\"}")  //屏蔽心跳
                WriteLogInfo("发送[硬件控制]数据:  " + message.toStdString());
        }
    }

    return;
}


void WebSocketClient::slotActiveReconnect()
{
    WriteLogWarning("try to Active Reconnect!!!");
    if(m_pDataRecvWS != nullptr)
    {
        m_bConnectStatus = false;
        m_pDataRecvWS->abort();
        WriteLogInfo("Exec Active Reconnect!");
        m_pDataRecvWS->open(QUrl(m_strURL));
    }

    return;
}

void WebSocketClient::slotReconnect()
{
    WriteLogWarning("try to reconnect!");

    m_pDataRecvWS->abort();
    m_pDataRecvWS->open(QUrl(m_strURL));
}

void WebSocketClient::slotOnConnected()
{
    WriteLogInfo("WebSocketClient websocket is already connect!");

    m_bConnectStatus = true;
    m_timer->stop();
    WriteLogInfo("Address：" + m_pDataRecvWS->peerAddress().toString().toStdString());

    //websocket测试用
//    if (m_nWsType == 2) {
//        Json::Value root;
//        root["apiKey"] = Json::Value("CMD_ISSUANCE_INIT");
//        root["pos"] = "1";
//        Json::FastWriter fast_writer;
//        string str = fast_writer.write(root).c_str();
//        QString message = QString::fromStdString(str);

//        QTimer *timerTest = new QTimer;
//        timerTest->setSingleShot(false);
//        connect(timerTest, &QTimer::timeout, this, [=] {
//            WriteLogInfo("send CMD_ISSUANCE_INIT");
//            m_pDataRecvWS->sendTextMessage(message);
//        });
//        timerTest->start(120000);
//    }
}

void WebSocketClient::slotOnDisConnected()
{
    WriteLogInfo("Address is disconnected：" + m_strURL.toStdString());

    m_bConnectStatus = false;
    m_timer->start(3000);/*-<当连接失败时，触发重连计时器，设置计数周期为3秒 */
}


void WebSocketClient::slotOnTextReceived(const QString& msg)
{
    emit sigClientTextMessageReceived(msg);
}
