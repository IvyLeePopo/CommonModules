#include "Helper.h"
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "Version.lib ") 
#include <iostream>
#include <windows.h>
#include <algorithm>
#include <time.h>
#include <io.h>
#include <fstream>

#include "Jpeg2Bmp.h"


CHelper::CHelper(void)
{
}

CHelper::~CHelper(void)
{
}

std::string CHelper::format(const char* pFmt, ...)
{
	if (NULL == pFmt)
		return "";

#define FORMAT_MSG_BUFFER_SIZE (90000)
	char szBuffer[FORMAT_MSG_BUFFER_SIZE + 1] = { 0 };
	va_list args;
	va_start(args, pFmt);
	vsnprintf(szBuffer, FORMAT_MSG_BUFFER_SIZE, pFmt, args);
	va_end(args);

	std::string strRet  = szBuffer;
	return strRet;
}

string CHelper::i2a(int nValue)
{
	string strResult = format("%d", nValue);
	return strResult;
}

string CHelper::f2a(float fValue, int nPoint)
{
	string strParam = "%0." + format("%df", nPoint);
	string strResult = format(strParam.c_str(), nPoint);
	return strResult;
}

__int64 CHelper::getCurrentTimeStamp(bool bMSecond)
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
		0};

	time_t _tTime = mktime(&curTime);
	return bMSecond ? _tTime * 1000 + sysTime.wMilliseconds : _tTime;
}

string CHelper::getCurrentTimeString(bool bMSecond, bool bUseSeparator)
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


string CHelper::getCurrentTimeString_DayofWeek(bool bMSecond)
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

int CHelper::getCurrentDate()
{
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	string strTime = format("%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	return atoi(strTime.c_str());
}

bool CHelper::isEqualTime(int nHour, int nMin, int nSec)
{
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	return sysTime.wHour == nHour && sysTime.wMinute == nMin && sysTime.wSecond == nSec;
}

string CHelper::md5(const char* pszBuf)
{
	//if(NULL == pszBuf)
		return "";

	//MD5 _md5(pszBuf);
	//string strBuf = _md5.toStr();
	//return strBuf;
}

string CHelper::getProcessPath()
{
	char szBuf[2048] = { 0 };
	GetModuleFileNameA(NULL, szBuf, sizeof(szBuf));
	
	string strPath(szBuf);
	strPath = strPath.substr(0, strPath.find_last_of("\\"));
	return strPath;
}

bool CHelper::isExistFile(string strFile, bool bIsDir /*= false*/)
{
	if (strFile.empty())
		return false;

	return !bIsDir ? FALSE != PathFileExistsA(strFile.c_str()) : FALSE != PathIsDirectoryA(strFile.c_str());
}

// map数据进行排序，从小到大
bool cmp(const pair<string, CHelper::tagFileInfo>& a,
		 const pair<string, CHelper::tagFileInfo>& b)
{
	return a.second.nCreateTime < b.second.nCreateTime;
}

bool CHelper::getDirFiles(string strDir, vector<CHelper::tagFileInfo>& vecFiles,string strSuffixName)
{
	if (strDir.empty() || !isExistFile(strDir, true))
	{
		return false;
	}

	vecFiles.clear();
	string strPath = strDir + "*.*" + strSuffixName;

	// 文件信息
	struct _finddata_t _fileInfo;
	
	// 文件句柄
	intptr_t hFile = _findfirst(strPath.c_str(), &_fileInfo);
	if (-1 != hFile)
	{
		do
		{
			string strFileName = strDir + string(_fileInfo.name);
			if (isExistFile(strFileName))
			{
				tagFileInfo _tagInfo;
				_tagInfo.nCreateTime = static_cast<int>(_fileInfo.time_create);
				_tagInfo.nModifyTime = static_cast<int>(_fileInfo.time_write);
				_tagInfo.uLen = static_cast<int>(_fileInfo.size);
				_tagInfo.strFileName = strFileName;
				vecFiles.push_back(_tagInfo);
			}
		} while (0 == _findnext(hFile, &_fileInfo));
	}
	return true;
}

bool CHelper::setFileContent(string strFile, string& strContent, string strMode)
{
	FILE* pFile = fopen(strFile.c_str(), strMode.c_str());
	if (nullptr == pFile)
	{
		return false;
	}

	fwrite(strContent.c_str(), strContent.size(), 1, pFile);
	fflush(pFile);
	fclose(pFile);
	pFile = nullptr;
	return true;
}

bool CHelper::getFileContent(string strFile, string& strContent, bool bHex /*= false*/)
{
	//CFile file;
	//UINT uFlag = bHex ? CFile::typeBinary : CFile::modeReadWrite;
	//if (FALSE == file.Open(CString(strFile.c_str()), uFlag))
	//	return false;

	//int nSize = (int)file.GetLength();
	//char* pszBuf = new char[nSize + 1];
	//memset(pszBuf, 0, nSize + 1);

	//file.Read(pszBuf, nSize);
	//file.Close();

	//strContent = pszBuf;
	//delete [] pszBuf;
	//pszBuf = NULL;

	return true;
}

bool CHelper::setFileBinary(string strFile, const char* pszData, int nSize)
{
	//CFile file;
	//if(FALSE == file.Open(CString(strFile.c_str()), CFile::modeCreate | CFile::typeBinary | CFile::modeNoTruncate | CFile::modeReadWrite))
	//	return false;

	//file.Write(pszData,(UINT)nSize);
	//file.Close();
	return true;
}

#include <fstream>
bool CHelper::getFileBinary(string strFile, char** pszData, int& nSize)
{
	if (nullptr != *pszData)
	{
		delete[] * pszData;
		*pszData = nullptr;
		return false;
	}

	HANDLE pFile = CreateFileA(strFile.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,        //打开已存在的文件 
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pFile);
		return false;
	}

	nSize = GetFileSize(pFile, NULL);          //得到文件的大小
	*pszData = new char[nSize];
	memset(*pszData, 0, nSize);	
	char* pTmpBuf = *pszData;

	DWORD dwBytesRead = 0;
	DWORD dwBytesToRead = nSize;
	do{ //循环读文件，确保读出完整的文件    

		ReadFile(pFile, pTmpBuf, dwBytesToRead, &dwBytesRead, NULL);
		if (dwBytesRead == 0)
			break;

		dwBytesToRead -= dwBytesRead;
		pTmpBuf += dwBytesRead;
	} while (dwBytesToRead > 0);

	CloseHandle(pFile);
	return true;
}

bool CHelper::createNewFileOrDir(string strFile, bool bIsDir /*= false*/)
{
	if (isExistFile(strFile, bIsDir))
		return true;

	if(!bIsDir)
	{
		return INVALID_HANDLE_VALUE != CreateFileA(strFile.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			0);
	}

	return TRUE == CreateDirectoryA(strFile.c_str(), NULL);
}

string CHelper::generate32UID()
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

string CHelper::encryptSha256(const char* pszData, int nSize)
{
	if (NULL == pszData)
		return "";

	//uint8_t szResult[64] = {0};
	//sha256(pszData, nSize, szResult);

	string strResult;// = format("") szResult;
	return strResult;
}

#define AVal(x) Base64ValTab[x]
#define BVal(x) Base64IdxTab[x]
const BYTE Base64ValTab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const BYTE Base64IdxTab[128] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
	255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255
};
int CHelper::encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput)
{
	if (nullptr == pszInput || nullptr == pszOutput)
	{
		return 0;
	}

	int nIndex = 0;
	int nLoop = 0;
	int nRemain = 0;
	int nDstLen = 0;
	int nSrcLen = nLength;

	nLoop = nSrcLen / 3;
	nRemain = nSrcLen % 3;

	// also can encode native char one by one as decode method
	// but because all of char in native string is to be encoded so encode 3-chars one time is easier.

	for (nIndex = 0; nIndex < nLoop; nIndex++)
	{
		BYTE a1 = (pszInput[nIndex * 3] >> 2);
		BYTE a2 = (((pszInput[nIndex * 3] & 0x03) << 4) | (pszInput[nIndex * 3 + 1] >> 4));
		BYTE a3 = (((pszInput[nIndex * 3 + 1] & 0x0F) << 2) | ((pszInput[nIndex * 3 + 2] & 0xC0) >> 6));
		BYTE a4 = (pszInput[nIndex * 3 + 2] & 0x3F);

		pszOutput[nIndex * 4] = AVal(a1);
		pszOutput[nIndex * 4 + 1] = AVal(a2);
		pszOutput[nIndex * 4 + 2] = AVal(a3);
		pszOutput[nIndex * 4 + 3] = AVal(a4);
	}

	nDstLen = nIndex * 4;
	if (nRemain == 1)
	{
		// should pad two equal sign
		nIndex = nSrcLen - 1;
		BYTE a1 = (pszInput[nIndex] >> 2);
		BYTE a2 = ((pszInput[nIndex] & 0x03) << 4);

		pszOutput[nDstLen++] = AVal(a1);
		pszOutput[nDstLen++] = AVal(a2);
		pszOutput[nDstLen++] = '=';
		pszOutput[nDstLen++] = '=';
		pszOutput[nDstLen] = 0x00;
	}
	else if (nRemain == 2)
	{
		// should pad one equal sign
		nIndex = nSrcLen - 2;
		BYTE a1 = (pszInput[nIndex] >> 2);
		BYTE a2 = (((pszInput[nIndex] & 0x03) << 4) | (pszInput[nIndex + 1] >> 4));
		BYTE a3 = ((pszInput[nIndex + 1] & 0x0F) << 2);

		pszOutput[nDstLen++] = AVal(a1);
		pszOutput[nDstLen++] = AVal(a2);
		pszOutput[nDstLen++] = AVal(a3);
		pszOutput[nDstLen++] = '=';
		pszOutput[nDstLen] = 0x00;
	}
	else
	{
		// just division by 3
		pszOutput[nDstLen] = 0x00;
	}

	return nDstLen;
}

int CHelper::decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput)
{
	if (nullptr == pszInput || nullptr == pszOutput)
	{
		return 0;
	}

	int nCnt = 0;
	int nSrcLen = nLength;

	char * pChar = pszOutput;
	for (int nIndex = 0; nIndex < nSrcLen; nIndex++)
	{
		if (pszInput[nIndex] > 127)
			continue;

		if (pszInput[nIndex] == '=')
			return (int)(pChar - pszOutput + 1);

		BYTE byteValue = BVal(pszInput[nIndex]);
		if (byteValue == 255) continue;

		switch (nCnt)
		{
		case 0:
			{
				*pChar = byteValue << 2;
				nCnt++;
			}
			break;
		case 1:
			{
				*pChar++ |= byteValue >> 4;
				*pChar = byteValue << 4;
				nCnt++;
			}
			break;
		case 2:
			{
				*pChar++ |= byteValue >> 2;
				*pChar = byteValue << 6;
				nCnt++;
			}
			break;
		case 3:
			{
				*pChar++ |= byteValue;
				nCnt = 0;
			}
			break;
		default:
			break;
		}
	}

	*pChar = 0x00;
	return (int)(pChar - pszOutput);
}

string CHelper::getModulePath()
{
	char szDir[2048] = {0};
	::GetModuleFileNameA(NULL, szDir, sizeof(szDir));

	string strResult = szDir;
	strResult = strResult.substr(0, strResult.find_last_of("\\"));
	return strResult;
}

unsigned short CHelper::crc16(const char* pszData, int nSize)
{
	if (nullptr == pszData || 0 == nSize)
		return 0;

	BYTE crc16Hi = 0xFF;
	BYTE crc16Lo = 0xFF;
	BYTE cH = 0xA0;
	BYTE cL = 0x01;
	BYTE saveHi = 0;
	BYTE saveLo = 0;

	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		crc16Lo = crc16Lo ^ pszData[nIndex];
		for (int nFlag = 0; nFlag < 8; nFlag++)
		{
			saveHi = crc16Hi;
			saveLo = crc16Lo;
			crc16Hi = crc16Hi / 2; //高位右移一位
			crc16Lo = crc16Lo / 2; //低位右移一位
			if ((saveHi & 1) == 1)
				crc16Lo |= 0x80;

			if ((saveLo & 1) == 1)
			{
				crc16Hi = crc16Hi ^ cH;
				crc16Lo = crc16Lo ^ cL;
			}
		}
	}

	return (WORD)(crc16Hi * 0x100 + crc16Lo);
}

unsigned short CHelper::crc16(const std::vector<unsigned char>::const_iterator& it_first,
							  const std::vector<unsigned char>::const_iterator& it_second)
{
	BYTE crc16Hi = 0xFF;
	BYTE crc16Lo = 0xFF;
	BYTE cH = 0xA0;
	BYTE cL = 0x01;
	BYTE saveHi = 0;
	BYTE saveLo = 0;

	for (std::vector<unsigned char>::const_iterator it = it_first; it != it_second; ++it)
	{
		crc16Lo = crc16Lo ^ (*it);
		for (int nFlag = 0; nFlag < 8; nFlag++)
		{
			saveHi = crc16Hi;
			saveLo = crc16Lo;
			crc16Hi = crc16Hi / 2; //高位右移一位
			crc16Lo = crc16Lo / 2; //低位右移一位
			if ((saveHi & 1) == 1)
				crc16Lo |= 0x80;

			if ((saveLo & 1) == 1)
			{
				crc16Hi = crc16Hi ^ cH;
				crc16Lo = crc16Lo ^ cL;
			}
		}
	}

	return (WORD)(crc16Hi * 0x100 + crc16Lo);
}

bool CHelper::isVaildIp(const char* pszIp)
{
	if (nullptr == pszIp)
		return false;

	int nDots = 0;								// 字符.的个数
	int nSetions = 0;							// ip每一部分总和（0-255） 

	if (nullptr == pszIp || '.' == *pszIp)		// 排除输入参数为NULL, 或者一个字符为'.'的字符串 
	{ 
		return false;
	}   

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

string CHelper::splitImageFromBase64(const string& strData, string& strType)
{
	/*
		strData的格式：
		"data:image/png;base64,iVBORw0KGgoAAAANSUh......="
	*/
	if (strData.empty())
		return "";

	int nStartPos = (int)strData.find("/") + 1;
	int nEndPos = (int)strData.find(";");

	strType = strData.substr(nStartPos, nEndPos - nStartPos);
	return strData.substr(strData.find("base64,") + 7, strData.size());
}

void CHelper::splitString(string& strSource, string strReps, vector<string>& vecString)
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

string CHelper::utf8ToGbk(const char *pszSrc)
{
	if (nullptr == pszSrc)
		return "";

	int nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, NULL, 0);
	wchar_t* pwszGBK = new wchar_t[nLen + 1];
	memset(pwszGBK, 0, nLen * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, pwszGBK, nLen);
	nLen = WideCharToMultiByte(CP_ACP, 0, pwszGBK, -1, NULL, 0, NULL, NULL);

	char* pszGBK = new char[nLen + 1];
	memset(pszGBK, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP, 0, pwszGBK, -1, pszGBK, nLen, NULL, NULL);
	string strTemp(pszGBK);

	delete [] pwszGBK;
	pwszGBK = nullptr;

	delete [] pszGBK;
	pszGBK = nullptr;
	return strTemp;
}

std::string CHelper::gbk2Utf8(string& strData)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, strData.c_str(), -1, NULL, 0);

	WCHAR *pWStr1 = new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP, 0, strData.c_str(), -1, pWStr1, nLen);
	nLen = WideCharToMultiByte(CP_UTF8, 0, pWStr1, -1, NULL, 0, NULL, NULL);


	char *pStr2 = new char[nLen];
	WideCharToMultiByte(CP_UTF8, 0, pWStr1, -1, pStr2, nLen, NULL, NULL);
	
	string strOutUtf8 = pStr2;

	delete [] pWStr1;
	pWStr1 = NULL;

	delete[] pStr2;
	pStr2 = NULL;

	return strOutUtf8;
}

string CHelper::replaceString(const char* pszSource, const char* pszOldKey, const char* pszNewKey)
{
	if(nullptr == pszSource || nullptr == pszOldKey || nullptr == pszNewKey)
		return "";

	string strSource(pszSource);
	string strNewKey(pszNewKey);
	string strOldKey(pszOldKey);

	while(true)   
	{     
		string::size_type pos(0);     
		if((pos = strSource.find(strOldKey)) != string::npos)     
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

std::string CHelper::eraseString(const char* pszSource, int nStartPos, int nLength)
{
	if (nullptr == pszSource)
		return "";

	if (0 == nLength)
	{
		return pszSource;
	}

	string strSource = pszSource;
	if ((int)strSource.size() < nStartPos)
	{
		return "";
	}

	nLength = (int)strSource.size() < (nStartPos + nLength) ? (int)strSource.size() - nStartPos : nLength;
	string strResult = strSource.substr(0, nStartPos) + strSource.substr(nStartPos + nLength, strSource.size() - (nStartPos + nLength));
	return strResult;
}

bool CHelper::write2SharedFileMap(const char* pszDataKey, const char* pszData, int nSize)
{
	if (nullptr == pszDataKey || nullptr == pszData)
		return false;

	HANDLE pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	if (nullptr == pHandle)
	{
		pHandle = CreateFileMappingA((HANDLE)(int)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 2048, pszDataKey);
	}

	if (nullptr == pHandle)
		return false;

	LPVOID pVoid = MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	memcpy(pVoid, pszData, nSize);
	FlushViewOfFile(pVoid, nSize + 1);

	return true;
}

bool CHelper::readSharedFileMap(const char* pszDataKey, string& strData)
{
	if (nullptr == pszDataKey)
		return false;

	HANDLE pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	if (nullptr == pHandle)
		return false;

	//CloseHandle(pHandle);

	//pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	//if (nullptr == pHandle)
	//	return false;

	LPTSTR lpMapAddr = (LPTSTR)MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == lpMapAddr)
		return false;

	strData = "";
	strData = (const char*)lpMapAddr;
	
	memset(lpMapAddr, 0, strData.size());
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(pHandle);
	pHandle = NULL;
	return !strData.empty();
}

bool CHelper::clearSharedFileMap(const char* pszDataKey)
{
	if (nullptr == pszDataKey)
		return false;

	HANDLE pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	if (nullptr == pHandle)
		return false;

	LPTSTR lpMapAddr = (LPTSTR)MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == lpMapAddr)
		return false;

	string strData = (const char*)lpMapAddr;
	memset(lpMapAddr, 0, strData.size());
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(pHandle);
	pHandle = NULL;
	return true;
}

bool CHelper::splitIpAndPort(const char* pszData,
		string& strIp,
		int& nPort,
		string& strUser,
		string& strPasswd)
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

string getVersionKey(string strKey,
	char* pszVersionInfo,
	WORD m_wLanguage,
	WORD m_wCcodePage,
	UINT& uLen)
{
	string strSubBlock = Helper->format("\\StringFileInfo\\%04x%04x\\%s",
		m_wLanguage,
		m_wCcodePage,
		strKey.c_str());

	string strResult;
	LPVOID* pResult = NULL;
	if (::VerQueryValueA(static_cast<LPVOID>(pszVersionInfo),
		strSubBlock.c_str(),
		reinterpret_cast<LPVOID*>(&pResult), &uLen))
	{
		strResult = (const char*)pResult;
	}

	return strResult;
}

bool CHelper::getDllExeVersion(const string& strName, string& strCompany, string& strVersion, string& strDescrip)
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

	delete [] pVersionInfo;
	pVersionInfo = NULL;

	return true;
}

int CHelper::getRandValue()
{
	int nMin = 1000;
	int nMax = 1000000;
	int nTime = (int)getCurrentTimeStamp()/1000000;
	int nValue = nMin + rand() % (nMax - nMin + 1);
	return nValue + nTime;
}

std::string CHelper::hex2String(const char* pszBuf, int nLen)
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

long CHelper::getFileSize(const char* pszFileName)
{
	if (nullptr == pszFileName)
	{
		return 0L;
	}

	long lSize = 0;
	HANDLE pFile = CreateFileA(pszFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,        //打开已存在的文件 
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pFile);
		return 0;
	}

	lSize = GetFileSize(pFile, NULL);          //得到文件的大小
	CloseHandle(pFile);
	return lSize;
}

bool CHelper::deleteFile(const char* pszFileName)
{
	if (nullptr == pszFileName)
	{
		return false;
	}

	if (!isExistFile(pszFileName))
	{
		return true;
	}

	
	return 0 != DeleteFileA(pszFileName);
}

bool CHelper::jpeg2Bmp(const char* pszSrc, int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength)
{
	return Bitmap->Jpeg2Bmp(pszSrc, nSrcLength, pszDst, nDstLength);
}

bool CHelper::Bmp2Binarization(const string& bmpFilePath, unsigned char** pszDst, unsigned int& nDstLength)
{
	return Bitmap->Bmp2Binarization(bmpFilePath, pszDst, nDstLength);
}
