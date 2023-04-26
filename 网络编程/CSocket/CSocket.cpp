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
	Disconnect();//�ر�ԭ����socket������еĻ�

	m_TempSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����µ�socket
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
	struct timeval stTV;//timeval �ṹ��ͨ���������ó�ʱʱ��
	fd_set stWriteSet, stExceptSet;//fd_set��һ���ļ�����������

	if (SOCKET_IS_VALID(m_TempSocket) && (iTimeoutMs >= 0))
	{
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stExceptSet);//��fd_set��գ���������λ������Ϊ0
		FD_SET(m_TempSocket, &stWriteSet);//���ļ�������fd��Ӧ��λ����Ϊ1
		FD_SET(m_TempSocket, &stExceptSet);
		stTV.tv_sec = iTimeoutMs / 1000;
		stTV.tv_usec = (iTimeoutMs % 1000) * 1000;
		iEvtCnt = select((int)m_TempSocket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);//select ����ʱ����������һ�� timeval �ṹ����Ϊ��ʱʱ��
		if ((iEvtCnt <= 0) || (FD_ISSET(m_TempSocket, &stExceptSet)))//FD_ISSET���ж��ļ�������fd��Ӧ��λ�Ƿ�Ϊ1
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

	//���������׽���ѡ��ĺ���
	/*int setsockopt(
					int sockfd �׽���������, 
					int level��ʾѡ�����ڵ�Э���, 
					int optnameѡ������, 
					const void *optvalѡ���ֵ,
					socklen_t optlenѡ��ֵ�ĳ���
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

	//ioctlsocket��Windows Sockets API�е�һ�����������ڿ����׽��ֵĸ������ԺͲ���
	//int ioctlsocket(SOCKET s, /*�׽��־��*/ long cmd, /*��������*/	u_long *argp /*���Ʋ���*/ );
	if (ioctlsocket(uiSocket, FIONBIO, &ulNotBlock) != 0)
	{
		return false;
	}
	m_NotBlock = bNotBlock;
	return true;
}
