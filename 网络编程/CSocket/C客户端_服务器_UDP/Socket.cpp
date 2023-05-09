/*****************************************************************
 * 版权信息:  
 * 文件名称: Socket.cpp
 * 文件作者:  
 * 完成日期: 
 * 当前版本:
 * 主要功能: 
 * 版本历史:
 *****************************************************************/
#include "Platform.h"
#include "Utils.h"
#include "Socket.h"

#define WIN_SOCKET_DEBUG

#ifdef WIN_SOCKET_DEBUG
#define WS_DEBUG  Log
#else
#define WS_DEBUG(...)
#endif

#if IS_WINDOWS

#pragma comment(lib, "IPHlpApi.lib")
#pragma comment(lib, "Ws2_32.lib")
 /*****************************************************************
  函数名称：SocketInitDll
  函数描述：
  输入参数：
  输出参数：
  返回说明：
  其它说明：
  *****************************************************************/
static int SocketInitDll(void)
{ // Initialize Winsock
	WSADATA stWSAData;
	int iRet = WSAStartup(MAKEWORD(2, 2), &stWSAData);
	if (iRet != 0) {
		return WSAGetLastError();;
	}
	if (LOBYTE(stWSAData.wVersion) != 2 || HIBYTE(stWSAData.wVersion) != 2) {
		//WSACleanup();
		return WSAGetLastError();
	}
	return ERROR_SUCCESS;
}

/*****************************************************************
 函数名称：WinSocketGetHostIPs
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int WinSocketGetHostIPs(char cIPs[][16], int iMaxCnt)
{
	//IP_ADAPTER_INFO结构体
	int iIPCnt = 0;
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO;
	//结构体大小
	ULONG ulSize = sizeof(IP_ADAPTER_INFO);
	//获取适配器信息
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(pIpAdapterInfo, &ulSize))
	{
		//空间不足，删除之前分配的空间
		if (pIpAdapterInfo != NULL)
		{
			delete[]pIpAdapterInfo;
		}
		//重新分配大小
		pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[ulSize];
		//获取适配器信息
		if (ERROR_SUCCESS != GetAdaptersInfo(pIpAdapterInfo, &ulSize))
		{//获取失败
			if (pIpAdapterInfo != NULL)
			{
				delete[]pIpAdapterInfo;
			}
			return iIPCnt;
		}
	}
	//赋值指针
	PIP_ADAPTER_INFO pIterater = pIpAdapterInfo;
	while (pIterater != NULL)
	{
		//指向IP地址列表
		PIP_ADDR_STRING pIpAddr = &pIterater->IpAddressList;
		while (pIpAddr != NULL)
		{
			const char *pccDiscard = "0.0.0.0";
			if ((iIPCnt < iMaxCnt) && (strcmp(pccDiscard, pIpAddr->IpAddress.String) != 0))
			{
				strncpy_s(cIPs[iIPCnt], pIpAddr->IpAddress.String, 16);
				iIPCnt++;
			}
			pIpAddr = pIpAddr->Next;
		}
		pIterater = pIterater->Next;
	}
	//清理
	if (pIpAdapterInfo != NULL)
	{
		delete[]pIpAdapterInfo;
	}
	return iIPCnt;
}
#else

static int SocketInitDll(void)
{
	return 0;
}

static int WSAGetLastError(void)
{
	return errno;
}

static void closesocket(SOCKET s)
{
	close(s);
}

static int ioctlsocket(SOCKET s, long cmd, void *parg)
{
	return ioctl(s, cmd, parg);
}
#endif
/*****************************************************************
 函数名称：SocketClientInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketClientInterface::SocketClientInterface(void)
{ // Initialize Winsock
	mErrorCode = SocketInitDll();
}

/*****************************************************************
 函数名称：SocketClientInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketClientInterface::SocketClientInterface(const char *pccServerIP, WORD wPort, int iTimeoutMs)
{
	mErrorCode = SocketInitDll();
	Connect(pccServerIP, wPort, iTimeoutMs);
}

/*****************************************************************
 函数名称：SocketClientInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketClientInterface::SocketClientInterface(const char *pccArgs)
{
	mErrorCode = SocketInitDll();
	Connect(pccArgs);
}
/*****************************************************************
 函数名称：~SocketClientInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketClientInterface::~SocketClientInterface(void)
{
	Disconnect();
}

/*****************************************************************
 函数名称：TryConnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketClientInterface::TryConnect(const char *pccServerIP, WORD wPort)
{
	struct sockaddr_in unSockAddrClient;
	if (pccServerIP != NULL)
	{
		mPort = wPort;
		strncpy_s(m_cServerIP, pccServerIP, sizeof(m_cServerIP));
	}
	Disconnect();
	mTempSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!SOCKET_IS_VALID(mTempSocket))
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s Create Socket Error ErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return false;
	}
	if (SetNotBlock(mTempSocket, true) != true)
	{
		mErrorCode = WSAGetLastError();
		Disconnect();
		WS_DEBUG("%s SetNotBlock Error ErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return false;
	}
	unSockAddrClient.sin_family = AF_INET;
	unSockAddrClient.sin_addr.s_addr = inet_addr(m_cServerIP);
	unSockAddrClient.sin_port = htons(mPort);
	m_dwServerIP = ntohl(unSockAddrClient.sin_addr.s_addr);
	if (connect(mTempSocket, (struct sockaddr *)&unSockAddrClient, sizeof(unSockAddrClient)) != 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s connect Error ErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return false;
	}
	return true;
}

/*****************************************************************
 函数名称：ConnectionConfirm
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketClientInterface::ConnectionConfirm(int iTimeoutMs)
{
	int iEvtCnt, iOptions = true;
	struct timeval stTV;
	fd_set stWriteSet, stExceptSet;
	if (SOCKET_IS_VALID(mTempSocket) && (iTimeoutMs >= 0))
	{
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mTempSocket, &stWriteSet);
		FD_SET(mTempSocket, &stExceptSet);
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		iEvtCnt = select((int)mTempSocket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);
		if ((iEvtCnt <= 0) || (FD_ISSET(mTempSocket, &stExceptSet)))
		{
			Disconnect();
			return false;
		}
	}
	mSocket = mTempSocket;
	mTempSocket = INVALID_SOCKET;
	if (!SOCKET_IS_VALID(mSocket))
	{
		WS_DEBUG("%s mSocket == INVALID_SOCKET\n", __FUNCTION__);
		return false;
	}
	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_REUSEADDR Error\n", __FUNCTION__);
		return false;
	}
	iOptions = true;
	if (setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt TCP_NODELAY Error\n", __FUNCTION__);
		return false;
	}
	iOptions = true;
	if (setsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_KEEPALIVE Error\n", __FUNCTION__);
		return false;
	}
	#if IS_WINDOWS
	iOptions = 10 * 1000;
	if (setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_SNDTIMEO Error\n", __FUNCTION__);
		return false;
	}
	iOptions = 10 * 1000;
	if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_RCVTIMEO Error\n", __FUNCTION__);
		return false;
	}
	#endif
	iOptions = 128 * 1024;
	if (setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_SNDBUF Error\n", __FUNCTION__);
		return false;
	}
	iOptions = 128 * 1024;
	if (setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_RCVBUF Error\n", __FUNCTION__);
		return false;
	}
	WS_DEBUG("%s Connection Established Socket = %d\n", __FUNCTION__, mSocket);
	return true;
}

/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketClientInterface::Connect(const char *pccServerIP, WORD wPort, int iTimeoutMs)
{
	if (TryConnect(pccServerIP, wPort) == true)
	{
		iTimeoutMs = -1;
	}
	return ConnectionConfirm(iTimeoutMs);
}

/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketClientInterface::Connect(const char *pccArgs)
{
	char cServerIP[32];
	int iPort = GetKeywordInt(pccArgs, "Port");
	int iTimeoutMs = GetKeywordInt(pccArgs, "TimeoutMs");
	if (iPort < 0)
	{
		return false;
	}
	if (iTimeoutMs < 0){ iTimeoutMs = 1000; }
	if (GetKeywordString(pccArgs, "ServerIP", cServerIP, sizeof(cServerIP)) <= 0)
	{
		return false;
	}
	return Connect(cServerIP, iPort, iTimeoutMs);
}

/*****************************************************************
 函数名称：TestAndReconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketClientInterface::TestAndReconnect(void)
{
	if (!this->IsConnected())
	{
		return Connect(m_cServerIP, mPort, 1000);
	}
	return true;
}
/*****************************************************************
 函数名称：Disconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
void SocketClientInterface::Disconnect(void)
{
	if (SOCKET_IS_VALID(mSocket))
	{
		shutdown(mSocket, SD_BOTH);
		closesocket(mSocket);
		WS_DEBUG("%s From Server %s Port %d mSocket %d\n", __FUNCTION__, m_cServerIP, mPort, mSocket);
	}
	if ((mSocket != mTempSocket) && (SOCKET_IS_VALID(mTempSocket)))
	{
		shutdown(mTempSocket, SD_BOTH);
		closesocket(mTempSocket);
		WS_DEBUG("%s From Server %s Port %d, mTempSocket %d\n", __FUNCTION__, m_cServerIP, mPort, mTempSocket);
	}
	mSocket = mTempSocket = INVALID_SOCKET;
}

/*****************************************************************
 函数名称：SetNotBlock
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketClientInterface::SetNotBlock(SOCKET uiSocket, bool bNotBlock)
{
	u_long ulNotBlock = bNotBlock;
	if (!SOCKET_IS_VALID(uiSocket))
	{
		return false;
	}
	if (ioctlsocket(uiSocket, FIONBIO, &ulNotBlock) != 0)
	{
		return false;
	}
	mNotBlock = bNotBlock;
	return true;
}

/*****************************************************************
 函数名称：SendTo
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SocketClientInterface::SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iDataLen, int iTimeoutMs)
{
	int iSendLen, iEvtCnt;
	const char *pccBytes = (const char *)(pcvData);
	this->TestAndReconnect();
	if (!this->IsConnected())
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		return SOCKET_ERROR;
	}
	if ((mNotBlock != false) && (iTimeoutMs >= 0))
	{
		fd_set stWriteSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocket, &stWriteSet);
		FD_SET(mSocket, &stExceptSet);
		iEvtCnt = select((int)mSocket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);  // Wait on read or error
		if (iEvtCnt <= 0)
		{
			return iEvtCnt;
		}
		if (FD_ISSET(mSocket, &stExceptSet))
		{
			Disconnect();
			return SOCKET_ERROR;
		}
	}
	iSendLen = send(mSocket, pccBytes, iDataLen, 0);
	if (iSendLen <= 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s send Error ErrorCode = %d\n", __FUNCTION__, mErrorCode);
		Disconnect();
	}
	return iSendLen;
}

/*****************************************************************
 函数名称：RecvFrom
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SocketClientInterface::RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs)
{
	int iEvtCnt, iRecvLen = 0;
	char *pcBytes = (char *)(pvData);
	if (!this->IsConnected())
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		return SOCKET_ERROR;
	}
	if ((mNotBlock != false) && (iTimeoutMs >= 0))
	{
		fd_set stReadSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stReadSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocket, &stReadSet);
		FD_SET(mSocket, &stExceptSet);
		iEvtCnt = select((int)mSocket + 1, &stReadSet, NULL, &stExceptSet, &stTV);  // Wait on read or error
		if (iEvtCnt <= 0)
		{
			return iEvtCnt;
		}
		if (FD_ISSET(mSocket, &stExceptSet))
		{
			Disconnect();
			return 0;
		}
	}
	iRecvLen = recv(mSocket, pcBytes, iMaxLen, 0);
	if (iRecvLen > 0)
	{
		if (pdwSrcIP != NULL)
		{
			*pdwSrcIP = m_dwServerIP;
		}
		if (pwSrcPort != NULL)
		{
			*pwSrcPort = mPort;
		}
		return iRecvLen;
	}
	else if (iRecvLen == 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s RecvLen = 0 Disconnect From Server ErrorCode = %d\n", __FUNCTION__, mErrorCode);
	}
	else
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s RecvLen = %d Recv Error ErrorCode = %d\n", __FUNCTION__, iRecvLen, mErrorCode);
	}
	Disconnect();
	return iRecvLen;
}

/*****************************************************************
 函数名称：SocketServerInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketServerInterface::SocketServerInterface(WORD wPort, WORD wBackLog)
{
	mErrorCode = SocketInitDll();
	Connect(wPort, wBackLog);
}

/*****************************************************************
 函数名称：SocketServerInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketServerInterface::SocketServerInterface(const char *pccArgs)
{
	mErrorCode = SocketInitDll();
	Connect(pccArgs);
}

/*****************************************************************
 函数名称：~SocketServerInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketServerInterface::~SocketServerInterface(void)
{
	WS_DEBUG("%s Destruct mSocketAccepted = %d, mSocketListenner = %d\n", __FUNCTION__, mSocketAccepted, mSocketListenner);
	if (SOCKET_IS_VALID(mSocketAccepted))
	{
		Disconnect();
	}
	if (SOCKET_IS_VALID(mSocketListenner))
	{
		shutdown(mSocketListenner, SD_BOTH);
		closesocket(mSocketListenner);
		mSocketListenner = INVALID_SOCKET;
	}
	//WSACleanup();
}

/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketServerInterface::Connect(WORD wPort, WORD wBackLog)
{ // Initialize Winsock
	int iOptions = 0;
	struct sockaddr_in unAddrSrv;
	mPort = wPort;
	mBackLog = wBackLog;
	mSocketListenner = socket(AF_INET, SOCK_STREAM, 0);
	if (!SOCKET_IS_VALID(mSocketListenner))
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s Create Listen Socket Error ErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return false;
	}
	#if IS_WINDOWS
	iOptions = false;
	if (setsockopt(mSocketListenner, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&iOptions, sizeof(int)) != 0)
	{
		mErrorCode = WSAGetLastError();
		closesocket(mSocketListenner);
		mSocketListenner = INVALID_SOCKET;
		WS_DEBUG("%s setsockopt SO_EXCLUSIVEADDRUSE For SocketListenner %d Error\n", __FUNCTION__, mSocketListenner);
		return false;
	}
	#endif
	iOptions = true;
	if (setsockopt(mSocketListenner, SOL_SOCKET, SO_REUSEADDR, (const char *)&iOptions, sizeof(int)) != 0)
	{
		mErrorCode = WSAGetLastError();
		closesocket(mSocketListenner);
		mSocketListenner = INVALID_SOCKET;
		WS_DEBUG("%s setsockopt SO_REUSEADDR For SocketListenner %d Error\n", __FUNCTION__, mSocketListenner);
		return false;
	}
	unAddrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
	unAddrSrv.sin_family = AF_INET;
	unAddrSrv.sin_port = htons(wPort);
	if (bind(mSocketListenner, (struct sockaddr *)&unAddrSrv, sizeof(unAddrSrv)) != 0)
	{
		mErrorCode = WSAGetLastError();
		closesocket(mSocketListenner);
		mSocketListenner = INVALID_SOCKET;
		WS_DEBUG("%s Bind To Port %d Error ErrorCode = %d\n", __FUNCTION__, wPort, mErrorCode);
		return false;
	}
	if (listen(mSocketListenner, wBackLog) != 0)
	{
		mErrorCode = WSAGetLastError();
		closesocket(mSocketListenner);
		mSocketListenner = INVALID_SOCKET;
		WS_DEBUG("%s Listen For wBackLog %d Error ErrorCode = %d\n", __FUNCTION__, wBackLog, mErrorCode);
		return false;
	}
	if (SetNotBlock(mSocketListenner, true) != true)
	{
		mErrorCode = WSAGetLastError();
		closesocket(mSocketListenner);
		mSocketListenner = INVALID_SOCKET;
		WS_DEBUG("%s SetNotBlock Error mErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return false;
	}
	WS_DEBUG("%s mSocketListenner = %d\n", __FUNCTION__, mSocketListenner);
	return true;
}

/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketServerInterface::Connect(const char *pccArgs)
{
	int iPort = GetKeywordInt(pccArgs, "Port");
	int iBackLog = GetKeywordInt(pccArgs, "BackLog");
	if (iPort < 0)
	{
		return false;
	}
	if (iBackLog < 0)
	{
		iBackLog = 5;
	}
	return Connect(iPort, iBackLog);
}

/*****************************************************************
 函数名称：TestAndReconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketServerInterface::TestAndReconnect(void)
{
	if (!SOCKET_IS_VALID(mSocketListenner))
	{
		return Connect(mPort, mBackLog);
	}
	return true;
}
/*****************************************************************
 函数名称：Accept
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SOCKET SocketServerInterface::Accept(int iTimeoutMs)
{
	struct sockaddr_in unSockAddrClient;
	socklen_t ulAddrLen = sizeof(unSockAddrClient);
	int iOptions = true;
	if (!SOCKET_IS_VALID(mSocketListenner))
	{
		return INVALID_SOCKET;
	}
	if (SOCKET_IS_VALID(mSocketAccepted))
	{
		Disconnect();
	}
	if (iTimeoutMs > 0)
	{
		fd_set stReadSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stReadSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocketListenner, &stReadSet);
		FD_SET(mSocketListenner, &stExceptSet);
		int iEvtCnt = select((int)mSocketListenner + 1, &stReadSet, NULL, &stExceptSet, &stTV);  // Wait on read or error
		if ((iEvtCnt <= 0) || (FD_ISSET(mSocketListenner, &stExceptSet)))
		{
			return INVALID_SOCKET;
		}
	}
	mSocketAccepted = accept(mSocketListenner, (struct sockaddr *)(&unSockAddrClient), &ulAddrLen);
	if (!SOCKET_IS_VALID(mSocketAccepted))
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s accept Error mErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return INVALID_SOCKET;
	}
	if (SetNotBlock(mSocketAccepted, true) != true)
	{
		mErrorCode = WSAGetLastError();
		Disconnect();
		WS_DEBUG("%s SetNotBlock Error mErrorCode = %d\n", __FUNCTION__, mErrorCode);
		return INVALID_SOCKET;
	}
	iOptions = true;
	if (setsockopt(mSocketAccepted, SOL_SOCKET, SO_REUSEADDR, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_REUSEADDR Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	iOptions = true;
	if (setsockopt(mSocketAccepted, IPPROTO_TCP, TCP_NODELAY, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt TCP_NODELAY Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	iOptions = true;
	if (setsockopt(mSocketAccepted, SOL_SOCKET, SO_KEEPALIVE, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_KEEPALIVE Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	#if IS_WINDOWS
	iOptions = 10 * 1000;
	if (setsockopt(mSocketAccepted, SOL_SOCKET, SO_SNDTIMEO, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_SNDTIMEO Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	iOptions = 10 * 1000;
	if (setsockopt(mSocketAccepted, SOL_SOCKET, SO_RCVTIMEO, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_RCVTIMEO Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	#endif
	iOptions = 128 * 1024;
	if (setsockopt(mSocketAccepted, SOL_SOCKET, SO_SNDBUF, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_SNDBUF Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	iOptions = 128 * 1024;
	if (setsockopt(mSocketAccepted, SOL_SOCKET, SO_RCVBUF, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		WS_DEBUG("%s setsockopt SO_RCVBUF Error\n", __FUNCTION__);
		return INVALID_SOCKET;
	}
	mClientPort = htons(unSockAddrClient.sin_port);
	m_dwClientIP = ntohl(unSockAddrClient.sin_addr.s_addr);
	//inet_ntop(AF_INET, &unSockAddrClient.sin_addr, m_cClientIP, sizeof(m_cClientIP)); //WINXP不存在
	Ip4ToStr(m_dwClientIP, m_cClientIPStr, sizeof(m_cClientIPStr));
	WS_DEBUG("%s Connect From Client %s, Port = %u\n", __FUNCTION__, m_cClientIPStr, mClientPort);
	return mSocketAccepted;
}

/*****************************************************************
 函数名称：Disconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
void SocketServerInterface::Disconnect(void)
{
	if (SOCKET_IS_VALID(mSocketAccepted))
	{
		shutdown(mSocketAccepted, SD_BOTH);
		closesocket(mSocketAccepted);
		mSocketAccepted = INVALID_SOCKET;
		WS_DEBUG("%s Client %s, Port %d\n", __FUNCTION__, m_cClientIPStr, mClientPort);
	}
}

/*****************************************************************
 函数名称：SetNotBlock
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketServerInterface::SetNotBlock(SOCKET iSocket, bool bNotBlock)
{
	u_long ulNotBlock = bNotBlock;
	if (!SOCKET_IS_VALID(iSocket))
	{
		return false;
	}
	if (ioctlsocket(iSocket, FIONBIO, &ulNotBlock) != 0)
	{
		WS_DEBUG("%s Set Not Block Enable = %d Error\n", __FUNCTION__, bNotBlock);
		return false;
	}
	mNotBlock = bNotBlock;
	return true;
}

/*****************************************************************
 函数名称：SendTo
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SocketServerInterface::SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iDataLen, int iTimeoutMs)
{
	int iSendLen, iEvtCnt, iMaxFD;
	const char *pccBytes = (const char *)(pcvData);
	if (!SOCKET_IS_VALID(mSocketAccepted))
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		WS_DEBUG("%s Client Disconnect\n", __FUNCTION__);
		return SOCKET_ERROR;
	}
	if ((mNotBlock != false) && (iTimeoutMs >= 0))
	{
		fd_set stWriteSet, stReadSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stReadSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocketListenner, &stReadSet);
		FD_SET(mSocketListenner, &stExceptSet);

		FD_SET(mSocketAccepted, &stExceptSet);
		FD_SET(mSocketAccepted, &stWriteSet);

		iMaxFD = mSocketAccepted > mSocketListenner ? (int)mSocketAccepted : (int)mSocketListenner;
		iEvtCnt = select(iMaxFD + 1, &stReadSet, &stWriteSet, &stExceptSet, &stTV);  // Wait on read or error
		if (iEvtCnt <= 0)
		{
			return iEvtCnt;
		}
		if ((FD_ISSET(mSocketAccepted, &stExceptSet)) || (FD_ISSET(mSocketListenner, &stExceptSet)))
		{
			Disconnect();
			return SOCKET_ERROR;
		}
		if (FD_ISSET(mSocketListenner, &stReadSet))
		{
			Accept(100);
		}
		if (!FD_ISSET(mSocketAccepted, &stWriteSet))
		{
			return 0;
		}
	}
	iSendLen = send(mSocketAccepted, pccBytes, iDataLen, 0);
	if (iSendLen <= 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s Send Error Ret = %d, ErrorCode = %d\n", __FUNCTION__, iSendLen, mErrorCode);
		Disconnect();
	}
	return iSendLen;
}

/*****************************************************************
 函数名称：RecvFrom
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SocketServerInterface::RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs)
{
	int iEvtCnt, iRecvLen = 0, iMaxFD;
	char *pcBytes = (char *)(pvData);
	if (!SOCKET_IS_VALID(mSocketAccepted))
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		WS_DEBUG("%s Client Disconnect\n", __FUNCTION__);
		return SOCKET_ERROR;
	}
	if ((mNotBlock != false) && (iTimeoutMs >= 0))
	{
		fd_set stReadSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stReadSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocketAccepted, &stReadSet);
		FD_SET(mSocketListenner, &stReadSet);
		FD_SET(mSocketAccepted, &stExceptSet);
		FD_SET(mSocketListenner, &stExceptSet);
		iMaxFD = mSocketAccepted > mSocketListenner ? (int)mSocketAccepted : (int)mSocketListenner;
		iEvtCnt = select(iMaxFD + 1, &stReadSet, NULL, &stExceptSet, &stTV);  // Wait on read or error
		if (iEvtCnt <= 0)
		{
			return iEvtCnt;
		}
		if ((FD_ISSET(mSocketAccepted, &stExceptSet)) || (FD_ISSET(mSocketListenner, &stExceptSet)))
		{
			Disconnect();
			return 0;
		}
		if (FD_ISSET(mSocketListenner, &stReadSet))
		{
			Accept(100);
			return 0;
		}
	}
	iRecvLen = recv(mSocketAccepted, pcBytes, iMaxLen, 0);
	if (iRecvLen > 0)
	{
		if (pdwSrcIP != NULL)
		{
			*pdwSrcIP = m_dwClientIP;
		}
		if (pwSrcPort != NULL)
		{
			*pwSrcPort = mPort;
		}
		return iRecvLen;
	}
	else if (iRecvLen == 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s RecvLen = 0 Disconnect From Client ErrorCode = %d\n", __FUNCTION__, mErrorCode);
	}
	else
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s RecvLen = %d Recv Error ErrorCode = %d\n", __FUNCTION__, iRecvLen, mErrorCode);
	}
	Disconnect();
	return iRecvLen;
}

/*****************************************************************
 函数名称：SocketUdpInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketUdpInterface::SocketUdpInterface(WORD wPort)
{ // Initialize Winsock
	mErrorCode = SocketInitDll();
	Connect(wPort);
}

/*****************************************************************
 函数名称：SocketUdpInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketUdpInterface::SocketUdpInterface(const char *pccArgs)
{ // Initialize Winsock
	mErrorCode = SocketInitDll();
	Connect(pccArgs);
}
/*****************************************************************
 函数名称：~SocketUdpInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SocketUdpInterface::~SocketUdpInterface(void)
{
	Disconnect();
}

/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketUdpInterface::Connect(WORD wPort)
{
	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, 0); //创建套接字
	if (!SOCKET_IS_VALID(sockServer))
	{
		return false;
	}
	mPort = wPort;
	//服务器端
	struct sockaddr_in stAddr;
	stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	stAddr.sin_family = AF_INET;
	stAddr.sin_port = htons(wPort);
	//绑定套接字
	if (bind(sockServer, (struct sockaddr *)&stAddr, sizeof(stAddr)) != 0)
	{
		closesocket(sockServer);
		return false;
	}
	if (SetNotBlock(sockServer, true) != true)
	{
		closesocket(sockServer);
		return false;
	}
	mSocket = sockServer;
	return true;
}
/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketUdpInterface::Connect(const char *pccArgs)
{
	int iPort = GetKeywordInt(pccArgs, "Port");
	if (iPort < 0)
	{
		return false;
	}
	return Connect(iPort);
}

/*****************************************************************
 函数名称：TestAndReconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketUdpInterface::TestAndReconnect(void)
{
	if (!this->IsConnected())
	{
		return Connect(mPort);
	}
	return true;
}
/*****************************************************************
 函数名称：Disconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
void SocketUdpInterface::Disconnect(void)
{
	if (SOCKET_IS_VALID(mSocket))
	{
		shutdown(mSocket, SD_BOTH);
		closesocket(mSocket);
		WS_DEBUG("%s UDP Port %d mSocket %d\n", __FUNCTION__, mPort, mSocket);
	}
	mSocket = INVALID_SOCKET;
}

/*****************************************************************
 函数名称：SetNotBlock
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SocketUdpInterface::SetNotBlock(SOCKET uiSocket, bool bNotBlock)
{
	u_long ulNotBlock = bNotBlock;
	if (!SOCKET_IS_VALID(uiSocket))
	{
		return false;
	}
	if (ioctlsocket(uiSocket, FIONBIO, &ulNotBlock) != 0)
	{
		return false;
	}
	mNotBlock = bNotBlock;
	return true;
}

/*****************************************************************
 函数名称：SendTo
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SocketUdpInterface::SendTo(DWORD dwIP, WORD wPort, const void *pcvData, int iDataLen, int iTimeoutMs)
{
	int iSentLen, iEvtCnt;
	if (!this->IsConnected())
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		return SOCKET_ERROR;
	}
	if ((mNotBlock != false) && (iTimeoutMs >= 0))
	{
		fd_set stWriteSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocket, &stWriteSet);
		FD_SET(mSocket, &stExceptSet);
		iEvtCnt = select((int)mSocket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);  // Wait on read or error
		if (iEvtCnt <= 0)
		{
			return iEvtCnt;
		}
		if (FD_ISSET(mSocket, &stExceptSet))
		{
			Disconnect();
			return SOCKET_ERROR;
		}
	}
	struct sockaddr_in stAddrTo;
	memset(&stAddrTo, 0, sizeof(stAddrTo));
	stAddrTo.sin_addr.s_addr = htonl(dwIP);
	stAddrTo.sin_family = AF_INET;
	stAddrTo.sin_port = htons(wPort);
	iSentLen = sendto(mSocket, (const char *)(pcvData), iDataLen, 0, (struct sockaddr *)(&stAddrTo), sizeof(stAddrTo));
	if (iSentLen <= 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s send Error ErrorCode = %d\n", __FUNCTION__, mErrorCode);
		Disconnect();
	}
	return iSentLen;
}

/*****************************************************************
 函数名称：RecvFrom
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SocketUdpInterface::RecvFrom(DWORD *pdwIP, WORD *pwPort, void *pvData, int iMaxLen, int iTimeoutMs)
{
	int iEvtCnt, iRecvLen = 0;
	if (!this->IsConnected())
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		return SOCKET_ERROR;
	}
	if ((mNotBlock != false) && (iTimeoutMs >= 0))
	{
		fd_set stReadSet, stExceptSet;
		struct timeval stTV;
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		FD_ZERO(&stReadSet);
		FD_ZERO(&stExceptSet);
		FD_SET(mSocket, &stReadSet);
		FD_SET(mSocket, &stExceptSet);
		iEvtCnt = select((int)mSocket + 1, &stReadSet, NULL, &stExceptSet, &stTV);  // Wait on read or error
		if (iEvtCnt <= 0)
		{
			return iEvtCnt;
		}
		if (FD_ISSET(mSocket, &stExceptSet))
		{
			Disconnect();
			return 0;
		}
	}
	struct sockaddr_in stAddrFrom;
	socklen_t ulAddrLen = sizeof(stAddrFrom);
	memset(&stAddrFrom, 0, sizeof(stAddrFrom));
	iRecvLen = recvfrom(mSocket, (char *)(pvData), iMaxLen, 0, (struct sockaddr *)(&stAddrFrom), &ulAddrLen);
	if (iRecvLen <= 0)
	{
		mErrorCode = WSAGetLastError();
		WS_DEBUG("%s RecvLen = %d Recv Error ErrorCode = %d\n", __FUNCTION__, iRecvLen, mErrorCode);
		return iRecvLen;
	}
	if (pdwIP != NULL)
	{
		*pdwIP = ntohl(stAddrFrom.sin_addr.s_addr);
	}
	if (pwPort != NULL)
	{
		*pwPort = ntohs(stAddrFrom.sin_port);
	}
	return iRecvLen;
}


