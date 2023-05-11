#include "c_thread_base.h"
/**
	实例演示：
*/
#pragma once
#include <vector>
#include <string>
#include "SingleTon.h"
#include "ThreadBase.h"

using namespace std;
class TestThread : public SingleTon<TestThread>, public CThreadBase
{
	friend class SingleTon<TestThread>;

public:
	TestThread();
	~TestThread();
	virtual int vThreadWork();

	void start();
	void stop();

};