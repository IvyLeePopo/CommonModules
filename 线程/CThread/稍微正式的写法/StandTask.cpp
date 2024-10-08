#include "StandTask.h"
#include <string>


#define TW_TRACE_DIARY_TEST(x)

namespace trawe
{
	void DebugInfoStr(std::string info)
	{
		info += '\n';
		printf("%s", info.c_str());
		//OutputDebugString(info.c_str());
	}
	void DebugInfo(const char* info)
	{
		DebugInfoStr(std::string(info));
	}
	//IMPLEMENT_DYNCREATE(CStandTask, CWinThread)

	DWORD WINAPI CStandTask::ThreadProc(LPVOID pParam)
	{
		CStandTask* core_handle = (CStandTask*)pParam;
		core_handle->Execute();
		return 0;
	}
	CStandTask::CStandTask()
	{
		::InitializeCriticalSection(&m_ctlMutex);
		m_coreThreadCtrl = NULL;
		m_isAlive = false;

		m_coreThreadCtrl = ::AfxBeginThread(AFX_THREADPROC(ThreadProc), this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		if (m_coreThreadCtrl != NULL)
		{
			m_isAlive = true;
		}
		m_isThreadExit = true;

		//printf("new id :%d",m_coreThreadCtrl->m_nThreadID);
	}
	CStandTask::~CStandTask()
	{
		if (IsAlive())
		{
			Stop();
		}
		while (!m_isThreadExit)
		{
			Sleep(1);
		}

		Sleep(50);//确保Execute退出
		::DeleteCriticalSection(&m_ctlMutex);
	}

	void CStandTask::RunBody()
	{
		TW_TRACE_DIARY_TEST("empty idle");
		Sleep(1000);
	}
	void CStandTask::Execute()
	{
		m_isThreadExit = false;
		while (true)
		{
			{
				CScopeLock ctrl_access(&m_ctlMutex);
				if (!IsAlive())
				{
					TW_TRACE_DIARY_TEST("线程结束!!!");
					m_isThreadExit = true;
					return;
				}
				RunBody();
			}
			Sleep(1);

		}
		CScopeLock ctrl_access(&m_ctlMutex);
		m_isAlive = false;
	}
	void CStandTask::Resume()
	{
		CScopeLock ctrl_access(&m_ctlMutex);
		m_coreThreadCtrl->ResumeThread();
		m_isAlive = true;
	}
	bool CStandTask::IsAlive()
	{
		CScopeLock ctrl_access(&m_ctlMutex);
		return m_isAlive;
	}
	bool CStandTask::Stop()
	{
		if (!IsAlive())
		{
			return true;
		}
		TW_TRACE_DIARY_TEST("尝试停止线程");
		{
			CScopeLock ctrl_access(&m_ctlMutex);
			m_isAlive = false;
		}
		Sleep(30);
		while (true)
		{
			if (m_isThreadExit)
			{
				break;
			}
			Sleep(20);
		}

		TW_TRACE_DIARY_TEST("停止线程成功 ");
		Sleep(30);

		return true;
	}

	DWORD CStandTask::GetThreadId()
	{
		CScopeLock ctrl_access(&m_ctlMutex);
		if (IsAlive())
		{
			return m_coreThreadCtrl->m_nThreadID;
		}
		else
		{
			return 0;
		}
	}

}//end of namespace 
