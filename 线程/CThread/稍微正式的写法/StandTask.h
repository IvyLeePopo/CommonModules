#ifndef STAND_TASK_H
#define STAND_TASK_H
#include <afxwin.h>

namespace trawe {
	void DebugInfo(const char* info);

class CStandTask //: public CWinThread
{
	//DECLARE_DYNCREATE(CStandTask)
public:
    CStandTask();
    virtual ~CStandTask();
public:
	static DWORD WINAPI ThreadProc(LPVOID pParam);
public:
    virtual void RunBody();
    void Execute();
    void Resume();
    bool IsAlive();
    bool Stop();
	DWORD GetThreadId();
private:
    bool m_isAlive;
	bool m_isThreadExit;
    CRITICAL_SECTION m_ctlMutex;//Ïß³Ì¿ØÖÆ»¥³â
    CWinThread* m_coreThreadCtrl;
private:
    CStandTask(const CStandTask&);
    CStandTask& operator = (const CStandTask&);

};



class CScopeLock
{
public:
    CScopeLock(CRITICAL_SECTION* lck)
        : mMutex(lck)
    {
        if(mMutex!=NULL)
        {
            ::EnterCriticalSection(mMutex);
        }
        
    }
    ~CScopeLock()
    {
        if(mMutex!=NULL)
        {
             ::LeaveCriticalSection(mMutex);
        }
       
    }
private:
    CRITICAL_SECTION* mMutex;
private:
    CScopeLock(const CScopeLock&);
    CScopeLock& operator = ( const CScopeLock&);

};
} // trawe



#endif // STAND_TASK_H
