#include "WebSocketClientApp.h"
#include <QTimer>
#include <QThread>
#include "WebSocketClient.h"

#define DELETEOBJECT(_object) \
    if(NULL != _object) \
    {\
        delete _object; \
        _object = NULL; \
    }

#define CMD_HEART_BEAT "CMD_HEART_BEAT"  //App心跳


WebSocketClientApp::WebSocketClientApp()
{
	m_thdClient = new QThread();
	m_wsClient = new WebSocketClient();
	m_wsClient->setWebSocketUrl(URL);//设置url：ws://127.0.0.1:9091
	m_wsClient->setWebSocketType(2);

	m_wsClient->moveToThread(m_thdClient);//新开一个线程执行
	connect(m_thdClient, &QThread::started, m_wsClient, &WebSocketClient::slotCreateDataRecWS);//线程启动时候，开始连接服务器
	connect(m_wsClient, &WebSocketClient::sigClientTextMessageReceived, this, &WebSocketClientApp::slotRecvData);
	connect(this, &WebSocketClientApp::sigSendToApp, m_wsClient, &WebSocketClient::slotSendTextMessage);
	connect(this, &WebSocketClientApp::sigReconnectApp, m_wsClient, &WebSocketClient::slotActiveReconnect);

	connect(m_thdClient, &QThread::finished, m_wsClient, &WebSocketClient::deleteLater);
	connect(m_thdClient, &QThread::finished, m_thdClient, &QThread::deleteLater);
	m_thdClient->start();//线程启动

	m_timerApp = new QTimer(this);
	connect(m_timerApp, &QTimer::timeout, this, &WebSocketClientApp::slotHeartBeatToApp);
	m_nHeartBeatTimeOut = 0;
	m_timerApp->start(10 * 1000);
}

WebSocketClientApp::~WebSocketClientApp()
{
    m_thdClient->quit();
    m_thdClient->wait();

    DELETEOBJECT(m_wsClient);
    DELETEOBJECT(m_timerApp);
}

//接收到服务器发来的消息
void WebSocketClientApp::slotRecvData(const QString &message)
{

	//解析发来的message，具体看对应的协议
    string strApikey = "";
    parseRecvAPIKEY(message.toStdString(), strApikey);
    if (strApikey == CMD_HEART_BEAT) 
	{
        m_nHeartBeatTimeOut = 0;
    }
    else 
	{
        WriteLogInfo("收到其他类型数据：" + message.toStdString());
    }

    return;
}

void WebSocketClientApp::slotHeartBeatToApp()
{
    Json::Value root;
    root["apiKey"] = Json::Value(CMD_HEART_BEAT);
    Json::FastWriter fast_writer;
    string str = fast_writer.write(root).c_str();
    QString message = QString::fromStdString(str);
    emit sigSendToApp(message);
    m_nHeartBeatTimeOut++;

    if(m_nHeartBeatTimeOut > 3)
    {
        WriteLogWarning("未收到App的心跳应答，开启websocket主动重连...");
        emit sigReconnectApp();
        m_nHeartBeatTimeOut = 0;
        return;
    }

    return;
}

bool WebSocketClientApp::parseRecvAPIKEY(const string &json_para, string &apikey)
{
    CHECKJSON(json_para);
    GETWSTRINGFROMJSON("apiKey", jsonRoot, apikey);
    return true;
}
