/*
	���������־���߳̿�ֱ�Ӳ���threadbase���ɣ����ڻ�ƽ̨��ֲ
	��ҵ�񣬱���ֱ�ӵ���ϵͳ�ĵײ����߱��������е����ݽṹ����
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
	//�ڲ��ṹ����
public:
	// ��־����
	enum enLogInfoLevel
	{
		ENUM_LOG_LEVEL_DEBUG = 0,
		ENUM_LOG_LEVEL_RUN,
		ENUM_LOG_LEVEL_INFO,
		ENUM_LOG_LEVEL_WARNING,
		ENUM_LOG_LEVEL_ERROR,
	};

	// ��־д������״̬
	enum enLogStatus
	{
		ENUM_LOG_INIT = 0,		//�ļ����ڳ�ʼ��״̬
		ENUM_LOG_RUN,			//�ļ���������״̬
		ENUM_LOG_EXITING,		//�ļ������˳�����״̬
		ENUM_LOG_EXITED,		//�ļ��Ѿ��ر�
		ENUM_LOG_INVALID,		//�ļ��Ƿ�
	};

	struct _tagLogData
	{
		int nLine;				// ��־����
		int nLevel;				// ��־�ȼ�
		string strLog;			// �������־����
		string strFunc;			// ������
		string strTime;			// ����ʱ�䣬ʾ�� 2022-12-16 12:10:10

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
	// ������־ strDir��־Ŀ¼��strSharedMap������(ÿһ�������඼Ҫ���Լ��Ĵ洢��������)
	void startLog(string strDir, string strSharedMap);

	// ������־·��
	void setDir(string strDir);

	// ��ȡ��־�ļ���
	string getFileName();

	// д��־
	void writeLog(int nLevel, string strLog, int nLine, string strFunc);

	// ɾ����־�ļ�
	void deleteLogFile(string strDir, int days);

private:
	// ��ʼ����־
	void init();

	// �Զ������־
	void autoClearLog();

	// ������־��ʽ����
	string generateLog(_tagLogData& _data);

	// ��ȡ��ǰʱ���
	string getCurrentTimeString();

private:
	int m_nLogNum;							// ��־��ţ���������������
	string m_strDir;						// ��־��·��
	string m_strSharedMap;					// �����ڴ�ļ�ֵ
	string m_strLogFileName;				// ��־�ļ���
	int m_nCurDate;							// ��ǰ�����ڻ���,����:20221213
	vector<_tagLogData> m_vecLogData;		// ��־����
	int m_nSaveDays;						// ��־������������
};

/* ��Ҫ�ر�ע�⣬���κ�һ��WriteLog�ĵ��ã�֮ǰ������Ҫִ��һ�� AutoLogEx::getInstance()->startLog,���򲻻�ִ����־��¼	*/
#define WriteLog(_type, str, line, funcname) AutoLogEx::getInstance()->writeLog(_type, str, line, funcname)
#define WriteLogInfo(log) WriteLog(AutoLogEx::ENUM_LOG_LEVEL_INFO, log, __LINE__, __FUNCTION__);
#define WriteLogErr(log) WriteLog(AutoLogEx::ENUM_LOG_LEVEL_ERROR, log, __LINE__, __FUNCTION__);
#define WriteLogWarning(log) WriteLog(AutoLogEx::ENUM_LOG_LEVEL_WARNING, log, __LINE__, __FUNCTION__);