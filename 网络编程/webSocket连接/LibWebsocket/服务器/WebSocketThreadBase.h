#pragma once
#include "singleton.h"
#include <windows.h>

class WebSocketThreadBase
{
public:
	WebSocketThreadBase(void);
	virtual ~WebSocketThreadBase(void);

public:
	// 开启线程
	void vStart();

	// 结束线程
	virtual void vStop();

	// 线程是否可运行
	virtual bool isCanRun();

	// 子类重写此函数用于接收线程逻辑
	virtual int circle();

	// 入锁、解锁临界区，不同的编译环境修改成其他的同步条件即可
	void _lock();
	void _unLock();

protected:
	DWORD m_dThreadId;						// 线程ID	
	bool m_bCanRun = false;					// 是否可以运行
	int m_nThreadHandle;					// 线程句柄

#ifdef WINVER
	CRITICAL_SECTION m_criSection;		// 临界区，定义一个，需要数据同步的自取即可
#endif
};
