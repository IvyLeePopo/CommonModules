#pragma once

#include "WebsocketThreadBase.h"
#include "WebSocketBase.h"

class WebSocketServer: public WebSocketThreadBase
{
public:
	WebSocketServer(void);
	virtual ~WebSocketServer(void);

	virtual int circle();

	// 启动服务器 
	bool startServer(/*const char* pszIp, const int nPort*/);

	// 关闭服务器 
	void stopServer();

	// 发送数据到客户端
	bool sendData(const char* pszJson, int nSize);

	// 接收客户端数据
	bool receiveData(const char* pszData, int nSize);

	// 接收读卡器从串口传送过来的数据
	void readFromReader();

private:
	// 初始化websocket
	void init();

	// 反初始化
	void unInit();

private:
	//WebSocketBase* m_pWebSocketBase;

	char*  m_pszRecvDataReader; // 读卡器从串口传送过来的数据
};
