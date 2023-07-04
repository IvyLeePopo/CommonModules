#include "StdAfx.h"
#include <WinSock2.h>
#include "WebSocketBase.h"

#pragma comment(lib, "websockets.lib")
#define PROTOCOL_NAME "ws"
#define MAX_PAYLOAD_SIZE 10 * 1024
#define nullptr NULL

// 线程函数
DWORD WINAPI threadProc_WebSocketBase(LPVOID pArgs)
{
	if (0 == pArgs)
		return 0;

	WebSocketBase* pThreadBase = (WebSocketBase*)pArgs;
	pThreadBase->circle();
	return 0;
}

void sendMessageToClient_Test(struct lws *wsi)
{
	char* message("hello world! hello world!");
	int n = strlen(message);
	unsigned char *buf = (unsigned char *)malloc(LWS_SEND_BUFFER_PRE_PADDING + n + LWS_SEND_BUFFER_POST_PADDING);
	memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING, message, n);
	lws_write(wsi, buf + LWS_SEND_BUFFER_PRE_PADDING, n, LWS_WRITE_TEXT);
	free(buf);
}

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void* pRecvData, size_t nLen)
{
	return 0;
}

static int callback_websocket(struct lws *pWsi, enum lws_callback_reasons reason, void *pUser, void* pRecvData, size_t nLen)
{
	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:					// 连接到客户端（正常只有一个客户端）
		WebSocketBaseServer->connectResult(true);
		break;
	case LWS_CALLBACK_RECEIVE:						// 接收客户端数据
		WebSocketBaseServer->recvMsgByCallBack((const char*)pRecvData, (int)nLen);
		sendMessageToClient_Test(pWsi);
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:				// 服务端可以发送数据了
		WebSocketBaseServer->consumeSendMsg();
		break;
	case LWS_CALLBACK_CLOSED:						// 断开连接
		if (WebSocketBaseServer->isCanRun())
			WebSocketBaseServer->connectResult(false);
		break;
	default:
		break;
	}
	return 0;
}

static struct lws_protocols protocols[] = {
	{ PROTOCOL_NAME, callback_websocket, 0, 0,},
	{ NULL, NULL, 0, 0 } /* end of list */
};

WebSocketBase::WebSocketBase(void)
: m_pLwsCtxInfo(nullptr)
, m_pContext(nullptr)
, m_pWsi(nullptr)
, m_bIsConnected(false)
, m_pUser(nullptr)
, m_pRecvWebSocketFunc(nullptr)
, m_bErrorDisconnect(false)
, m_nReconnectTimes(0)
{
	init();
}

WebSocketBase::~WebSocketBase(void)
{
	unInit();
}

void WebSocketBase::vStart()
{
	m_nThreadHandle = (int)CreateThread(NULL, 0, threadProc_WebSocketBase, this, 0, &m_dThreadId);
	m_bCanRun = true;
}

void WebSocketBase::vStop()
{
	m_bCanRun = false;
}

bool WebSocketBase::isCanRun()
{
	return m_bCanRun;
}

int WebSocketBase::circle()
{
	while (isCanRun())
	{
		//lws_callback_on_writable_all_protocol(m_pContext, &protocols[0]);
		lws_service(m_pContext, 1000);
	}
	lws_context_destroy(m_pContext);

	return TRUE;
}

void WebSocketBase::init()
{
	if (nullptr == m_pLwsCtxInfo)
	{
		m_pLwsCtxInfo = new struct lws_context_creation_info();
		memset(m_pLwsCtxInfo, 0, sizeof(struct lws_context_creation_info));
	}

	m_pLwsCtxInfo->port = CONTEXT_PORT_NO_LISTEN;
	m_pLwsCtxInfo->iface = NULL;
	m_pLwsCtxInfo->protocols = protocols;
	m_pLwsCtxInfo->gid = -1;
	m_pLwsCtxInfo->uid = -1;

	m_listMsg.clear();
}

void WebSocketBase::unInit()
{
	if (isCanRun())
	{
		vStop();
		Sleep(20);
	}

	if (nullptr != m_pLwsCtxInfo)
	{
		delete m_pLwsCtxInfo;
		m_pLwsCtxInfo = nullptr;
	}
}

bool WebSocketBase::startSever()
{
	if (nullptr == m_pLwsCtxInfo)
		return false;

	if (nullptr != m_pContext)
	{
		lws_context_destroy(m_pContext);
		m_pContext = nullptr;
	}

	m_pLwsCtxInfo->port = 9000;
	m_pLwsCtxInfo->protocols = protocols;

	// 创建服务器句柄
	m_pContext = lws_create_context(m_pLwsCtxInfo);
	if (!m_pContext) 
	{
		return false;
	}

	lws_service(m_pContext, 1000);

	vStart();
	return true;
}

bool WebSocketBase::startSever1()
{
	if (nullptr == m_pLwsCtxInfo)
		return false;

	if(nullptr != m_pContext)
	{
		lws_context_destroy(m_pContext);
		m_pContext = nullptr;
	}

	int port = 9000;
	m_pLwsCtxInfo->port = port;

	m_pContext = lws_create_context(m_pLwsCtxInfo);//创建上下文对象，管理ws
	if (!m_pContext) 
	{
		return false;
	}

	//设置服务器消费的发送数据
	lws_callback_on_writable_all_protocol(m_pContext, &protocols[0]);
	lws_service(m_pContext, 1000); //处理WebSocket连接的事件

	return true;
}

bool WebSocketBase::startSever2()
{
	struct lws_context_creation_info info;
	struct lws_context *context;

	memset(&info, 0, sizeof(info));
	info.port = 9000;
	info.protocols = protocols;

	context = lws_create_context(&info);
	if (!context) {
		printf("Failed to create context\n");
		return -1;
	}

	printf("WebSocket server started on port %d\n", info.port);

	while (1) 
	{
		lws_service(context, 1000);
	}

	lws_context_destroy(context);

	return true;
}

bool WebSocketBase::isStarted()
{
	return m_bStartServer;
}

bool WebSocketBase::stopServer()
{
	vStop();
	return true;
}

bool WebSocketBase::send(const char* pszJson, int nSize)
{
	if(isCanRun())
		return false;

	if (nullptr == pszJson)
		return false;

#ifdef NO_threadProc
	_lock();
#endif
	// 此处先检查一下缓存，避免异常情况下，数据发送不出去导致的缓存堆积了
	if (m_listMsg.size() > 30)
		m_listMsg.clear();

	m_listMsg.push_back(pszJson);


#ifdef NO_threadProc
	_unLock();
#endif
	return true;
}

void WebSocketBase::setCallBack(recvWebSocketMsg pFunc, void* pUser)
{
	this;
	if (nullptr != pFunc)
		m_pRecvWebSocketFunc = pFunc;

	if (nullptr != pUser)
		m_pUser = pUser;
}

void WebSocketBase::connectResult(bool bConnect)
{
	if (0 != m_nReconnectTimes)
	{
		char szBuf[1024] = {0};
		sprintf_s(szBuf, "websocket连接-%s:%d", m_strIp.c_str(), m_nPort);
		m_strLog = szBuf;
		m_nReconnectTimes = 0;
	}
	
	resetConnected(bConnect);
	reseErrortDisconnect(!bConnect);
}

void WebSocketBase::consumeSendMsg()
{
	if (m_listMsg.size() < 1)
		return;

	string strData;

#ifdef NO_threadProc
	_lock();
#endif

	list<string>::reverse_iterator rIter = m_listMsg.rbegin();
	strData = *rIter;
	m_listMsg.pop_back();		// 从队列删除

#ifdef NO_threadProc
	_unLock();
#endif

	if (strData.empty())
		return;

	int nSize = (int)strData.size();
	char *pszBuf = new char[nSize + LWS_PRE];
	memset(&pszBuf[LWS_PRE], 0, nSize);
	memcpy(&pszBuf[LWS_PRE], strData.c_str(), nSize);
	m_nSendResult = lws_write(m_pWsi, (unsigned char*)&pszBuf[LWS_PRE], nSize, LWS_WRITE_TEXT);

	delete [] pszBuf;
	pszBuf = nullptr;
}

void WebSocketBase::writeLog(const char* pszData)
{
	if (nullptr == pszData || nullptr == m_pRecvWebSocketFunc)
		return;

	string strErr = LOGTYPE + string(pszData);
	m_pRecvWebSocketFunc(m_pUser, strErr.c_str(), (int)strErr.size());
}

bool WebSocketBase::recvMsgByCallBack(const char* pszData, int nSize)
{
	this;
	if (nullptr == pszData || nullptr == m_pRecvWebSocketFunc)
		return false;

	m_pRecvWebSocketFunc(m_pUser, pszData, nSize);
	return true;
}
