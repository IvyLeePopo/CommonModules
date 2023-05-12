/**
	实例演示：线程实现一个socket通讯
	
	升级版的线程库直接采用threadbase即可，利于换平台移植
	纯业务，避免直接调用系统的底层库或者编译器特有的数据结构！！
*/
#pragma once
#include <vector>
#include <string>
#include "SingleTon.h"
#include "c_thread_base.h"

#define FailHandle -1

using namespace std;
class TestThread : public SingleTon<TestThread>, public CThreadBase
{
	friend class SingleTon<TestThread>;

public:
	TestThread();
	~TestThread();

	// 返回0失败，否则返回句柄
	virtual int connectSvr(const char* pszIp, int nPort);
	virtual bool disConnectSvr();
	virtual bool isConnected() { return m_bIsTcpConnected; }

	virtual int vThreadWork();

private:

	// 内部连接tcp服务器
	int connectEx(const char* pszIp, int nPort);

	// 发送消息需要组装 nType包类型 0-数据包  1-心跳包
	int sendData(int nType, const char* pszData, int nLen);

	// 接收数据
	int recvData();

private:
	void* m_pParent;				// 父句柄：主要实现回调函数
	bool m_bIsTcpConnected;         // 是否已经连接
	char* m_pszRecvBuf;             // 接收数据缓存
	int m_nMySelfHandle;			// 成员句柄

	SOCKET m_nSocketFd;             // socket句柄 
};