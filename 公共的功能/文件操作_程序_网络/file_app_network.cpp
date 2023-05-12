#include "file_app_network.h"

string app_network::getProcessPath()
{
	char szBuf[2048] = { 0 };
	GetModuleFileNameA(NULL, szBuf, sizeof(szBuf));
	//::GetModuleFileNameA(NULL, szDir, sizeof(szDir));

	string strPath(szBuf);
	strPath = strPath.substr(0, strPath.find_last_of("\\"));
	return strPath;
}

string app_network::getAppDirPath()
{
	return QCoreApplication::applicationDirPath().toStdString();
}

bool app_network::splitIpAndPort(const char* pszData, string& strIp, int& nPort, string& strUser, string& strPasswd)
{
	if (NULL == pszData)
	{
		return false;
	}

	vector<string> vecList;
	splitString(string(pszData), ",", vecList);

	strIp = vecList.size() > 0 ? vecList[0] : "";
	nPort = atoi(vecList.size() > 1 ? vecList[1].c_str() : "80");
	strUser = vecList.size() > 2 ? vecList[2] : "";
	strPasswd = vecList.size() > 3 ? vecList[3] : "";
	return true;
}

bool app_network::getDllExeVersion(const string& strName, string& strCompany, string& strVersion, string& strDescrip)
{
	if (!isExistFile(strName))
	{
		return false;
	}

	UINT uVersionInfoSize = ::GetFileVersionInfoSizeA(strName.c_str(), 0);

	char* pVersionInfo = new char[uVersionInfoSize];
	memset(pVersionInfo, 0, uVersionInfoSize);

	struct VersionLanguage {
		WORD m_wLanguage;
		WORD m_wCcodePage;
	};

	if (::GetFileVersionInfoA(strName.c_str(), 0, uVersionInfoSize, pVersionInfo) != 0)
	{
		UINT uLen = 0;
		VersionLanguage* pVersionLanguage = NULL;
		if (::VerQueryValueA(pVersionInfo, "\\VarFileInfo\\Translation",
			reinterpret_cast<LPVOID*>(&pVersionLanguage),
			&uLen))
		{
			strCompany = getVersionKey("CompanyName",
				pVersionInfo,
				pVersionLanguage->m_wLanguage,
				pVersionLanguage->m_wCcodePage,
				uLen);

			strVersion = getVersionKey("ProductVersion",
				pVersionInfo,
				pVersionLanguage->m_wLanguage,
				pVersionLanguage->m_wCcodePage,
				uLen);

			strDescrip = getVersionKey("FileDescription",
				pVersionInfo,
				pVersionLanguage->m_wLanguage,
				pVersionLanguage->m_wCcodePage,
				uLen);
		}
	}

	delete[] pVersionInfo;
	pVersionInfo = NULL;

	return true;
}

bool app_network::isVaildIp(const char * pszIp)
{
	if (nullptr == pszIp || '.' == *pszIp)		// 排除输入参数为NULL, 或者一个字符为'.'的字符串 
	{
		return false;
	}

	int nDots = 0;								// 字符.的个数
	int nSetions = 0;							// ip每一部分总和（0-255） 

	while (*pszIp)
	{
		if ('.' == *pszIp)
		{
			nDots++;
			if (nSetions >= 0 && nSetions <= 255)		// 检查ip是否合法
			{
				nSetions = 0;
				pszIp++;
				continue;
			}
			return false;
		}
		else if (*pszIp >= '0' && *pszIp <= '9')		// 判断是不是数字
		{

			nSetions = nSetions * 10 + (*pszIp - '0');	// 求每一段总和
		}
		else
			return false;

		pszIp++;
	}

	/*判断IP最后一段是否合法*/
	if (nSetions >= 0 && nSetions <= 255)
	{
		if (nDots == 3)
		{
			return true;
		}
	}

	return false;
}

void app_network::replaceIp2Domain(string & strUrl, string strDomain, bool bIsReplaceHttp2Https)
{
	if (-1 == strUrl.find_first_of("://"))
		return;

	int nStartPos = strUrl.find_first_of("://") + 3;
	string _strUrl = strUrl.substr(nStartPos, strUrl.size() - nStartPos);
	if (_strUrl.find_first_of(":") < _strUrl.find_first_of("/"))     // 说明会有端口
	{
		strUrl.replace(nStartPos, _strUrl.find_first_of(":"), strDomain.c_str());
	}
	else            // 如果先找到了 “/” ，那就说明没有端口
	{
		int nFindPos = _strUrl.find_first_of("/") + nStartPos;
		strUrl.replace(nStartPos, nFindPos, strDomain.c_str());
	}

	if (bIsReplaceHttp2Https)
	{
		if (-1 == strUrl.find("https") && 0 == strUrl.find("http"))
		{
			strUrl.replace(0, 4, "https");
		}
	}
}

#include <guiddef.h>
string app_network::generate32UID()
{
	GUID guid;
	string strUID;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		strUID = format("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
		return strUID;
	}

	// 防止上面的方式获取不到，将会使用MD5的方式通过随机数+时间的方式创建一个32位的随机数To-do

	return strUID;
}


bool InitWndMutex(QRect& winRect)
{
	{
		//兼容旧版本 2020-5-25 之前
		QString sAppDirPath = QCoreApplication::applicationDirPath(); //软件路径
		CreateMutex(NULL, FALSE, sAppDirPath.toStdWString().c_str());//互斥锁
	}
	const QString sCheckKey = QCryptographicHash::hash(QCoreApplication::applicationDirPath().toLocal8Bit(), QCryptographicHash::Md5).toHex();
	const QString sMutexName = QString("[%1][%2]").arg("CT4_Client").arg(sCheckKey); //检查更新会用到这个值，不要修改
	const QString SAppName = QApplication::applicationName();
	HANDLE hMutex = CreateMutex(NULL, FALSE, sMutexName.toStdWString().c_str());
	if (hMutex && (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		CloseHandle(hMutex);
		HWND hwnd = FindWindow(L"Qt5QWindowIcon", SAppName.toStdWString().c_str());
		if (nullptr == hwnd)
			hwnd = FindWindow(L"Qt5QWindowToolSaveBits", SAppName.toStdWString().c_str());
		if (nullptr != hwnd)
		{
			QWindow* widget = QWindow::fromWinId(WId(hwnd));
			if (widget != nullptr)
			{
				winRect = widget->geometry();
			}
			SetForegroundWindow(hwnd); //激活窗口
		}
		return false;
	}
	return true;
}

//是否重复启动  true 重复  false不重复
bool app_network::IsRepeatStart(const QRect & rect)
{
	QRect mRect = rect;
	if (InitWndMutex(mRect) == false)
	{
		QSettings setting(PATH_GLOBAL_CLOUD_SETTING, QSettings::IniFormat);
		QString themeType = setting.value("Setting/m_themeTopic", "E_THEMEWHITE").toString();
		BLMsgBox* msgBox;
		if (themeType == "E_THEMEWHITE") {
			msgBox = new BLMsgBox(nullptr, E_BoxType::E_WhiteBox);
		}
		else
		{
			msgBox = new BLMsgBox(nullptr, E_BoxType::E_BlackBox);
		}
		msgBox->move(mRect.x() + mRect.width() / 2 - msgBox->width() / 2, mRect.y() + mRect.height() / 2 - msgBox->height() / 2);
		msgBox->exec();
		return true;
	}
	return false;
}

bool linux::excuteCmd(string strCmd)
{
	return QProcess::startDetached(QString(strCmd.c_str()));;
}

void linux::runProcess(const char * pszName, const char * pszCmd, bool bKill)
{
	if (nullptr == pszName || nullptr == pszCmd)
		return;

	//  step 01 强制杀掉本地推流进程
	string strCmd = Helper->format("ps -ef | grep %s | grep -v grep | awk '{print $2}' | xargs kill -9", pszName);
	if (bKill)
		system(strCmd.c_str());

	// step 02 启动
	excuteCmd(pszCmd);
}

bool linux::isExistProcess(const char * pszProcessName)
{
	if (nullptr == pszProcessName)
		return false;

	bool bRet = false;

	FILE *fstream = nullptr;
	char buff[1024] = { 0 };

	//用空格，是去掉类似dah_main的噪声
	sprintf(buff, "ps -A | grep \" %s\"", pszProcessName);
	if (nullptr == (fstream = popen(buff, "r")))
		return false;

	QString strCurPid = QString::number(getpid());
	while (nullptr != fgets(buff, sizeof(buff), fstream))
	{
		if (strlen(buff) <= 0)
			break;

		QString strResult = buff;
		if (strResult.contains(strCurPid))
			continue;

		char* psHead = strstr(buff, pszProcessName);
		if (psHead == nullptr)
			continue;

		int pos = strlen(psHead) - 1;
		if (psHead[pos] == '\n')
			psHead[pos] = 0;

		if (!strcmp(psHead, pszProcessName))
		{
			bRet = true;
			break;
		}
	}

	pclose(fstream);

	return bRet;
}
