/*
 * @Description: 自定义服务端
 * @Author: Ivy
 * @Date: 2022-05-18 09:34:43
 * @LastEditTime: 2022-05-18 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include<QObject>

/*
*	QT_FORWARD_DECLARE_CLASS 是 Qt 框架中的一个宏定义，用于前向声明一个类。
	它的作用是在不需要完整定义类的情况下，告诉编译器这个类的存在，以便在编译时能够通过类名来访问它的成员和方法。
	这样做可以提高编译速度，避免头文件的循环依赖，同时也可以减小编译产生的中间文件的大小。
*/
QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class MyServer : public QObject
{
    Q_OBJECT

public:
    MyServer(const QString& serverName,  quint16 port);
    virtual ~MyServer();

public:
    //检测是否存在客户端
    bool hadClients();

    //获取本机IP
    QString getLocalHostIP();

public slots:
    //发送文本给所有客户端
    void slotSendToAllClients(const QString &strMsg);

    //启动websocket服务器
    void slotStartServer();

private slots:
    //处理新接入的连接
    void slotNewConnection();

    //接收数据，并转发给其他所有客户端节点
    void slotProcessMessage(const QString &strMsg);

    //处理链接断开的事件
    void slotSocketDisconnected();

signals:
	//回馈数据的信号，给客户端
	void sigProcessServerMessage(const QString & strMsg);

private:
    QWebSocketServer* m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    unsigned short m_nPort;
    QString m_strServerName;
};
