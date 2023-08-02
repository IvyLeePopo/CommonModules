#pragma once

#include "WebsocketThreadBase.h"

// 回调函数，将websocket的结果抛给上层
typedef void(*recvMsgToSerial)(const char* pszData, unsigned int  nSize);

class WebSocketServer : public WebSocketThreadBase
{
public:
	WebSocketServer(void);
	virtual ~WebSocketServer(void);

	// 启动服务器 
	bool startServer(/*const char* pszIp, const int nPort*/);

	// 关闭服务器 
	void stopServer();

	// 发送数据到客户端
	bool sendData(const char* pszJson, unsigned int nSize);

	// 接收客户端数据
	bool receiveData(const char* pszData, unsigned int nSize);

	// 设置回调函数
	void setCallBack(recvMsgToSerial pFunc, void* pUser = NULL);

private:
	// 初始化websocket
	void init();

private:
	void* m_pUser;							// 返回回调的对象
	recvMsgToSerial m_pRecvMsgToSerial;		// 回调
};
