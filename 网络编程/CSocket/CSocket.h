/*
 * @Description: C语言的Socket
 * @Author: Ivy
 * @Date: 2022-04-26 09:34:43
 * @LastEditTime: 2023-02-26 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#if (defined(_WIN32)) || (defined(_WIN64))
#define IS_WINDOWS 1
#else
#define IS_WINDOWS 0
#endif

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

//客户端
class SocketClientInterface
{
public:
	SocketClientInterface(const char *pccServerIP, WORD usPort, int iTimoutMs);
	~SocketClientInterface(void);

	//连接
	bool Connect(const char *pccServerIP, WORD usPort, int iTimeoutMs);
	void Disconnect(void);
	bool IsConnected(void) { return SOCKET_IS_VALID(mSocket); }

private:
	//连接成功后，其他的设置，如超时
	bool ConnectionConfirm(int iTimeoutMs);

	//设置套接字的非阻塞模式
	bool SetNotBlock(SOCKET uiSocket, bool bNotBlock);

private:
	char m_cServerIP[16] = { 0 };
	DWORD m_dwServerIP = (127 << 24) + (1 << 0);
	WORD m_Port = DEFAULT_RSU_PORT;
	SOCKET m_Socket = INVALID_SOCKET, m_TempSocket = INVALID_SOCKET;

	bool m_NotBlock = false;
}；