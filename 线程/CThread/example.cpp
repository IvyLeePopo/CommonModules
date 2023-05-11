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

TestThread::TestThread()
{
}

TestThread::~TestThread()
{
}

int TestThread::vThreadWork()
{
	//在线程中实行
	while (isCanRun)
	{
		//线程同步：临界区
		_lock();

		// todo

		_unLock();

		Sleep(1000 * 1);		// 1秒钟更新一次
	}
	return 0;
}

void TestThread::start()
{
	vStart();
}

void TestThread::stop()
{
	vStop();
}
