/*
 * @Description: tcp连接，会将接收到的消息抛出(基础的客户端)
 * @Author: Ivy
 * @Date: 2022-06-01 15:34:43
 * @LastEditTime: 2023-02-27 18:58:06
 * @LastEditors: XTZJ-2022OQEHLZ
 */
 
#pragma once
#include <string>
#include <queue>
#include "TcpClientBase.h"

class QTimer;
class TcpClient : public TcpClientBase
{
public:
    TcpClient(void* pParent, int nDeviceType = 0);
    virtual ~TcpClient();
	
	/************************* 重载基类的虚函数 start *******************************/
	// 发送消息
    virtual int send(tagTcpDataParams& param, bool bLog = true);

    // 接收消息
    virtual int recv(const char* pszData, int nLen);
    
    // 获取连接状态
    virtual int isConnected() { return m_bIsConnected; }
	
private slots:
    // 断线重连槽
    virtual void slotReConnect();

    // 已连接事件
    virtual void slotConnected();

    // 断线事件
    virtual void slotDisConnected();
	/************************* 重载基类的虚函数 end *******************************/

    // 心跳发送槽
    void slotHearBeat();

	
private: 
    // 从接收的数据解帧
    bool splitFrame(string &array);
	
private:
    int m_nDeviceType;          // 消息类型
    QTimer* m_pTimerHeartBeat;  // 心跳定时器
    QTimer* m_pTimerReconnect;  // 重连定时器
	
    string m_strEntireData;         //完整的TCP包，如果有分包，则需要重新组装
    string m_strUncompleteDatas;    //从远端读取回来的原始数据
    queue<string> m_queueFrameList; //待处理的帧数据队列
}
