#include "ThreadBase.h"
//#include <windows.h>
//#include <afxwin.h>

// Ïß³Ìº¯Êý
DWORD WINAPI threadProc(LPVOID pArgs)
{
	if (0 == pArgs)
		return 0;

	CThreadBase* pThreadBase = (CThreadBase*)pArgs;
	pThreadBase->vThreadWork();
	return 0;
}

CThreadBase::CThreadBase(void)
{
	m_bCanRun = false;
	m_dThreadId = 0;
	m_nThreadHandle = NULL;
#ifdef WINVER
	InitializeCriticalSection(&m_criSection);
#endif
}

CThreadBase::~CThreadBase(void)
{
}

void CThreadBase::vStart()
{
	m_nThreadHandle = (int)CreateThread(NULL, 0, threadProc, this, 0, &m_dThreadId);
	m_bCanRun = true;
}

void CThreadBase::vStop()
{
	m_bCanRun = false;
}

bool CThreadBase::isCanRun()
{
	return m_bCanRun;
}

int CThreadBase::vThreadWork()
{
	return 1;
}

void CThreadBase::_lock()
{
#ifdef WINVER
	EnterCriticalSection(&m_criSection);
#endif
}

void CThreadBase::_unLock()
{
#ifdef WINVER
	LeaveCriticalSection(&m_criSection);
#endif
}