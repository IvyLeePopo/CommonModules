/*
 * @Description: tcp client 基类
 * @Author: Ivy
 * @Date: 2022-04-09 09:34:43
 * @LastEditTime: 2023-03-17 18:31:20
 * @LastEditors: XTZJ-2022OQEHLZ
 */
#pragma once
#include <string>
#include <QObject>
#include <QAbstractSocket>

using namespace std;

class QTcpSocket;
class QTimer;
class TcpClientBase :public QObject
{
    Q_OBJECT

public:
    enum EnumResult
    {
        EnumResult_OK = 0,                      // 执行成功
        EnumResult_ConnectFail = 400,           // 连接服务失败
        EnumResult_TimeOut = 401,               // 超时错误
        EnumResult_DataNone = 402,              // 数据为空
        EnumResult_DataErr = 403,               // 数据错误
        EnumResult_FrameIncomplete = 404,       // 数据有分包
    };

public:
    TcpClientBase();
    virtual ~TcpClientBase();
    
public:
    // 连接tcp服务器
    virtual int connectSvr(const char* pszIp, int nPort);

    // 断开tcp服务器
    virtual int disConnect();

    // 接收数据
    virtual int recv(const char* pszData, int nLen);
    
    // 发送数据
    virtual int send(const char* pszData, int nLen);

    // 获取连接状态
    virtual int isConnected() { return m_bIsConnected; }

    // 获取ip
    virtual string getIp() { return m_strIp; }
signals:
    void signalRecv(QByteArray byteArray);

private slots:
    // 获取消息的信号
    void slotRecvMsg();
    
    // tcp 读取的错误信息
    void slotReadError(QAbstractSocket::SocketError);

    // tcp 已连接信号
    virtual void slotConnected();

    // tcp 断开连接信号
    virtual void slotDisConnected();

    // tcp 重连信号
    void slotReconnection();

    // 接收数据
    void slotRecv(QByteArray byteArray);

protected:
    int m_nPort;                // port
    string m_strIp;             // ip
    void* m_pParent;            // parent句柄 
    bool m_bIsConnected;        // 是否已经连接了
    QTcpSocket* m_pSocket;      // tcp socket句柄 
    QTimer* m_pConnectTimer;    // 连接服务器的定时器
};
