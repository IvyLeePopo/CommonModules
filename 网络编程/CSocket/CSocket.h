/*
 * @Description: C���Ե�Socket
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
 //#define WIN32_LEAN_AND_MEAN //��Ҫ������C/C++���ﶨ��Ԥ���룬���ﲻ��
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

//�ͻ���
class SocketClientInterface
{
public:
	SocketClientInterface(const char *pccServerIP, WORD usPort, int iTimoutMs);
	~SocketClientInterface(void);

	//����
	bool Connect(const char *pccServerIP, WORD usPort, int iTimeoutMs);
	void Disconnect(void);
	bool IsConnected(void) { return SOCKET_IS_VALID(mSocket); }

private:
	//���ӳɹ������������ã��糬ʱ
	bool ConnectionConfirm(int iTimeoutMs);

	//�����׽��ֵķ�����ģʽ
	bool SetNotBlock(SOCKET uiSocket, bool bNotBlock);

private:
	char m_cServerIP[16] = { 0 };
	DWORD m_dwServerIP = (127 << 24) + (1 << 0);
	WORD m_Port = DEFAULT_RSU_PORT;
	SOCKET m_Socket = INVALID_SOCKET, m_TempSocket = INVALID_SOCKET;

	bool m_NotBlock = false;
}��