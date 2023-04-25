#include "dpiscaled.h"

#include "pch.h"
#include "global.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QScreen>
#include <QSettings>
#include <QtGlobal>
#include <functional>
#include "common/sdsettingconfig.h"
#include "general/genericdef.h"
#include "general/genericfunc.h"
#include "general/pathutils.h"
#include "sdassert.h"
#include "sdsetting.h"
#include "updatemanager/sdupdatemanager.h"

#ifdef Q_OS_WIN
#include <psapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <windef.h>
#endif

#define SD_APPDATA_FOLDER       L"ideation"
#define SD_APPDATA_PKG_CACHE    "sdshoepkgtmp"

namespace
{
	const QString gMainUpdateUrl = "http://www.4dshoetech.com/";
	const QString gAppUniqueGUID = "E916D3C9-B61B-4B71-865B-C4098E05B303";

	int defaultDpiX()
	{
		if (QCoreApplication::instance()->testAttribute(Qt::AA_Use96Dpi))
			return 96;
		if (const QScreen *screen = QGuiApplication::primaryScreen())
			return qRound(screen->logicalDotsPerInchX());
		return 100;
	}

	LPCWSTR getThisModuleName()
	{
		static HMODULE s_hThisModule = NULL;
		static WCHAR s_szModuleName[_MAX_PATH] = { 0 };
		if (!s_szModuleName[0])
		{
			BOOL br = GetModuleHandleExW(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				(LPCWSTR)(void *)&getThisModuleName,
				&s_hThisModule);
			if (br)
			{
				GetModuleFileNameW(s_hThisModule, s_szModuleName, _MAX_PATH);
			}
			for (WCHAR* psz = s_szModuleName; *psz; ++psz)
			{
				if (*psz == L'\\')
					*psz = L'/';
			}
		}
		return s_szModuleName;
	}

	QString getAppdataPath()
	{
		WCHAR szPath[MAX_PATH] = {};
		::SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, FALSE);
		wcscat_s(szPath, MAX_PATH, L"\\");
		wcscat_s(szPath, MAX_PATH, SD_APPDATA_FOLDER);
		return QDir::toNativeSeparators(QString::fromWCharArray(szPath));
	}
}