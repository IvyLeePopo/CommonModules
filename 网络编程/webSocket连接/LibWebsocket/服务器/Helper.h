#pragma once
#include "SingleTon.h"

#ifndef nullptr
#define nullptr NULL
#endif

class CHelper : public SingleTon<CHelper>
{
	friend class SingleTon<CHelper>;

public:
	CHelper(void);
	virtual ~CHelper(void);
	
	// 获取时间戳,精确到秒，int类型
	long getCurrentTimeStamp(bool bMs = false); 


public:
};

#define CheckResult(a)  if(!(a)) \
  return;
#define CheckResultWithRet(a, b)  if(!(a)) \
  return (b);

#define Helper CHelper::getInstance()
