/*
 * @Description: websocket客户端，用于与hardwareControl_App通信
 */
#pragma once
#include <QObject>
#include "Singleton.h"

class WebSocketClient;
class QThread;
class QTimer;
class WebSocketClientApp : public QObject, public Singleton<WebSocketClientApp>
{
    Q_OBJECT
    friend class Singleton<WebSocketClientApp>;

public:
    WebSocketClientApp();
    virtual ~WebSocketClientApp();

public:
    //解析从HardwareControl_App接收的数据，获取apiKey
    bool parseRecvAPIKEY(const string& json_para, string &apikey);

signals:
    //向App发送文本
    void sigSendToApp(const QString &message);

    //发起App重连信号
    void sigReconnectApp();

private slots:
    //接收App数据
    void slotRecvData(const QString &message);

    //发送心跳包
    void slotHeartBeatToApp();

private:
    WebSocketClient* m_wsClient;
    QThread* m_thdClient;
    QTimer *m_timerApp;
    int m_nHeartBeatTimeOut;
};
