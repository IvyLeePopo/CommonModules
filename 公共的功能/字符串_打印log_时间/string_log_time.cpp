#include "string_log_time.h"
#include <windows.h>

namespace string
{
	string i2a(int nValue)
	{
		string strResult = format("%d", nValue);
		return strResult;
	}

	string f2a(float fValue, int nPoint)
	{
		string strParam = "%0." + format("%df", nPoint);
		string strResult = format(strParam.c_str(), nPoint);
		return strResult;
	}

	std::string hex2String_1(const char* pszBuf, int nLen)
	{
		if (nullptr == pszBuf || nLen <= 0)
			return "";

		bool ok;
		QString strData(pszBuf);
		string strHex;
		for (int i = 0; i < (nLen / 2); i++)
		{
			uint8_t nHex = (uint8_t)strData.mid(i * 2, 2).toInt(&ok, 16);
			strHex.push_back(nHex);
		}
		return strHex;
	}

	string hex2String_2(const char * pszBuf, int nLen)
	{
		if (nullptr == pszBuf || 0 == nLen)
		{
			return "";
		}

		string strResult;
		for (int nIndex = nLen; nIndex < nLen; nIndex++)
		{
			string strTemp = format("%02X ", pszBuf[nIndex]);
			strResult += strTemp;
		}
		return strResult;
	}

	string string2Hex(const char * pszBuf, int nLen)
	{
		if (nullptr == pszBuf || nLen <= 0)
			return "";

		string strData;
		for (int nIndex = 0; nIndex < nLen; nIndex++)
		{
			strData += format("%02x", 0xFF & pszBuf[nIndex]);
		}
		return strData;
	}

	string string2Hex_qt(const char * pszBuf, int nLen)
	{
		if (nullptr == pszBuf || 0 == nLen)
			return "";

		string strResult;

		QByteArray szData(pszBuf, nLen);
		szData = szData.toHex();
		strResult = szData.toStdString();
		return strResult;
	}

	string int2HexString_qt(int nData)
	{
		QString qstrHex = QString::number(nData, 16);
		return qstrHex.toStdString();
	}

	string replaceString(const char * pszSource, const char * pszOldKey, const char * pszNewKey)
	{
		if (nullptr == pszSource ||
			nullptr == pszNewKey ||
			nullptr == pszOldKey)
		{
			return "";
		}
		string strSource(pszSource);
		string strNewKey(pszNewKey);
		string strOldKey(pszOldKey);

		while (true)
		{
			string::size_type   pos(0);
			if ((pos = strSource.find(strOldKey)) != string::npos)
			{
				strSource.replace(pos, strOldKey.length(), pszNewKey);
			}
			else
			{
				break;
			}
		}

		return strSource;
	}

	void splitString(string & strSource, string strReps, vector<string>& vecString)
	{
		string str = strSource;
		for (; !str.empty();)
		{
			int nPos = (int)str.find(strReps.c_str());
			if (nPos == -1)
			{
				vecString.push_back(str);
				str = "";
				break;
			}

			string strValue = str.substr(0, nPos);
			if (strValue.empty() && !str.empty())
			{
				vecString.push_back(strValue);
				str = str.substr(nPos + strReps.size(), str.size());
				continue;
			}

			if (!str.empty())
			{
				vecString.push_back(strValue);
			}
			str = str.substr(nPos + strReps.size(), str.size());
		}
	}
}


namespace time
{
	//SYSTEMTIME Windows 操作系统中的一个结构体，用于表示日期和时间

	__int64 getCurrentTimeStamp(bool bMSecond)
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		struct tm curTime = { sysTime.wSecond,
			sysTime.wMinute,
			sysTime.wHour,
			sysTime.wDay,
			sysTime.wMonth - 1,
			sysTime.wYear - 1900,
			sysTime.wDayOfWeek,
			0,
			0 };

		time_t _tTime = mktime(&curTime);
		return bMSecond ? _tTime * 1000 + sysTime.wMilliseconds : _tTime;
	}

	long long transTimeStampfromString_qt(string strTime, string strFormat, bool bIsMs)
	{
		QDateTime dateTime = QDateTime::fromString(QString::fromStdString(strTime), QString::fromStdString(strFormat));
		return bIsMs ? dateTime.toMSecsSinceEpoch() : dateTime.toSecsSinceEpoch();
	}

	string getCurrentTimeString(bool bMSecond, bool bUseSeparator)
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		string strTime;
		if (bMSecond)
		{
			strTime = format(bUseSeparator ? "%04d-%02d-%02d %02d:%02d:%02d.%03d" : "%04d%02d%02d%02d%02d%02d%03d",
				sysTime.wYear,
				sysTime.wMonth,
				sysTime.wDay,
				sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond,
				sysTime.wMilliseconds);
		}
		else
		{
			string strFormat = bUseSeparator ? "%04d-%02d-%02d %02d:%02d:%02d" : "%04d%02d%02d%02d%02d%02d";
			strTime = format(strFormat.c_str(),
				sysTime.wYear,
				sysTime.wMonth,
				sysTime.wDay,
				sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond);
		}

		return strTime;
	}

	#include <QDateTime>
	string transMsTimeFromInt_qt(int64_t nTime, bool bIsMs = false, bool bIsPlain = false)
	{
		QDateTime _time = bIsMs ? QDateTime::fromMSecsSinceEpoch(nTime) : QDateTime::fromSecsSinceEpoch(nTime);

		if (!bIsPlain)
			return bIsMs ? _time.toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() : _time.toString("yyyy-MM-dd hh:mm:ss").toStdString();
		else
			return bIsMs ? _time.toString("yyyyMMddhhmmsszzz").toStdString() : _time.toString("yyyyMMddhhmmss").toStdString();

	}

	long long getCurTimeStamp_qt(bool bIsMs)
	{
		return bIsMs ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
	}

	string getCurTimeString_qt(bool bIsMs)
	{
		long long llDatetime = bIsMs ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
		QDateTime _time = bIsMs ? QDateTime::fromMSecsSinceEpoch(llDatetime) : QDateTime::fromSecsSinceEpoch(llDatetime);

		return bIsMs ? _time.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString() : _time.toString("yyyy-MM-dd hh:mm:ss").toStdString();
	}

	string getCurrentTimeString_DayofWeek(bool bMSecond)
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		string strTime;
		if (bMSecond)
		{
			strTime = format("%04d-%02d-%02d星期%d %02d:%02d:%02d.%03d",
				sysTime.wYear,
				sysTime.wMonth,
				sysTime.wDay,
				sysTime.wDayOfWeek,
				sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond,
				sysTime.wMilliseconds);
		}
		else
		{
			strTime = format("%04d-%02d-%02d星期%d %02d:%02d:%02d",
				sysTime.wYear,
				sysTime.wMonth,
				sysTime.wDay,
				sysTime.wDayOfWeek,
				sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond);
		}

		return strTime;
	}

	int getCurrentDate()
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		string strTime = format("%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
		return atoi(strTime.c_str());
	}

	QString getCurrentDate_qt()
	{
		QDateTime dateTime = QDateTime::currentDateTime();
		QString str = dateTime.toString("yyyy-MM-ddThh:mm:ss");
		return str;
	}


	bool isEqualTime(int nHour, int nMin, int nSec)
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);
		return sysTime.wHour == nHour && sysTime.wMinute == nMin && sysTime.wSecond == nSec;
	}


	#include<QTime >
	void calTimeGap()
	{
		QTime time;
		time.start();
		QTime::currentTime().msecsSinceStartOfDay() - (time.msecsSinceStartOfDay()) << QString("ms");
	}

	void calTimeGap_TimeStamp()
	{
		int nStartTime = (int)getCurrentTimeStamp();
		int nEndTime = nStartTime;
		//later
		nEndTime = (int)getCurrentTimeStamp();
		if (nEndTime - nStartTime > 15)//15秒钟
		{
			// todo
		}
	}

	string get32Guid()
	{
		QTime curTime = QTime::currentTime();
		QByteArray hash = QCryptographicHash::hash(
			QString(curTime.toString() + QUuid::createUuid().toString()).toLocal8Bit(),
			QCryptographicHash::Md5);
		return string(hash.toHex().data());
	}

	string getMsgId()
	{
		// 组装成 04afe44d-09f5-47e4-9826-a0e2e62596c2
		string strId = get32Guid();
		string strMsgId = strId.substr(0, 8) + "-" + strId.substr(9, 4) + "-" +
			strId.substr(13, 4) + "-" + strId.substr(17, 4) + "-" + strId.substr(21, 12);

		return  strMsgId;
	}

	string getPlainDateTime(string strDateTime)
	{
		if (strDateTime.empty())
			return "";

		string strResult;
		for (int i = 0; i < strDateTime.length(); i++)
		{
			if (strDateTime.at(i) == '-')
				continue;
			if (strDateTime.at(i) == ':')
				continue;
			if (strDateTime.at(i) == ' ')
				continue;
			strResult.push_back(strDateTime.at(i));
		}
		return strResult;
	}
}