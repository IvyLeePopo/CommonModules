#include "stdafx.h"
#include "WebSocketThreadBase.h"
//#include <afxwin.h>

// 线程函数
DWORD WINAPI threadProc(LPVOID pArgs)
{
	if (0 == pArgs)
		return 0;

	WebSocketThreadBase* pThreadBase = (WebSocketThreadBase*)pArgs;
	pThreadBase->circle();
	return 0;
}

WebSocketThreadBase::WebSocketThreadBase(void)
{
	m_bCanRun = false;
	m_dThreadId = 0;
	m_nThreadHandle = NULL;

#ifdef WINVER
	InitializeCriticalSection(&m_criSection);
#endif
}

WebSocketThreadBase::~WebSocketThreadBase(void)
{
}

void WebSocketThreadBase::vStart()
{
	m_nThreadHandle = (int)CreateThread(NULL, 0, threadProc, this, 0, &m_dThreadId);
	m_bCanRun = true;
}

void WebSocketThreadBase::vStop()
{
	m_bCanRun = false;
}

bool WebSocketThreadBase::isCanRun()
{
	return m_bCanRun;
}

int WebSocketThreadBase::circle()
{
	return TRUE;
}

void WebSocketThreadBase::_lock()
{
#ifdef WINVER
	EnterCriticalSection(&m_criSection);
#endif
}

void WebSocketThreadBase::_unLock()
{
#ifdef WINVER
	LeaveCriticalSection(&m_criSection);
#endif
}

