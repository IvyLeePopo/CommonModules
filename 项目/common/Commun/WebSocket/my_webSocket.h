/*
 * @Description: 自定义socket
 * @Author: Ivy
 * @Date: 2022-05-18 09:34:43
 * @LastEditTime: 2022-05-18 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
#include <QObject>
#include<QWebSocket>
#include<QTimer>

class MyWebSocket : public QObject
{
    Q_OBJECT
public:
    MyWebSocket(QObject *parent = nullptr);
    ~MyWebSocket();
    void SetUrl(QString strUrl="");
    QString getPeerAddress();

signals:
    void sigMessageReceived(const QString &msg);
    void sigBinaryMessageReceived(const QByteArray &msg);

public slots:
    void slotWebSocketInit();       //初始化WebSocket
    void slotSendTextMessage(const QString &msg);
    void slotReconnect();           //周期重连函数
    void slotActiveReconnect();
    void slotConnected();         //socket建立成功后，触发该函数
    void slotMessageReceived(const QString& msg);   //收到Sev端的数据时，触发该函数
    void slotBinaryMessageReceived(const QByteArray& msg);
    void slotDisConnected();      //socket连接断开后，触发该函数

private:
    QWebSocket *m_WebSocket = nullptr;
    QString m_strURL = "";          //连接URL
    int m_iWsType = 0;

	bool connectStatus = false;         //websocket连接状态，连接成功：true；断开：false
	QTimer *m_timer = nullptr;          //周期重连Timer
};