#include "AutoLogEx.h"
#include "Helper.h"

#define MAX_ONEDATE_FILE_COUNT	1000					// 一天最多可以产生的日志文件数量
#define MAX_ALL_FILE_COUNT		10000					// 所有日志文件的数量上限
//#define TIMEOUT_CLEARLOG		259200					// 3600*24*30 默认30天的日志
#define TIMEOUT_CLEARLOG		5						// 默认10天的日志
#define MAX_TOTOL_LOGFILESIZE	100*1024*1024			// 所有日志允许的最大数据长度
#define MAX_SINGLE_LOGFILESIZE	10*1024*1024			// 每一个日志的数据长度界限

AutoLogEx::AutoLogEx()
{
	m_nLogNum = 0;												// 默认是0
	m_strDir = Helper->getModulePath() + "\\LogFile";			// 默认的目录文件
	m_nCurDate = Helper->getCurrentDate();
	m_nSaveDays = 60;
	init();
}

AutoLogEx::~AutoLogEx()
{
	// 正常的退出，需要主动清空共享内存
	//Helper->clearSharedFileMap(m_strSharedMap.c_str());
}

int AutoLogEx::vThreadWork()
{
	// 线程中自己去记录日志
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
				// 写入文件以及清除共享内存
				string strFileName = getFileName();
				Helper->setFileContent(strFileName, strLogData, "a+");
				//Helper->clearSharedFileMap(m_strSharedMap.c_str());
			}
			m_vecLogData.clear();
		}
		_unLock();
		autoClearLog();
		Sleep(1000 * 1);		// 1秒钟更新一次
	}
	return 0;
}

void AutoLogEx::startLog(string strDir, string strSharedMap)
{
	setDir(strDir);
	m_strSharedMap = strSharedMap;
	
	// 先去把共享内存里边的数据读取上来，并写入日志
	string strData;
	//Helper->readSharedFileMap(m_strSharedMap.c_str(), strData);
	if (!strData.empty())
	{
		strData = "***************读取异常退出的日志 - 开始***************\r\n" + strData + "***************读取异常退出的日志 - 结束***************\r\n";
		string strFileName = getFileName();
		Helper->setFileContent(strFileName, strData, "a+");
	}

	vStart();
	WriteLogInfo("程序启动，日志开始记录");
}

void AutoLogEx::setDir(string strDir)
{
	// 强制修改日志路径，默认是放在 "LogFile"的
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
		// 不同的日期，需要改变文件名的
		m_nLogNum = 0;
		m_nCurDate = Helper->getCurrentDate();
	}

	// 超出了一天最大的允许日志数量，要及时删除，虽然可能性很小，但是不要放过任何一个导致程序卡死的步骤
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
			if (lSize >= MAX_SINGLE_LOGFILESIZE)		// 允许每一个日志最大为10MB
			{
				// 找到文件，并且size大于最大尺寸，说明该日志文件已经满了
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
	// 写入日志
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

	// 将数据保存到内存块当中，可以避免程序突然的夯死，有部分日志没法记录而产生的丢失，重启一样可以把上次最后的日志给读取出来
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

	// 没有超过最大的数据长度，不予以处理
	if (uLen > MAX_TOTOL_LOGFILESIZE)
	{
		// 计算删除的个数
		int nDeleteCount = (uLen - MAX_TOTOL_LOGFILESIZE) / MAX_SINGLE_LOGFILESIZE;		// 求商
		nDeleteCount = nDeleteCount + ((uLen - MAX_TOTOL_LOGFILESIZE) % MAX_SINGLE_LOGFILESIZE) != 0 ? 1 : 0; // 求余

		int nIndex = 0;
		for (vector<CHelper::tagFileInfo>::iterator iter = vecFiles.begin();
			iter != vecFiles.end(), nIndex++ < nDeleteCount;)
		{
			string strFileName = iter->strFileName;
			iter = vecFiles.erase(iter);
			Helper->deleteFile(strFileName.c_str());
		}
	}
	// 没有超过时间的，不予以处理
	else if (vecFiles.size() > m_nSaveDays)
	{
		// 计算删除的个数
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
