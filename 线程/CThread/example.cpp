#include "c_thread_base.h"
/**
	实例演示：
*/
#pragma once
#include <vector>
#include <string>
#include "SingleTon.h"
#include "ThreadBase.h"
#include "example.h"



bool initWSAData()
{
#define WS_VERSION_CHOICE1 0x202
#define WS_VERSION_CHOICE2 0x101

	WSADATA wsaData;
	if (WSAStartup(WS_VERSION_CHOICE1, &wsaData) != 0 &&
		WSAStartup(WS_VERSION_CHOICE2, &wsaData) != 0)
	{
		return false;
	}

	if (WS_VERSION_CHOICE1 != wsaData.wVersion &&
		WS_VERSION_CHOICE2 != wsaData.wVersion)
	{
		WSACleanup();
		return false;
	}

	return true;
}

bool unInitWSAData()
{
	WSACleanup();
	return true;
}


TestThread::TestThread()
{
}

TestThread::~TestThread()
{
}

int TestThread::connectSvr(const char * pszIp, int nPort)
{
	if (nullptr == pszIp)
	{
		return 0;
	}

	if (isConnected())
	{
		return m_nMySelfHandle;
	}

	m_strIP = pszIp;
	m_nPort = nPort;
	int m_nMySelfHandle = connectEx(pszIp, nPort);
	if (FailHandle != m_nMySelfHandle)
	{
		vStart();// 线程开始
	}
	return m_nMySelfHandle;
}

bool TestThread::disConnectSvr()
{
	vStop(); // 线程结束
	m_bIsTcpConnected = false;
	m_nMySelfHandle = FailHandle;
	return true;
}

int TestThread::vThreadWork()
{
	//在线程中实行
	while (isCanRun)
	{
		// 重连
		if (!m_bIsTcpConnected)
		{
			if (!connectEx(m_strIP.c_str(), m_nPort))
			{
				Sleep(1000 * 10);       // 暂停10秒，10秒之后再一次尝试吧
				continue;
			}
			// todo
		}

		//线程同步：临界区
		_lock();
		// 进入接收逻辑
		if (0 > recvData())
		{
			Sleep(10);
		}
		_unLock();

		// 15秒钟定时发送心跳：可以设计发送心跳
		nHeartBeatTime = (int)Helper->getCurrentTimeStamp();//时间戳
		if (nHeartBeatTime - nStartTime > 15)
		{
			sendHeartBeat();
			nStartTime = nHeartBeatTime;
		}
	}
	return 0;
}

int TestThread::connectEx(const char * pszIp, int nPort)
{
	if (NULL == pszIp)
	{
		WriteLogErr("camera ip is err!");
		return 0;
	}

	struct sockaddr_in clientAddr;
	unsigned long lIpAddr = inet_addr(pszIp);

	bool bRet = false;
	do
	{
		if (INADDR_NONE == lIpAddr)
		{
			WriteLogErr("please input right camera ip !");
			break;
		}

		if (!initWSAData())
		{
			WriteLogErr("initWSAData init fail！");
			break;
		}

		m_nSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == m_nSocketFd)
		{
			WriteLogErr("socket fail!");
			unInitWSAData();
			break;
		}

		unsigned long arg = 0;
		if (0 != ioctlsocket(m_nSocketFd, FIONBIO, &arg))
		{
			WriteLogErr("ioctlsocket fail!");
			unInitWSAData();
			break;
		}

		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(nPort);
		clientAddr.sin_addr.s_addr = lIpAddr;
		if (connect(m_nSocketFd, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) != 0)
		{
			WriteLogErr("connect fail!");
			closesocket(m_nSocketFd);
			unInitWSAData();
			break;
		}

		bRet = true;
		m_bIsTcpConnected = true;
		WriteLogInfo("connect success! ip : " + std::string(pszIp));
	} while (false);

	return bRet ? getHandle() : 0;
}

int TestThread::sendData(int nType, const char * pszData, int nLen)
{
	if (!m_bIsTcpConnected)
	{
		WriteLogErr("摄像机未连接，请连接");
		return -1;
	}

	int nRet = 0;
	do
	{
		if (nullptr == pszData)
		{
			WriteLogErr("摄像机请求命令为null，请检查");
			break;
		}
		//根据协议组建发送的数据
		nRet = send(m_nSocketFd, pszData, nLen, 0);
	} while (false);

	if (EPIPE == nRet)
	{
		// 断线
		m_bIsTcpConnected = false;
	}

	return nRet;
}



int TestThread::recvData()
{
	fd_set readSet;
	struct timeval tv = { 1, 0 };
	tv.tv_sec = 1;
	FD_ZERO(&readSet);
	FD_SET(m_nSocketFd, &readSet);

	int nRet = select(m_nSocketFd + 1, &readSet, NULL, NULL, &tv);
	if (-1 == nRet)
	{
		WriteLogErr(Helper->format("摄像机断线了"));
		return -1;
	}
	else if (0 == nRet)
	{
		return 0;
	}

	FD_SET(m_nSocketFd, &readSet);
	unsigned int nPacketSize = (int)getPacketSize(m_nSocketFd);
	if (nPacketSize > 0)
	{
		// 去继续接收包数据
		char* pszRecvBuf = new char[nPacketSize];
		memset(pszRecvBuf, 0, nPacketSize);

		unsigned int nRecvSize = 0;
		unsigned int nTotleRecvSize = 0;
		while (nTotleRecvSize < nPacketSize)
		{
			nRecvSize = recv(m_nSocketFd, &pszRecvBuf[nTotleRecvSize], nPacketSize - nTotleRecvSize, 0);

			if (nRecvSize > 0)
			{
				nTotleRecvSize += nRecvSize;
				continue;
			}

			delete[] pszRecvBuf;
			pszRecvBuf = nullptr;
			return 0;
		}

		parseVzMessage(pszRecvBuf, nPacketSize);//解析数据，根据实际来
		delete[] pszRecvBuf;
		pszRecvBuf = nullptr;
		return 1;
	}
	return 0;
}
