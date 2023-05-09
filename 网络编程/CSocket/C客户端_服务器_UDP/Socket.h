#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "CommInterface.h"

#if IS_WINDOWS
//#define WIN32_LEAN_AND_MEAN //需要到属性C/C++那里定义预编译，这里不行
#include <winsock2.h>
#include <ws2tcpip.h>
#include <IPHlpApi.h>

#define SOCKET_IS_VALID(s) ((s) != INVALID_SOCKET)

#else

#include <netinet/tcp.h>

#define SD_BOTH SHUT_RDWR

#define SOCKET_IS_VALID(s) ((s) > 0)

#endif

#define DEFAULT_RSU_PORT (21003)

class SocketClientInterface: public CommInterface
{
public:
	SocketClientInterface(void);
	SocketClientInterface(const char *pccServerIP, WORD usPort, int iTimoutMs);
	SocketClientInterface(const char *pccArgs);
	~SocketClientInterface(void);
	bool TryConnect(const char *pccServerIP, WORD usPort);
	bool ConnectionConfirm(int iTimeoutMs);
	bool Connect(const char *pccServerIP, WORD usPort, int iTimeoutMs);
	bool Connect(const char *pccArgs);
	void Disconnect(void);
	bool TestAndReconnect(void);
	int RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs);
	int SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iDataLen, int iTimeoutMs);
	int GetErrorCode() { return mErrorCode; }
	bool SetNotBlock(SOCKET uiSocket, bool bNotBlock);
	bool IsConnected(void) { return SOCKET_IS_VALID(mSocket); }
	long GetHandle(void) { return (long)mSocket; }
private:
	char m_cServerIP[16] = { 0 };
	DWORD m_dwServerIP = (127 << 24) + (1 << 0);
	WORD mPort = DEFAULT_RSU_PORT;
	SOCKET mSocket = INVALID_SOCKET, mTempSocket = INVALID_SOCKET;
	int mErrorCode = 0;
	bool mNotBlock = false;
};


class SocketServerInterface : public CommInterface
{
public:
	SocketServerInterface(WORD wPort = DEFAULT_RSU_PORT, WORD wBackLog = 5);
	SocketServerInterface(const char *pccArgs);
	~SocketServerInterface(void);
	bool Connect(WORD wPort = DEFAULT_RSU_PORT, WORD wBackLog = 5);
	bool Connect(const char *pccArgs);
	bool TestAndReconnect(void);
	void Disconnect(void);
	bool IsConnected(void) { return SOCKET_IS_VALID(mSocketAccepted); }

	SOCKET Accept(int iTimeoutMs);
	int RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs);
	int SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iDataLen, int iTimeoutMs);
	int GetErrorCode() { return mErrorCode; }
	bool SetNotBlock(SOCKET uiSocket, bool bNotBlock);

	long GetHandle(void) { return (long)mSocketAccepted; }

	long GetAcceptedHandle(void) { return (long)mSocketAccepted; }
	long GetListennerHandle(void) { return (long)mSocketListenner; }

private:
	char m_cClientIPStr[16] = { 0 };
	DWORD m_dwClientIP = 0;
	WORD mPort = DEFAULT_RSU_PORT, mBackLog = 5, mClientPort = 0;
	SOCKET mSocketListenner = INVALID_SOCKET;
	SOCKET mSocketAccepted = INVALID_SOCKET;
	int mErrorCode = 0;
	bool mNotBlock = false;
};


class SocketUdpInterface : public CommInterface
{
public:
	SocketUdpInterface(WORD wPort = DEFAULT_RSU_PORT);
	SocketUdpInterface(const char *pccArgs);
	
	~SocketUdpInterface(void);
	bool Connect(WORD usPort);
	bool Connect(const char *pccArgs);
	bool TestAndReconnect(void);
	void Disconnect(void);
	bool IsConnected(void) { return SOCKET_IS_VALID(mSocket); }
	int RecvFrom(DWORD *pdwIP, WORD *pwPort, void *pvData, int iMaxLen, int iTimeoutMs);
	int SendTo(DWORD dwIP, WORD wPort, const void *pcvData, int iDataLen, int iTimeoutMs);
	int GetErrorCode() { return mErrorCode; }
	bool SetNotBlock(SOCKET uiSocket, bool bNotBlock);
	long GetHandle(void) { return (long)mSocket; }
private:
	SOCKET mSocket = INVALID_SOCKET;
	WORD mPort = DEFAULT_RSU_PORT;
	int mErrorCode = 0;
	bool mNotBlock = false;
	DWORD mSrcIP;
	WORD mSrcPort;
};
/*****************************************************************
 函数名称：WinSocketGetHostIPs
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int WinSocketGetHostIPs(char cIPs[][16], int iMaxCnt);

#endif
