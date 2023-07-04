#include "WebSocketClient.h"
#include "WebSocketBase.h"
#include "../Utils.h"

void recvMsgFunc(void* pArgs, const char* pszData, int nSize)
{
	if (nullptr == pArgs || nullptr == pszData)
		return;

	WebSocketClientInterface* pWebSocket = (WebSocketClientInterface*)pArgs;
	pWebSocket->recv(pszData, nSize);
}

WebSocketClientInterface::WebSocketClientInterface(void)
	:m_pszRecvData(nullptr)
{
	init();
}

WebSocketClientInterface::~WebSocketClientInterface(void)
{
}

bool WebSocketClientInterface::Connect(const char * pccArgs)
{
	char cServerIP[32];
	int iPort = GetKeywordInt(pccArgs, "Port");
	int iTimeoutMs = GetKeywordInt(pccArgs, "TimeoutMs");
	if (iPort < 0)
		return false;

	if (iTimeoutMs < 0) { iTimeoutMs = 1000; }

	if (GetKeywordString(pccArgs, "ServerIP", cServerIP, sizeof(cServerIP)) <= 0)
	{
		return false;
	}

	return Connect(cServerIP, iPort);
}

bool WebSocketClientInterface::TestAndReconnect(void)
{
	if (!IsConnected())
	{
		return Connect(m_strServerIP, m_nPort);
	}
	return true;
}

void WebSocketClientInterface::Disconnect(void)
{
}

int WebSocketClientInterface::RecvFrom(DWORD * pdwSrcIP, WORD * pwSrcPort, void * pvData, int iMaxLen, int iTimeoutMs)
{
	if (m_pszRecvData)
	{
		char *pcBytes = (char *)(pvData);
		memcpy(pcBytes, m_pszRecvData, m_nRecvDataSize);

		// todo 返回数据后要记得清空缓存
		delete[] m_pszRecvData;
		m_pszRecvData = nullptr;

		return m_nRecvDataSize;
	}

	return 0;
}

int WebSocketClientInterface::SendTo(DWORD dwDstIP, WORD wDstPort, const void * pcvData, int iLen, int iTimeoutMs)
{
	if (!pcvData)
		return -1;

	bool bRet = WebSocketBaseClient->send((const char *)(pcvData), iLen);
	return bRet;
}

bool WebSocketClientInterface::IsConnected(void)
{
	CheckResultWithRet(WebSocketBaseClient, false);
	return WebSocketBaseClient->isConnected();
}

int WebSocketClientInterface::GetErrorCode(void)
{
	return 0;
}

long WebSocketClientInterface::GetHandle(void)
{
	return 0;
}

void WebSocketClientInterface::init()
{
	//m_pWebSocketBase = new WebSocketBase();
	WebSocketBaseClient->setCallBack(recvMsgFunc, (void*)this);
}

bool WebSocketClientInterface::Connect(string strIp, int nPort)
{
	if (WebSocketBaseClient->isConnected())
		return true;

	m_nPort = nPort;
	m_strServerIP = strIp;

	string strErr;
	if (!WebSocketBaseClient->connect(m_strServerIP.c_str(), m_nPort, strErr))
	{
		return false;
	}

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "成功连接websocket服务器%s:%d", m_strServerIP.c_str(), m_nPort);

	return true;
}

void WebSocketClientInterface::disConnect()
{
	CheckResult(WebSocketBaseClient);
	WebSocketBaseClient->disConnect();
}

void WebSocketClientInterface::send(string& strData)
{
	CheckResult(WebSocketBaseClient);

	bool bRet = WebSocketBaseClient->send(strData.c_str(), (int)strData.size());

	char szInfo[1024] = {0};
	sprintf_s(szInfo, "发送数据(%s):%s", bRet ? "成功" : "失败", strData.c_str());
}

void WebSocketClientInterface::recv(const char* pszData, int nSize)
{
	// TODO:转发服务器的数据给调用者
	m_pszRecvData = new char[nSize];
	memset(m_pszRecvData, 0, nSize);
	memcpy(m_pszRecvData, pszData, nSize);
	m_nRecvDataSize = nSize;
}
