/*
 * @Description: C线程
 * @Author: Ivy
 * @Date: 2022-05-11 09:34:43
 * @LastEditTime: 2022-05-11 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <Windows.h>
class CThreadBase
{
public:
	CThreadBase(void);
	virtual ~CThreadBase(void);

public:
	// 开启线程
	void vStart();

	// 结束线程
	virtual void vStop();

	// 线程是否可运行
	virtual bool isCanRun();

	// 子类重写此函数用于接收线程逻辑
	virtual int vThreadWork();

	// 入锁、解锁临界区，不同的编译环境修改成其他的同步条件即可
	void _lock();
	void _unLock();
private:
protected:
	DWORD m_dThreadId;						// 线程ID	
	bool m_bCanRun;							// 是否可以运行
	int m_nThreadHandle;					// 线程句柄
#ifdef WINVER
	CRITICAL_SECTION m_criSection;		// 临界区，定义一个，需要数据同步的自取即可
#endif
};
