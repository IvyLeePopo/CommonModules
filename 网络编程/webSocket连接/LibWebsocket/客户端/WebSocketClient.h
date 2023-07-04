/*
	WebsocketClient类：客户端
*/

#pragma once
#include "ThreadBase.h"
#include "../CommInterface.h"

#define DEFAULT_RSU_PORT (9000)

class WebSocketBase;
class WebSocketClientInterface : /*CThreadBase , */public CommInterface
{
public:
	WebSocketClientInterface(void);
	virtual ~WebSocketClientInterface(void);
	//virtual int circle();

	virtual bool Connect(const char *pccArgs);
	virtual bool TestAndReconnect(void);
	virtual void Disconnect(void);
	virtual int RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs);
	virtual int SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iLen, int iTimeoutMs);
	virtual bool IsConnected(void);
	virtual int GetErrorCode(void);
	virtual long GetHandle(void);

public:
	// 初始化websocket
	void init();

	// 连接websocket服务
	bool Connect(string strIp, int nPort);

	// 断开websocket服务
	void disConnect();

	// 发送消息
	void send(string& strData);

	// 接收消息
	void recv(const char* pszData, int nSize);

private:
	//WebSocketBase* m_pWebSocketBase;
	string m_strLaneId;
	string m_strLaneHexId;

	string m_strServerIP;
	WORD m_nPort = DEFAULT_RSU_PORT;
	char*  m_pszRecvData;
	int m_nRecvDataSize = 0;
};
