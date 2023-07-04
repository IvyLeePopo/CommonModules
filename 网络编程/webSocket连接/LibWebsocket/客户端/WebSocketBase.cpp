#include "WebSocketBase.h"
#include <WinSock2.h>
#include "../Utils.h"
#include "../CommInterface.h"

//#pragma comment(lib, "websockets.lib")
#define PROTOCOL_NAME "ws"
#define MAX_PAYLOAD_SIZE 10 * 1024

void sendMessageToClient_Test(struct lws *wsi)
{
	char* message = (char*)("hello world! hello world!");
	int n = strlen(message);
	unsigned char *buf = (unsigned char *)malloc(LWS_SEND_BUFFER_PRE_PADDING + n + LWS_SEND_BUFFER_POST_PADDING);
	memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING, message, n);
	lws_write(wsi, buf + LWS_SEND_BUFFER_PRE_PADDING, n, LWS_WRITE_TEXT);
	free(buf);
}

int wsCallBack(struct lws *pWsi, enum lws_callback_reasons reason, void* pUser, void* pRecvData, size_t nLen)
{
	if (nullptr == pUser)
		return 0;

	WebSocketBase* pWebSocket = (WebSocketBase*)WebSocketBase::getLwsToWs(pWsi);
	if (nullptr == pWebSocket)
		return 0;

	switch (reason)
	{
	case LWS_CALLBACK_CLIENT_ESTABLISHED:				// ���ӵ�������
		pWebSocket->connectResult(true);
		break;
	case LWS_CALLBACK_CLIENT_RECEIVE:					// ���շ���������
		pWebSocket->recvMsgByCallBack((const char*)pRecvData, (int)nLen);
		//sendMessageToClient_Test(pWsi);
		break;
	case LWS_CALLBACK_CLIENT_WRITEABLE:					// �ͻ��˿��Է���������
		pWebSocket->consumeSendMsg();
		break;
	case LWS_CALLBACK_CLOSED:							// �Ͽ�����
		if (pWebSocket->isCanRun())
			pWebSocket->connectResult(false);
		break;
	default:
		break;
	}

	return 0;
};

struct session_data
{
	int msg_count;
	unsigned char buf[LWS_PRE + MAX_PAYLOAD_SIZE];
	int len;
};

map<lws*, void*> WebSocketBase::m_mapLwsToWs;

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

int WebSocketBase::circle()
{
	while (isCanRun())
	{
		lws_service(m_pContext, 1000);
		lws_callback_on_writable(m_pWsi);
		Sleep(50);

		if (isErrorDisonnect())
		{
			if (isConnected())
			{
				reseErrortDisconnect(false);
				continue;
			}
			autoConnect();
			lws_service(m_pContext, 1000);
			Sleep(5000);
		}
		if (!m_strLog.empty())
		{
			// �����﷢�ͣ���Ҫ�Ǵ�websocket recv�������̣߳����ֱ�ӵ���д��־�ӿڣ����������
			writeLog(m_strLog.c_str());
			m_strLog = "";
		}
	}

	lws_context_destroy(m_pContext);
	m_pContext = nullptr;
	resetConnected(false);

	writeLog("websocket�����߳�");
	return TRUE;
}

struct lws_protocols protocols[] =
{
	{ PROTOCOL_NAME, wsCallBack, sizeof(WebSocketBase), MAX_PAYLOAD_SIZE},
	{NULL, NULL, 0}
};

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

bool WebSocketBase::connect(const char* pszUrl)
{
	return true;
}

bool WebSocketBase::connect(const char* pszIp, int nPort, string& strErr)
{
	char szBuf[1028] = { 0 };
	if (nullptr == pszIp || 0 == nPort)
	{
		strErr = "����websocket��ip�Ͷ˿�����";
		return false;
	}

	if (isConnected())
	{
		sprintf_s(szBuf, "websocket�Ѿ�����%s:%d�������ٴ�����!", pszIp, nPort);
		strErr = szBuf;
		return true;
	}

	if (isCanRun() && isErrorDisonnect())
	{
		sprintf_s(szBuf, "����������%s:%d�������ٴ�����!", pszIp, nPort);
		strErr = szBuf;
		return false;
	}

	if (nullptr == m_pLwsCtxInfo)
	{
		strErr = "m_pLwsCtxInfo is null,û�г�ʼ������ע��";
		return false;
	}

	if (nullptr != m_pContext)
	{
		lws_context_destroy(m_pContext);
		m_pContext = nullptr;
	}

	m_strIp = pszIp;
	m_nPort = nPort;
	m_pContext = lws_create_context(m_pLwsCtxInfo);

	memset(m_szHost, 0, sizeof(m_szHost));
	string strUrl = m_strIp.c_str() + string(":") + std::to_string(m_nPort);
	memcpy(m_szHost, strUrl.c_str(), strUrl.size());

	memset(&m_tagConnInfo, 0, sizeof(m_tagConnInfo));
	m_tagConnInfo.context = m_pContext;
	m_tagConnInfo.address = m_strIp.c_str();
	m_tagConnInfo.port = m_nPort;
	m_tagConnInfo.ssl_connection = 0;
	m_tagConnInfo.path = "./";
	m_tagConnInfo.host = m_szHost;
	m_tagConnInfo.protocol = PROTOCOL_NAME;

	m_pWsi = lws_client_connect_via_info(&m_tagConnInfo); // ���ӵ�WebSocket������
	m_mapLwsToWs[m_pWsi] = (void*)this;

	int nTimes = 0;
	while (true)
	{
		// �˴���Ϊ�˿��Կ������ӣ����ҿ��Ը����ϲ�����״̬
		int nRet = lws_service(m_pContext, 50); //�����κδ�������ݻ��¼�
		nRet = lws_callback_on_writable(m_pWsi);

		if (nTimes++ > 10 || isConnected())
			break;

		Sleep(20);
	}

	if (!isConnected())
	{
		strErr = "m_pLwsCtxInfo is null,û�г�ʼ������ע��";
		return false;
	}

	vStart();
	return true;
}

bool WebSocketBase::disConnect()
{
	vStop();
	return true;
}

bool WebSocketBase::autoConnect()
{
	lws_context_destroy(m_pContext);

	m_pContext = nullptr;
	m_pContext = lws_create_context(m_pLwsCtxInfo);

	map<lws*, void*>::iterator iter = m_mapLwsToWs.find(m_pWsi);
	if (iter != m_mapLwsToWs.end())
	{
		m_mapLwsToWs.erase(iter);
	}

	m_tagConnInfo.context = m_pContext;
	m_tagConnInfo.host = m_szHost;

	m_pWsi = lws_client_connect_via_info(&m_tagConnInfo);
	m_mapLwsToWs[m_pWsi] = (void*)this;

	char szBuf[1024] = { 0 };
	sprintf_s(szBuf, "�Զ�����%d��", ++m_nReconnectTimes);
	writeLog(szBuf);
	return true;
}

bool WebSocketBase::send(const char* pszJson, unsigned int nSize)
{
	if (isCanRun() && !isConnected())
		return false;

	if (nullptr == pszJson)
		return false;

	_lock();
	// �˴��ȼ��һ�»��棬�����쳣����£����ݷ��Ͳ���ȥ���µĻ���ѻ���
	if (m_listMsg.size() > 30)
		m_listMsg.clear();

	pair<int, char*> pairData;
	pairData.first = nSize;
	pairData.second = new char[pairData.first];
	memcpy(pairData.second, pszJson, pairData.first);

	m_listMsg.push_back(pairData);

	_unLock();
	return true;
}

void* WebSocketBase::getLwsToWs(lws* pLws)
{
	if (nullptr == pLws)
		return nullptr;

	map<lws*, void*>::iterator iter = m_mapLwsToWs.find(pLws);
	if (iter == m_mapLwsToWs.end())
		return nullptr;

	return (void*)iter->second;
}

void WebSocketBase::setCallBack(recvWebSocketMsg pFunc, void* pUser)
{
	if (nullptr != pFunc)
		m_pRecvWebSocketFunc = pFunc;

	if (nullptr != pUser)
		m_pUser = pUser;
}

void WebSocketBase::connectResult(bool bConnect)
{
	if (0 != m_nReconnectTimes)
	{
		char szBuf[1024] = { 0 };
		sprintf_s(szBuf, "websocket����-%s:%d", m_strIp.c_str(), m_nPort);
		m_strLog = szBuf;
		m_nReconnectTimes = 0;
	}

	resetConnected(bConnect);
	reseErrortDisconnect(!bConnect);
}

void WebSocketBase::consumeSendMsg()
{
	if (m_listMsg.empty())
		return;

	int nSize;
	char* pszData;
	_lock();
	list<pair<int, char*>>::reverse_iterator rIter = m_listMsg.rbegin();
	pair<int, char*> pairData = *rIter;
	nSize = pairData.first;
	pszData = new char[nSize];
	memcpy(pszData, pairData.second, nSize);
	m_listMsg.pop_back();		// �Ӷ���ɾ��
	_unLock();

	if (nSize < 10)
		return;

	char *pszBuf = new char[nSize + LWS_PRE];
	memset(&pszBuf[LWS_PRE], 0, nSize);
	memcpy(&pszBuf[LWS_PRE], pszData, nSize);
	m_nSendResult = lws_write(m_pWsi, (unsigned char*)&pszBuf[LWS_PRE], nSize, LWS_WRITE_BINARY);

	delete[] pszBuf;
	pszBuf = nullptr;

	delete[] pszData;
	pszData = nullptr;
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
	if (nullptr == pszData || nullptr == m_pRecvWebSocketFunc)
		return false;

	m_pRecvWebSocketFunc(m_pUser, pszData, nSize);
	return true;
}
