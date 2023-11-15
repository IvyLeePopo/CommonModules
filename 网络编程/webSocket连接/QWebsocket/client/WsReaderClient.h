/*
 * @Description: websocket客户端，用于与中间件通信
 */
#pragma once

#include <QObject>
#include <QCoreApplication>

#include "Singleton.h"

class WebSocketBase;
class QTimer;

class WsReaderClient : public QObject, public Singleton<WsReaderClient>
{
    Q_OBJECT
    friend class Singleton<WsReaderClient>;

public:
    WsReaderClient(QObject *parent = nullptr);
    ~WsReaderClient();

public:
    void SendTo(const QByteArray &byteArray);
    int RecvFrom(QByteArray& byteArray);

    bool getConnectStatus();

signals:
    //转发数据给server
    void sigSendToServer(const QByteArray &byteArray);

public slots:
    //接收服务器数据
    void slotRecvServerData(const QByteArray &byteArray);

    //发送服务器心跳包
    void slotHeartBeatToServer();

private:
    void readConfig();

private:
    WebSocketBase* m_wsReaderClient;
    QThread* m_thdReaderClient;
    QTimer *m_pTimerReader;
    int m_nHeartBeatTimeOutReader;
    QString m_URL = "";
};
