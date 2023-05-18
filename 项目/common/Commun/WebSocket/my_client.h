/*
 * @Description: 自定义客户端
 * @Author: Ivy
 * @Date: 2022-05-18 09:34:43
 * @LastEditTime: 2022-05-18 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include <QObject>
#include <QWebSocket>
#include <QThread>
#include <QTimer>
#include <QUuid>
#include <QDateTime>
#include "my_webSocket.h"
#include "CommonPacketObj.h"

#define CMD_HEART_BEAT "CMD_HEART_BEAT"

class MyClient : public QObject
{
    Q_OBJECT
public:
    MyClient(QObject *parent = nullptr);
    ~MyClient();
    static MyClient *instance();
    static MyClient *self;

public:
    void getUrl();

public slots:
    void slotRecvTextMessage(const QString &msg);   //接收信息
    void slotSendHeartBeat();						//发送心跳包

signals:
    void sigSendTextMessage(const QString &msg);    //发送信息
    void sigActiveReconnect();						//发起重连

private:
    QThread *m_thread;
    QTimer *m_timerHeartBeat;
    MyWebSocket *m_myWebSocket;

	int m_nHeartBeatCount = 0;
	QString m_strUrl;
};
