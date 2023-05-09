#include "global.h"

#include "Windows.h"
#include "dpiscaled.h"

namespace global
{
	QString appUniqueGUID()
	{
		return gAppUniqueGUID;
	}

	qreal dpiScaled(qreal value)
	{
		static const qreal scale = qreal(defaultDpiX()) / 96.0;
		return value * scale;
	}

	qreal dpiDeScaled(qreal value)
	{
		static const qreal scale = dpiValue();
		return value / scale;
	}

	qreal dpiValue()
	{
		return qreal(defaultDpiX()) / 96.0;
	}

	QString getResDPIRelatePath()
	{
		static QString dpiPath = QString();
		if (dpiPath.isEmpty())
		{
			if (dpiScaled(1.0) > 1.99)
				dpiPath = QString("2x");
			else
				dpiPath = QString("1x");
		}

		return dpiPath;
	}

	QString getResDPIFullPath(const QString &subPath)
	{
		return QString(":/%0/%1").arg(getResDPIRelatePath()).arg(subPath);
	}

	QPixmap getResImgDPIFullPath(const QString &imgName, const QString &ext)
	{
		return QPixmap(getResDPIFullPath(imgName + ext));
	}

	QString getHostModulePath()
	{
		static QString s_hostModulePath;
		if (s_hostModulePath.isEmpty())
		{
			s_hostModulePath = QFileInfo(getThisModulePath()).absoluteDir().absolutePath();
		}
		return s_hostModulePath;
	}

	QString getThisModulePath()
	{
		static QString s_thisModulePath;
		if (s_thisModulePath.isEmpty())
		{
			WCHAR szfileNameBuffer[MAX_PATH] = { 0 };
			DWORD ret = ::GetModuleFileNameW(NULL, szfileNameBuffer, MAX_PATH);
			if (ret != 0)
			{
				s_thisModulePath = QString::fromUtf16((char16_t*)szfileNameBuffer, ret);
			}
		}
		return s_thisModulePath;
	}

	QString getAppRoamingPath()
	{
		return getAppdataPath();
	}

	int getChannelID()
	{
		static int clientChannelId = 0;
		if (clientChannelId == 0)
		{
			QString cfgPath = PathUtils::jointPath(getHostModulePath(), "config.ini");
			QSettings setting(cfgPath, QSettings::IniFormat);
			clientChannelId = setting.value("channel/channelId", 1000).toInt();
		}

		return clientChannelId;
	}

	QSize getWindowSizeFromScreen()
	{
		QRect rcScreen = QApplication::desktop()->screenGeometry();
		static QSize size;
		if (size.isEmpty())
		{
			if (rcScreen.width() < _S(1400))
			{
				//¼æÈÝµÍ·ÖÆÁ
				size.setWidth(_S(1200));
				size.setHeight(_S(650));
				//>³¬Ð¡·Ö±æÂÊÆÁÄ»
				if (rcScreen.width() < 1200)
					size = rcScreen.size();
			}
			else
			{
				size.setWidth(_S(1420));
				size.setHeight(_S(800));
			}
			if (size.width() > rcScreen.width())
			{
				size.setWidth(rcScreen.width());
			}
			if (size.height() > rcScreen.height())
			{
				size.setHeight(rcScreen.height());
			}
		}
		return size;
	}

	QString userAgent()
	{
		return QString(" Ideation/%0 ").arg(genericfunc::getExeVersion());
	}

	QString getWindowTitle()
	{
		return "4D SHOETECH";
	}

	QString getVersion()
	{
		static QString ver = QString("");
		if (ver.isEmpty())
		{
			bool b = false;
			wchar_t verBuf[MAX_PATH] = { 0 };
			do
			{
				std::wstring filepath = getThisModuleName();
				if (filepath.empty())
					break;
				DWORD dwFileVerSize = 0;
				unsigned nValueBlockSize = 0;
				VS_FIXEDFILEINFO* lpValueBlock = NULL;
				dwFileVerSize = ::GetFileVersionInfoSizeW(filepath.c_str(), NULL);
				if (dwFileVerSize == 0)
					break;
				std::vector<char> vecVerBuffer(dwFileVerSize, 0);
				if (!::GetFileVersionInfoW(filepath.c_str(), 0, dwFileVerSize, vecVerBuffer.data()))
					break;
				if (!::VerQueryValueW(vecVerBuffer.data(), L"\\", (void**)(&lpValueBlock), &nValueBlockSize)
					|| !lpValueBlock)
					break;
				swprintf_s(verBuf, MAX_PATH, L"%lu.%lu.%lu.%lu",
					HIWORD(lpValueBlock->dwFileVersionMS),
					LOWORD(lpValueBlock->dwFileVersionMS),
					HIWORD(lpValueBlock->dwFileVersionLS),
					LOWORD(lpValueBlock->dwFileVersionLS));
				b = true;
			} while (0);
			if (b)
			{
				ver = QString::fromWCharArray(verBuf);
			}
			else
			{
				LOG(WARNING) << "*ASSERT failure";
				ver = "IllegalVersion";
			}
		}
		return ver;
	}

	QString getFileVersion(const std::wstring& filepath)
	{
		QString ver = QString("");
		if (ver.isEmpty())
		{
			bool b = false;
			wchar_t verBuf[MAX_PATH] = { 0 };
			do
			{
				if (filepath.empty())
					break;
				DWORD dwFileVerSize = 0;
				unsigned nValueBlockSize = 0;
				VS_FIXEDFILEINFO* lpValueBlock = NULL;
				dwFileVerSize = ::GetFileVersionInfoSizeW(filepath.c_str(), NULL);
				if (dwFileVerSize == 0)
					break;
				std::vector<char> vecVerBuffer(dwFileVerSize, 0);
				if (!::GetFileVersionInfoW(filepath.c_str(), 0, dwFileVerSize, vecVerBuffer.data()))
					break;
				if (!::VerQueryValueW(vecVerBuffer.data(), L"\\", (void**)(&lpValueBlock), &nValueBlockSize)
					|| !lpValueBlock)
					break;
				swprintf_s(verBuf, MAX_PATH, L"%lu.%lu.%lu.%lu",
					HIWORD(lpValueBlock->dwFileVersionMS),
					LOWORD(lpValueBlock->dwFileVersionMS),
					HIWORD(lpValueBlock->dwFileVersionLS),
					LOWORD(lpValueBlock->dwFileVersionLS));
				b = true;
			} while (0);
			if (b)
			{
				ver = QString::fromWCharArray(verBuf);
			}
			else
			{
				ver = QString();
			}
		}
		return ver;
	}

	int compareVersion(const QString& src, const QString& dest)
	{
		if (src.compare(dest) == 0)
			return 0;
		QStringList srcList = src.split(".");
		QStringList destList = dest.split(".");
		int count = srcList.count() > destList.count() ?
			destList.count() : srcList.count();
		for (int i = 0; i < count; i++)
		{
			bool bSrcOk = false, bDestOk = false;
			int num1 = srcList[i].toInt(&bSrcOk);
			int num2 = destList[i].toInt(&bDestOk);
			if (!bSrcOk || !bDestOk)
				LOG(WARNING) << "*ASSERT failure";
			if (!bSrcOk)
				num1 = 0;
			if (!bDestOk)
				num2 = 0;
			if (num1 == num2)
				continue;
			if (num1 > num2)
				return 1;
			else
				return -1;
		}
		return 0;
	}

	QString getMainUpdateUrl()
	{
		return gMainUpdateUrl;
	}

	void startNewProcess(const QString& exePath, const QString& command)
	{
#ifdef Q_OS_WIN
		WCHAR szExe[MAX_PATH] = {};
		wcscpy_s(szExe, MAX_PATH, exePath.toStdWString().c_str());
		WCHAR szCmd[1024] = {};
		wcscpy_s(szCmd, 1024, command.toStdWString().c_str());
		::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		SHELLEXECUTEINFOW shellInfo = {};
		//memset(&shellInfo, 0, sizeof(SHELLEXECUTEINFOW));
		shellInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
		shellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellInfo.hwnd = NULL;
		shellInfo.lpVerb = NULL;
		shellInfo.lpFile = szExe;
		shellInfo.lpParameters = szCmd;
		shellInfo.lpDirectory = NULL;
		shellInfo.nShow = SW_HIDE;
		shellInfo.hInstApp = NULL;
		ShellExecuteExW(&shellInfo);
		DWORD dwCode = 0;
		GetExitCodeProcess(shellInfo.hProcess, &dwCode);
		CloseHandle(shellInfo.hProcess);
		::CoUninitialize();
#endif
		return;
	}

	bool deleteDir(const QString& path)
	{
		if (path.isEmpty())
		{
			return false;
		}
		QDir dir(path);
		if (!dir.exists())
		{
			return true;
		}
		dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //filter
		QFileInfoList fileList = dir.entryInfoList();
		foreach(QFileInfo file, fileList)
		{
			if (file.isFile())
			{
				file.dir().remove(file.fileName());
			}
			else
			{
				deleteDir(file.absoluteFilePath());
			}
		}
		return dir.rmpath(dir.absolutePath());
	}

	bool isInstallIllustrator(QString& illustratorPath)
	{
		auto getAIPath = [=](QString& aiPath, const QString& regPath)
		{
			QSettings settings(regPath, QSettings::NativeFormat);
			QString clsid = settings.value(QString("."), "").toString();
			if (!clsid.isEmpty())
			{
				QString regpath = QString("HKEY_CLASSES_ROOT\\CLSID\\%0\\LocalServer32").arg(clsid);
				QSettings setting(regpath, QSettings::NativeFormat);
				QString path = setting.value(".", "").toString();
				if (QFileInfo(path).isFile())
				{
					aiPath = path;
					return true;
				}
			}
			aiPath = QString();
			return false;
		};

		bool ret = getAIPath(illustratorPath, "HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Adobe.Illustrator\\CLSID");
		if (!ret)
		{
			ret = getAIPath(illustratorPath, "HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Adobe.Illustrator.24\\CLSID");
			if (!ret)
			{
				illustratorPath = QString();
				return false;
			}
		}
		return true;
	}

	void setForgroundWindow(QWidget *widget, bool isForce)
	{
		if (!widget)
			return;
#ifdef Q_OS_WIN
		HWND hwnd = (HWND)widget->winId();
		HWND hCurrWnd = ::GetForegroundWindow();
		DWORD dwThisPID = ::GetCurrentProcessId();
		DWORD dwThisTID = ::GetCurrentThreadId();
		DWORD dwCurrPID = 0;
		DWORD dwCurrTID = ::GetWindowThreadProcessId(hCurrWnd, &dwCurrPID);
		if (dwThisPID != dwCurrPID)
		{
			::AttachThreadInput(dwThisTID, dwCurrTID, TRUE);
			::BringWindowToTop(hwnd);
			::SetForegroundWindow(hwnd);
			::SwitchToThisWindow(hwnd, TRUE);
			::AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
			::SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		else {
			if (isForce) {
				::SetForegroundWindow(hwnd);
			}
		}
#endif
	}

	QString makeGmtNow()
	{
		const char *ListWeekDay[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
		const char *ListMonth[] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
		time_t now;
		time(&now);
		const struct tm *t = gmtime(&now);
		char buf[100] = { 0 };
		sprintf_s(buf,
			"%s %s %02d %02d:%02d:%02d %04d", ListWeekDay[t->tm_wday], ListMonth[t->tm_mon], t->tm_mday,
			t->tm_hour + 8, t->tm_min, t->tm_sec, (t->tm_year < 1900) ? t->tm_year + 1900 : t->tm_year);
		QString strRet = buf;
		return strRet;
	}

	bool checkShowRedPoint()
	{
		if (SDUpdateManager::getInstance()->getUpdateStatus() == SDUpdateManager::NeedRemindUpdate)
		{
			SDSetting setting;
			uint utime = setting.readSetting(sdconfig::remindpoint).toUInt();
			if (utime == 0)
			{
				return true;
			}
			else if (QDateTime::fromTime_t(utime).isValid())
			{
				QDateTime time = QDateTime::fromTime_t(utime);
				QDateTime curTime = QDateTime::currentDateTime();
				qint64 second = time.secsTo(curTime);
				if (second >= 60 * 60 * 24 * 3)
				{
					return true;
				}
			}
		}
		return false;
	}

	void updateRemindPoint()
	{
		SDSetting setting;
		setting.writeSetting(sdconfig::remindpoint, QDateTime::currentDateTime().toTime_t());
	}

	QString getPkgTempDir()
	{
		QString  appdataPath = getAppdataPath();
		QDir dir(appdataPath);
		if (!dir.exists(appdataPath))
		{
			dir.mkdir(appdataPath);
		}
		//ÅÐ¶Ï%appdata%/sdshoepkgtmpÊÇ·ñ´æÔÚ
		appdataPath = dir.absoluteFilePath(SD_APPDATA_PKG_CACHE);
		dir.setPath(appdataPath);
		if (!dir.exists(appdataPath))
		{
			dir.mkdir(appdataPath);
		}
		return appdataPath;
	}

	QString getCachePkgPath()
	{
		return getPkgTempDir() + QDir::separator() + "ideationtemp.exe";
	}

	QString getApplicationDir()
	{
		QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\Ideation\\", QSettings::NativeFormat);
		QString program = setting.value("installpath", "").toString();
		return program;
	}

	QString getFunctionZipPkgPath()
	{
		QString path = QString();
		SDSetting setting;
		QString rootPath = setting.value(sdconfig::installpath, "").toString();
		if (rootPath.isEmpty())
			return QString();
		QDir dir(rootPath);
		dir.cd("Installer");
		QString funcPath = dir.path();
		funcPath = funcPath + QDir::separator() + "function.7z";
		qDebug() << funcPath;
		path = funcPath;
		return path;
	}

	void setLazyAutoUpdateFlag(const QString& params)
	{
		SDSetting setting;
		setting.writeSetting(sdconfig::autoUpdateFlags, params);
	}

	void removeLayAutoUpdateFlags()
	{
		SDSetting setting;
		setting.removeSetting(sdconfig::autoUpdateFlags);
	}

	QString getLazyAutoUpdate()
	{
		SDSetting setting;
		QString value = setting.readSetting(sdconfig::autoUpdateFlags).toString();
		return value;
	}

	bool checkExistLazyUpdateFlags()
	{
		SDSetting setting;
		bool isExist = setting.contains(sdconfig::autoUpdateFlags);
		return isExist;
	}

	int getLazyUpdateDeltaTime()
	{
		SDSetting setting;
		bool isExist = setting.contains(sdconfig::lazytimeSwitch);
		if (isExist)
			return 60 * 2;
		//> 2 days
		return 60 * 60 * 24 * 2;
	}
}
