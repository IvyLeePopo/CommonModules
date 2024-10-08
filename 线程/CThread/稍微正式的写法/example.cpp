#include "StandTask.h"

class CUnit : public trawe::CStandTask
{
public:
	CUnit();
	virtual ~CUnit();
public:
	virtual void RunBody();

private:
	/*À¯*/
	CRITICAL_SECTION m_Lock;
}


////////////////////////////////////////////////////////////////////////////
CUnit::CUnit()
{
	::InitializeCriticalSection(&m_Lock);
}

CUnit::~CUnit()
{
	Stop();

	::DeleteCriticalSection(&m_Lock);

}

void CUnit::RunBody()
{
	//todo 

	// ø…  µ±Sleep(10);
	Sleep(10);
}
