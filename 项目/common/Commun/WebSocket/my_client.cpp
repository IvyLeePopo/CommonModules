#include "my_client.h"

#include "CommonPacketObj.h"
#include "json/json.h"
#include"thirdPart/runparameter.h"
#include "Config/setting.h"
#include "TcpServer/CarTypeRecognition.h"

MyClient *MyClient::self = nullptr;

MyClient::MyClient(QObject *parent)
{
    Q_UNUSED(parent);

    getUrl();
    m_thread = new QThread();
    m_myWebSocket = new MyWebSocket();
    m_myWebSocket->SetUrl(m_strUrl);
    m_myWebSocket->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_myWebSocket, &MyWebSocket::SlotWebSocketInit);  //WebSocketInit初始化
    connect(m_myWebSocket, &MyWebSocket::SigMessageReceived, this, &MyClient::SlotRecvTextMessage, Qt::AutoConnection);    //接收
    connect(this, &MyClient::sigSendTextMessage, m_myWebSocket, &MyWebSocket::SlotSendTextMessage, Qt::QueuedConnection);  //发送
    connect(this, &MyClient::sigActiveReconnect, m_myWebSocket, &MyWebSocket::SlotActiveReconnect, Qt::AutoConnection);    //重连

    connect(m_thread,&QThread::finished, m_myWebSocket, &MyWebSocket::deleteLater);
    connect(m_thread,&QThread::finished, m_thread, &QThread::deleteLater);
    m_thread->start();

    m_timerHeartBeat = new QTimer(this);
    connect(m_timerHeartBeat, &QTimer::timeout, this, &MyClient::SlotSendHeartBeat);    //定时发送心跳包
    m_timerHeartBeat->start(10000);  
}

MyClient::~MyClient()
{
    unRegister2Bus(KEY_BACKSTAGE_COMM);
    m_thread->quit();
    m_thread->wait();
    delete m_myWebSocket;
    delete m_timerHeartBeat;
    if(self != nullptr)
        delete self;
}

MyClient *MyClient::instance()
{
    if(self == nullptr)
        self = new MyClient();
    return self;
}

//获取后台通信URL
void MyClient::getUrl()
{
    if(Setting::value("/AddressUrl/Url").toString() == "")
    {
        m_strUrl = "ws://192.168.1.110:5551/JQR/ws?appId=006"; 
        Setting::setValue("/AddressUrl/Url", m_strUrl);
    }
    else
    {
        m_strUrl = Setting::value("/AddressUrl/Url").toString();
    }

    if(m_strUrl.contains("="))
        QStringList strUrlList = m_strUrl.split("=");
}

//接收数据
void MyClient::slotRecvTextMessage(const QString &msg)
{
	// todo
}

//定时发送心跳包
void MyClient::slotSendHeartBeat()
{
    Json::Value root;
    root["apiKey"] = CMD_HEART_BEAT;
    Json::FastWriter fast_writer;
    std::string str = fast_writer.write(root).c_str();
    QString message = QString::fromStdString(str);
    emit sigSendTextMessage(message);
    m_nHeartBeatCount++;

    if(m_nHeartBeatCount > 3)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "未收后台心跳应答，开启websocket主动重连...";
        emit sigActiveReconnect();
        m_nHeartBeatCount = 0;
        return;
    }

    return;
}