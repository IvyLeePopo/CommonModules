#include "StdAfx.h"
#include "Helper.h"

CHelper::CHelper(void)
{
}

CHelper::~CHelper(void)
{
}

long CHelper::getCurrentTimeStamp(bool bMs /*= false*/)
{
	long nTime;
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	CTime cTime(sysTime);
	nTime = (long)cTime.GetTime();
	if (!bMs)
		return nTime;

	nTime = nTime*1000 + sysTime.wMilliseconds;
	return nTime;
}