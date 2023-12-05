/*
	升级版的日志，线程库直接采用threadbase即可，利于换平台移植
	纯业务，避免直接调用系统的底层库或者编译器特有的数据结构！！
*/
#pragma once
#include <vector>
#include <string>
#include "SingleTon.h"
#include "ThreadBase.h"

using namespace std;
class AutoLogEx : public SingleTon<AutoLogEx>, public CThreadBase
{
	friend class SingleTon<AutoLogEx>;
	//内部结构声明
public:
	// 日志级别
	enum enLogInfoLevel
	{
		ENUM_LOG_LEVEL_DEBUG = 0,
		ENUM_LOG_LEVEL_RUN,
		ENUM_LOG_LEVEL_INFO,
		ENUM_LOG_LEVEL_WARNING,
		ENUM_LOG_LEVEL_ERROR,
	};

	// 日志写入器的状态
	enum enLogStatus
	{
		ENUM_LOG_INIT = 0,		//文件处于初始化状态
		ENUM_LOG_RUN,			//文件处于运行状态
		ENUM_LOG_EXITING,		//文件处于退出过程状态
		ENUM_LOG_EXITED,		//文件已经关闭
		ENUM_LOG_INVALID,		//文件非法
	};

	struct _tagLogData
	{
		int nLine;				// 日志行数
		int nLevel;				// 日志等级
		string strLog;			// 具体的日志内容
		string strFunc;			// 函数名
		string strTime;			// 产生时间，示例 2022-12-16 12:10:10

		_tagLogData& operator = (const _tagLogData& tag)
		{
			if (this == &tag)
			{
				return *this;
			}

			nLine = tag.nLine;
			nLevel = tag.nLevel;
			strLog = tag.strLog;
			strFunc = tag.strFunc;
			strTime = tag.strTime;
			return *this;
		}
	};

public:
	AutoLogEx();
	~AutoLogEx();
	virtual int vThreadWork();

public:
	// 启动日志 strDir日志目录，strSharedMap的名字(每一个单例类都要有自己的存储共享名称)
	void startLog(string strDir, string strSharedMap);

	// 设置日志路径
	void setDir(string strDir);

	// 获取日志文件名
	string getFileName();

	// 写日志
	void writeLog(int nLevel, string strLog, int nLine, string strFunc);

	// 删除日志文件
	void deleteLogFile(string strDir, int days);

private:
	// 初始化日志
	void init();

	// 自动清除日志
	void autoClearLog();

	// 生成日志格式内容
	string generateLog(_tagLogData& _data);

	// 获取当前时间戳
	string getCurrentTimeString();

private:
	int m_nLogNum;							// 日志编号，按照日期来清零
	string m_strDir;						// 日志的路径
	string m_strSharedMap;					// 共享内存的键值
	string m_strLogFileName;				// 日志文件名
	int m_nCurDate;							// 当前的日期缓存,例如:20221213
	vector<_tagLogData> m_vecLogData;		// 日志缓存
	int m_nSaveDays;						// 日志保存的最近天数
};

/* 需要特别注意，在任何一次WriteLog的调用，之前，必须要执行一次 AutoLogEx::getInstance()->startLog,否则不会执行日志记录	*/
#define WriteLog(_type, str, line, funcname) AutoLogEx::getInstance()->writeLog(_type, str, line, funcname)
#define WriteLogInfo(log) WriteLog(AutoLogEx::ENUM_LOG_LEVEL_INFO, log, __LINE__, __FUNCTION__);
#define WriteLogErr(log) WriteLog(AutoLogEx::ENUM_LOG_LEVEL_ERROR, log, __LINE__, __FUNCTION__);
#define WriteLogWarning(log) WriteLog(AutoLogEx::ENUM_LOG_LEVEL_WARNING, log, __LINE__, __FUNCTION__);