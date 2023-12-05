#include "AutoLogEx.h"
#include "Helper.h"

#define MAX_ONEDATE_FILE_COUNT	1000					// һ�������Բ�������־�ļ�����
#define MAX_ALL_FILE_COUNT		10000					// ������־�ļ�����������
//#define TIMEOUT_CLEARLOG		259200					// 3600*24*30 Ĭ��30�����־
#define TIMEOUT_CLEARLOG		5						// Ĭ��10�����־
#define MAX_TOTOL_LOGFILESIZE	100*1024*1024			// ������־�����������ݳ���
#define MAX_SINGLE_LOGFILESIZE	10*1024*1024			// ÿһ����־�����ݳ��Ƚ���

AutoLogEx::AutoLogEx()
{
	m_nLogNum = 0;												// Ĭ����0
	m_strDir = Helper->getModulePath() + "\\LogFile";			// Ĭ�ϵ�Ŀ¼�ļ�
	m_nCurDate = Helper->getCurrentDate();
	m_nSaveDays = 60;
	init();
}

AutoLogEx::~AutoLogEx()
{
	// �������˳�����Ҫ������չ����ڴ�
	//Helper->clearSharedFileMap(m_strSharedMap.c_str());
}

int AutoLogEx::vThreadWork()
{
	// �߳����Լ�ȥ��¼��־
	while (true)
	{
		_lock();
		if (!m_vecLogData.empty())
		{
			string strLogData;
			for (vector<_tagLogData>::iterator iter = m_vecLogData.begin();
				iter != m_vecLogData.end(); ++iter)
			{
				strLogData += generateLog(*iter);
			}

			if (!strLogData.empty())
			{
				// д���ļ��Լ���������ڴ�
				string strFileName = getFileName();
				Helper->setFileContent(strFileName, strLogData, "a+");
				//Helper->clearSharedFileMap(m_strSharedMap.c_str());
			}
			m_vecLogData.clear();
		}
		_unLock();
		autoClearLog();
		Sleep(1000 * 1);		// 1���Ӹ���һ��
	}
	return 0;
}

void AutoLogEx::startLog(string strDir, string strSharedMap)
{
	setDir(strDir);
	m_strSharedMap = strSharedMap;
	
	// ��ȥ�ѹ����ڴ���ߵ����ݶ�ȡ��������д����־
	string strData;
	//Helper->readSharedFileMap(m_strSharedMap.c_str(), strData);
	if (!strData.empty())
	{
		strData = "***************��ȡ�쳣�˳�����־ - ��ʼ***************\r\n" + strData + "***************��ȡ�쳣�˳�����־ - ����***************\r\n";
		string strFileName = getFileName();
		Helper->setFileContent(strFileName, strData, "a+");
	}

	vStart();
	WriteLogInfo("������������־��ʼ��¼");
}

void AutoLogEx::setDir(string strDir)
{
	// ǿ���޸���־·����Ĭ���Ƿ��� "LogFile"��
	m_strDir = strDir;
	if (!Helper->isExistFile(m_strDir, true))
	{
		Helper->createNewFileOrDir(m_strDir, true);
	}
}

std::string AutoLogEx::getFileName()
{
	string strFileName;
	if (Helper->getCurrentDate() != m_nCurDate)
	{
		// ��ͬ�����ڣ���Ҫ�ı��ļ�����
		m_nLogNum = 0;
		m_nCurDate = Helper->getCurrentDate();
	}

	// ������һ������������־������Ҫ��ʱɾ������Ȼ�����Ժ�С�����ǲ�Ҫ�Ź��κ�һ�����³������Ĳ���
	if (m_nLogNum > MAX_ONEDATE_FILE_COUNT)
	{
		m_nLogNum = 0;
		Helper->deleteFile(Helper->format("Log-%d_%d.log", m_nCurDate, m_nLogNum).c_str());
	}

	for (;;)
	{
		strFileName = Helper->format("Log-%d_%d.log", m_nCurDate, m_nLogNum);
		if (Helper->isExistFile(strFileName))
		{
			long lSize = Helper->getFileSize(strFileName.c_str());
			if (lSize >= MAX_SINGLE_LOGFILESIZE)		// ����ÿһ����־���Ϊ10MB
			{
				// �ҵ��ļ�������size�������ߴ磬˵������־�ļ��Ѿ�����
				m_nLogNum++;
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return m_strDir + strFileName;
}

void AutoLogEx::writeLog(int nLevel, string strLog, int nLine, string strFunc)
{
	// д����־
	_tagLogData logData;
	logData.nLevel = nLevel;
	logData.strLog = strLog;
	logData.strFunc = strFunc;
	logData.nLine = nLine;
	
	string strLogData;
	_lock();
	m_vecLogData.push_back(logData);
	for (vector<_tagLogData>::iterator iter = m_vecLogData.begin();
		iter != m_vecLogData.end(); ++iter)
	{
		_tagLogData& itLogData = *iter;
		strLogData += generateLog(itLogData);
	}

	// �����ݱ��浽�ڴ�鵱�У����Ա������ͻȻ�ĺ������в�����־û����¼�������Ķ�ʧ������һ�����԰��ϴ�������־����ȡ����
	//Helper->write2SharedFileMap(m_strSharedMap.c_str(), strLogData.c_str(), strLogData.size());
	_unLock();	
}

void AutoLogEx::init()
{
}

void AutoLogEx::deleteLogFile(string strDir, int days)
{
	setDir(strDir);
	m_nSaveDays = days;
	autoClearLog();
}

void AutoLogEx::autoClearLog()
{
	vector<CHelper::tagFileInfo> vecFiles;
	Helper->getDirFiles(m_strDir, vecFiles, "log");

	ULONG uLen = 0;
	for (vector<CHelper::tagFileInfo>::iterator iter = vecFiles.begin();
		iter != vecFiles.end(); ++iter)
	{
		uLen += iter->uLen;
	}

	uLen += (int)Helper->getFileSize(m_strLogFileName.c_str());

	// û�г����������ݳ��ȣ������Դ���
	if (uLen > MAX_TOTOL_LOGFILESIZE)
	{
		// ����ɾ���ĸ���
		int nDeleteCount = (uLen - MAX_TOTOL_LOGFILESIZE) / MAX_SINGLE_LOGFILESIZE;		// ����
		nDeleteCount = nDeleteCount + ((uLen - MAX_TOTOL_LOGFILESIZE) % MAX_SINGLE_LOGFILESIZE) != 0 ? 1 : 0; // ����

		int nIndex = 0;
		for (vector<CHelper::tagFileInfo>::iterator iter = vecFiles.begin();
			iter != vecFiles.end(), nIndex++ < nDeleteCount;)
		{
			string strFileName = iter->strFileName;
			iter = vecFiles.erase(iter);
			Helper->deleteFile(strFileName.c_str());
		}
	}
	// û�г���ʱ��ģ������Դ���
	else if (vecFiles.size() > m_nSaveDays)
	{
		// ����ɾ���ĸ���
		int nDeleteCount = vecFiles.size() - m_nSaveDays;

		int nIndex = 0;
		for (vector<CHelper::tagFileInfo>::iterator iter = vecFiles.begin();
			iter != vecFiles.end(), nIndex++ < nDeleteCount;)
		{
			string strFileName = iter->strFileName;
			iter = vecFiles.erase(iter);
			Helper->deleteFile(strFileName.c_str());
		}
	}

	return;
}

std::string AutoLogEx::generateLog(_tagLogData& _data)
{
	string strType;
	switch (_data.nLevel)
	{
	case ENUM_LOG_LEVEL_INFO:
		strType = "INFO";
		break;
	case ENUM_LOG_LEVEL_WARNING:
		strType = "WARNING";
		break;
	case ENUM_LOG_LEVEL_ERROR:
		strType = "ERROR";
		break;
	default:
		break;
	}

	string strCurTime = _data.strTime.empty() ? Helper->getCurrentTimeString() : _data.strTime;
	string strData = Helper->format("[%s] %s [%s_line%d] %s\n",
		strCurTime.c_str(),
		strType.c_str(),
		_data.strFunc.c_str(),
		_data.nLine,
		_data.strLog.c_str());

	return strData;
}
