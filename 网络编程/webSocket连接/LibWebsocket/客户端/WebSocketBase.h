/*
	本类主要是封装了websocket的基类，基于libwebsockets开源库 
*/
#pragma once
#include <list>
#include <map>
#include "ThreadBase.h"
#include "libwebsockets.h"

#define LOGTYPE "_log_type:"

// 回调函数，将websocket的结果抛给上层
typedef void(*recvWebSocketMsg)(void* pUser, const char* pszData, int nSize);

// 每一个websocket均基于线程基类，主要用于管理自己的消息
class WebSocketBase : public SingleTon<WebSocketBase>, public CThreadBase
{
	friend class SingleTon<WebSocketBase>;

public:
	WebSocketBase(void);
	virtual ~WebSocketBase(void);
	virtual int circle();

public:
	// 初始化
	void init();

	// 反初始化
	void unInit();

	// 是否已经连接
	bool isConnected() { return m_bIsConnected; }
	bool isErrorDisonnect() { return m_bErrorDisconnect; }

	// 是否发送数据成功
	int isSended() { return m_nSendResult; }

	// 设置连接标志
	void resetConnected(bool bConnected) { m_bIsConnected = bConnected; }
	void reseErrortDisconnect(bool bDisconnect) { m_bErrorDisconnect = bDisconnect; }

	// 连接服务器
	bool connect(const char* pszUrl);
	bool connect(const char* pszIp, int nPort, string& strErr);

	// 断开连接
	bool disConnect();

	// 断开重连
	bool autoConnect();

	// 发送消息
	bool send(const char* pszJson, unsigned int  nSize);

	// 获取配对lws和websocketbase
	static void* getLwsToWs(lws* pLws);


public:
	// 设置回调函数
	void setCallBack(recvWebSocketMsg pFunc, void* pUser);

	// 连接上了服务器
	void connectResult(bool bConnect);
	void getConnectStatus(int nConncet);		//-1-未连接，0-客户端断开，1-服务端断开

	// 消费发送消息函数
	void consumeSendMsg();

	// 日志发送，复用recvWebSocketMsg回调
	void writeLog(const char* pszData);

	// 接收消息
	bool recvMsgByCallBack(const char* pszData, int nSize);

private:
	int m_nPort;												// websocket 端口
	string m_strIp;												// websocket ip
	char m_szHost[256];											// 服务端链接
	int m_nSendResult;											// 是否发送数据成功
	bool m_bIsConnected;										// 是否已经连接上了服务器
	bool m_bErrorDisconnect;									// 是否异常断开连接
	list<pair<int, char*>> m_listMsg;							// 等待发送的消息队列
	int m_nReconnectTimes;										// 重连的次数
	string m_strLog;											// 私有类型的日志

	struct lws* m_pWsi;											// websocket连接句柄
	struct lws_context* m_pContext;								// websocket处理器
	struct lws_context_creation_info* m_pLwsCtxInfo;			// context参数
	struct lws_protocols m_pLwsProtocols[2];					// 协议层传输句柄
	
	void* m_pUser;												// 返回回调的对象
	struct lws_client_connect_info m_tagConnInfo;				// 连接参数
	recvWebSocketMsg m_pRecvWebSocketFunc;						// 回调

	static map<lws*, void*> m_mapLwsToWs;						// 配对lws和websocketbase
};

#define WebSocketBaseClient WebSocketBase::getInstance()