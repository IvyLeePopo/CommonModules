#include "CSocket.h"

SocketClientInterface::SocketClientInterface(const char * pccServerIP, WORD usPort, int iTimoutMs)
{
}

SocketClientInterface::~SocketClientInterface(void)
{
}

bool SocketClientInterface::Connect(const char * pccServerIP, WORD usPort, int iTimeoutMs)
{
	struct sockaddr_in unSockAddrClient;
	if (pccServerIP != NULL)
	{
		m_Port = wPort;
		strncpy_s(m_cServerIP, pccServerIP, sizeof(m_cServerIP));
	}
	Disconnect();//关闭原本的socket，如果有的话

	m_TempSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建新的socket
	if (!SOCKET_IS_VALID(m_TempSocket))
	{
		return false;
	}
	if (SetNotBlock(m_TempSocket, true) != true)
	{
		Disconnect();
		return false;
	}

	unSockAddrClient.sin_family = AF_INET;
	unSockAddrClient.sin_addr.s_addr = inet_addr(m_cServerIP);
	unSockAddrClient.sin_port = htons(m_Port);
	m_dwServerIP = ntohl(unSockAddrClient.sin_addr.s_addr);

	if (connect(m_TempSocket, (struct sockaddr *)&unSockAddrClient, sizeof(unSockAddrClient)) != 0)
	{
		return false;
	}

	ConnectionConfirm(iTimeoutMs);
	return true;
}

void SocketClientInterface::Disconnect(void)
{
	if (SOCKET_IS_VALID(m_Socket))
	{
		shutdown(m_Socket, SD_BOTH);
		closesocket(m_Socket);
	}
	if ((m_Socket != m_TempSocket) && (SOCKET_IS_VALID(m_TempSocket)))
	{
		shutdown(m_TempSocket, SD_BOTH);
		closesocket(m_TempSocket);
	}
	m_Socket = m_TempSocket = INVALID_SOCKET;
}

bool SocketClientInterface::ConnectionConfirm(int iTimeoutMs)
{
	int iEvtCnt, iOptions = true;
	struct timeval stTV;//timeval 结构体通常用于设置超时时间
	fd_set stWriteSet, stExceptSet;//fd_set是一个文件描述符集合

	if (SOCKET_IS_VALID(m_TempSocket) && (iTimeoutMs >= 0))
	{
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stExceptSet);//将fd_set清空，即将所有位都设置为0
		FD_SET(m_TempSocket, &stWriteSet);//将文件描述符fd对应的位设置为1
		FD_SET(m_TempSocket, &stExceptSet);
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		iEvtCnt = select((int)m_TempSocket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);//select 函数时，可以设置一个 timeval 结构体作为超时时间
		if ((iEvtCnt <= 0) || (FD_ISSET(m_TempSocket, &stExceptSet)))//FD_ISSET：判断文件描述符fd对应的位是否为1
		{
			Disconnect();
			return false;
		}
	}

	m_Socket = m_TempSocket;
	m_TempSocket = INVALID_SOCKET;
	if (!SOCKET_IS_VALID(m_Socket))
	{
		return false;
	}

	//用于设置套接字选项的函数
	/*int setsockopt(
					int sockfd 套接字描述符, 
					int level表示选项所在的协议层, 
					int optname选项名称, 
					const void *optval选项的值,
					socklen_t optlen选项值的长度
					);*/

	if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		//WS_DEBUG("%s setsockopt SO_REUSEADDR Error\n", __FUNCTION__);
		return false;
	}
	iOptions = true;
	if (setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		return false;
	}
	iOptions = true;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		return false;
	}
#if IS_WINDOWS
	iOptions = 10 * 1000;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		return false;
	}
	iOptions = 10 * 1000;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		return false;
	}
#endif
	iOptions = 128 * 1024;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		return false;
	}
	iOptions = 128 * 1024;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char *)&iOptions, sizeof(int)) != 0)
	{
		Disconnect();
		return false;
	}
	return true;
}

bool SocketClientInterface::SetNotBlock(SOCKET uiSocket, bool bNotBlock)
{
	u_long ulNotBlock = bNotBlock;
	if (!SOCKET_IS_VALID(uiSocket))
	{
		return false;
	}

	//ioctlsocket是Windows Sockets API中的一个函数，用于控制套接字的各种属性和操作
	//int ioctlsocket(SOCKET s, /*套接字句柄*/ long cmd, /*控制命令*/	u_long *argp /*控制参数*/ );
	if (ioctlsocket(uiSocket, FIONBIO, &ulNotBlock) != 0)
	{
		return false;
	}
	m_NotBlock = bNotBlock;
	return true;
}
