/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: TypeHelper.cpp
* 摘   要:  用于类型转换
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-12
* 修改记录: 
*/
#include "stdafx.h"
#include "TypeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//改写自 scanmachine
std::string Bin2Hex(const unsigned char* bin_buf,size_t buf_len)
{
    CString strHex;
    CString strTemp;
    if(NULL != bin_buf)
    {
        for ( DWORD i = 0; i < buf_len; ++i )
        {
            strTemp.Format(_T("%02X"), bin_buf[i] );
            strHex += strTemp;
        }
    }
    return strHex.GetBuffer(0);
}
//改写自 scanmachine
std::string Bin2Hex(const std::vector<unsigned char> src)
{
    CString strHex;
    CString strTemp;
    if(!src.empty())
    {
        for ( DWORD i = 0; i < src.size(); ++i )
        {
            strTemp.Format(_T("%02X "), src[i] );
            strHex += strTemp;
        }
    }
    return strHex.GetBuffer(0);
}
std::string Bin2Hex(const std::list<unsigned char> src)
{
    std::vector<unsigned char> tmp;
    tmp.insert(tmp.end(),src.begin(),src.end());
    return Bin2Hex(tmp);
}
//改写来自scanmachine
bool Hex2Bin(const char* hex_buf,char* bin_buf,size_t hex_len)
{
    if(hex_len%2)
        return false;

    CString strTemp(hex_buf); 
    strTemp.MakeUpper();

    USES_CONVERSION;
    for(int i=0;i<strTemp.GetLength()/2;i++)
    {
        BYTE a = (BYTE)(strTemp.GetAt(i*2)>='A')?(BYTE)(strTemp.GetAt(i*2)-'A'+10):(BYTE)(strTemp.GetAt(i*2)-'0');
        BYTE b = (BYTE)(strTemp.GetAt(i*2+1)>='A')?(BYTE)(strTemp.GetAt(i*2+1)-'A'+10):(BYTE)(strTemp.GetAt(i*2+1)-'0');
        bin_buf[i] = (BYTE)(a * 0x10 + b);
    }
    return true;
}

std::string Int2Str(int info)
{
    char numStr[30]={0};
    itoa(info,numStr,10);
    return numStr;
}

int Str2Int(const std::string& info)
{
    return atoi(info.c_str());
}

//////////////////////////////////////////////////////////////
// 函数名称:Str2Datetime
//
// 功能描述:转换日期字符串到整型，只支持固定格式的内容
//
// 输入参数:const std::string& info,
// 输出参数:time_t 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
time_t Str2Datetime(const std::string& info)
{
	return Str2ChinaTime(info);
    tm tm_;  
    int year, month, day, hour, minute,second;
    year = month = day = hour = minute = second = 0;
    sscanf(info.c_str(),"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);  
    tm_.tm_year  = year-1900;  
    tm_.tm_mon   = month-1;  
    tm_.tm_mday  = day;  
    tm_.tm_hour  = hour;  
    tm_.tm_min   = minute;  
    tm_.tm_sec   = second;  
    tm_.tm_isdst = 0;  
  
    time_t t_ = mktime(&tm_); //已经减了8个时区  
    return t_;
}

std::string Datetime2Str(time_t dt)
{
	if(dt<=0)
	{
		return "1970-01-01 00:00:00";
	}
	return ChinaTime2Str(dt);
    struct tm tm1;
    char result[50]={0};
    tm1 = *localtime(&dt);
    sprintf(result,"%04d-%02d-%02d %02d:%02d:%02d.1",tm1.tm_year+1900,tm1.tm_mon+1,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);

    return std::string(result);
}

std::string _Datetime2Str(time_t dt)
{
	if(dt<=0)
	{
		return "19700101000000";
	}
	return _ChinaTime2Str(dt);
}



//////////////////////////////////////////////////////////////
// 函数名称:String2IP
//
// 功能描述:转换字符串格式(xxx.xxx.xxx.xxx)的IP到int
//
// 输入参数:const std::string& info,
// 输出参数:unsigned int 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
unsigned int String2IP(const std::string& info)
{
    unsigned int result = 0;
    unsigned int first,second,third,four;
    first = second = third = four = 0;
    sscanf(info.c_str(),"%d.%d.%d.%d",&first,&second,&third,&four);
	unsigned char cFirst,cSecond,cThird,cFour;
	cFirst = first & 0x000000FF;
	cSecond = second & 0x000000FF;
	cThird = third & 0x000000FF;
	cFour = four & 0x000000FF;
    result = (cFirst<<24)|(cSecond<<16)|(cThird<<8)|cFour;
    return result;
}

std::vector< std::vector<unsigned char>> SplitCharArray(const unsigned char* src,int len,unsigned char split_ch)
{
	std::vector< std::vector<unsigned char>> result;
	if(len <=0)
	{
		return result;
	}
	size_t ch_it = 0;
	while(ch_it < len)
	{
		std::vector<unsigned char> one_data;
		while( ch_it++<len)
		{
			if(src[ch_it]==split_ch)
			{
				break;
			}
			one_data.push_back(src[ch_it]);
		}
		result.push_back(one_data);

	}
	return result;
}


//将一个时间字符串，转换为unix时间戳。字符串中的时间，认为是使用了系统时区产生的
time_t Str2ChinaTime(const std::string& time_str)
{
	SYSTEMTIME china_t;
	memset(&china_t, 0, sizeof(SYSTEMTIME));
	sscanf(time_str.c_str(), "%d-%d-%d %d:%d:%d",&china_t.wYear, &china_t.wMonth, &china_t.wDay, &china_t.wHour, &china_t.wMinute, &china_t.wSecond);

	struct tm china_tm = { china_t.wSecond,china_t.wMinute,china_t.wHour,china_t.wDay,china_t.wMonth - 1,china_t.wYear - 1900,china_t.wDayOfWeek,0,0 };
	return mktime(&china_tm);
	//return _mkgmtime(&china_tm)-8*60*60;
}


//将参数t，按照unix 时间戳，进行计算
std::string ChinaTime2Str(time_t t)
{
	struct tm test_tm =  *localtime(&t);  
	char time_chs[100]={0};
	sprintf(time_chs,"%04d-%02d-%02d %02d:%02d:%02d", test_tm.tm_year + 1900, test_tm.tm_mon + 1, test_tm.tm_mday, test_tm.tm_hour, test_tm.tm_min, test_tm.tm_sec);
	return std::string(time_chs);
}

std::string _ChinaTime2Str(time_t t)
{
	struct tm test_tm =  *localtime(&t);  
	char time_chs[100]={0};
	sprintf(time_chs,"%04d%02d%02d%02d%02d%02d", test_tm.tm_year + 1900, test_tm.tm_mon + 1, test_tm.tm_mday, test_tm.tm_hour, test_tm.tm_min, test_tm.tm_sec);
	return std::string(time_chs);
}

// unix 时间戳
time_t ChinaTime()
{
	return time(0);
}
std::string ChinaTimeStr()
{
	return ChinaTime2Str(ChinaTime());
}
std::string ChinaTimeStrWithMS()
{
    return ChinaTimeStr()+".000";
}

INT BASE64_Encode(const BYTE* inputBuffer, INT inputCount, TCHAR* outputBuffer)
{
    INT  i  = 0x00;
    BYTE b0 = 0x00;
    BYTE b1 = 0x00;
    BYTE b2 = 0x00;

    CHAR* DATA_BIN2ASCII = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if ((inputBuffer == NULL) || (inputCount < 0))
    {
        return -1;  // 参数错误
    }

    if (outputBuffer != NULL)
    {
        for (i = inputCount; i > 0; i -= 3)
        {
            if (i >= 3)
            {   // 将3字节数据转换成4个ASCII字符
                b0 = *inputBuffer++;
                b1 = *inputBuffer++;
                b2 = *inputBuffer++;

                *outputBuffer++ = DATA_BIN2ASCII[b0 >> 2];
                *outputBuffer++ = DATA_BIN2ASCII[((b0 << 4) | (b1 >> 4)) & 0x3F];
                *outputBuffer++ = DATA_BIN2ASCII[((b1 << 2) | (b2 >> 6)) & 0x3F];
                *outputBuffer++ = DATA_BIN2ASCII[b2 & 0x3F];
            }
            else
            {
                b0 = *inputBuffer++;
                if (i == 2)b1 = *inputBuffer++; else b1 = 0;

                *outputBuffer++ = DATA_BIN2ASCII[b0 >> 2];
                *outputBuffer++ = DATA_BIN2ASCII[((b0 << 4) | (b1 >> 4)) & 0x3F];
                *outputBuffer++ = (i == 1) ? TEXT('=') : DATA_BIN2ASCII[(b1 << 2) & 0x3F];
                *outputBuffer++ = TEXT('=');
            }
        } // End for i

        *outputBuffer++ = TEXT('/0');  // 添加字符串结束标记
    }

    return ((inputCount + 2) / 3) * 4;  // 返回有效字符个数
}
std::string HexToBase64(IN const std::string& strSrcHex_org )
{
	CString strSrcHex;
	CString strDesBase64;
	strSrcHex.Format(_T("%s"),strSrcHex_org.c_str());
    BOOL bReturn = FALSE;
	if (0 == strSrcHex.GetLength())
		return "";

	if (0  != (strSrcHex.GetLength() % 2))
		return "";

	unsigned int uSize = strSrcHex.GetLength();
	BYTE *pTemp = new BYTE[uSize / 2 + 1];
	memset( pTemp, 0, (uSize / 2 + 1)*sizeof(BYTE) );
	for (unsigned int i = 0; i < uSize; i += 2)
	{
		unsigned char tempC = 0, bcd = 0;
		tempC = strSrcHex.GetAt(i);
		if (tempC >= 'a' && tempC <= 'f') tempC = tempC - 'a' + 'A';
		bcd = ( tempC >= 'A' ? tempC - 'A' + 0x0a : tempC - '0') << 4;
		tempC = strSrcHex.GetAt(i + 1);
		if (tempC >= 'a' && tempC <= 'f') tempC = tempC - 'a' + 'A';
		bcd += tempC >= 'A' ? tempC - 'A' + 0x0a : tempC - '0';
		pTemp[i / 2] = bcd;
	}
	char *pOutBuffer = new char[uSize];
	int  iOutDataLen = 0x00;
	memset(pOutBuffer, 0, uSize);

	iOutDataLen = BASE64_Encode(pTemp, uSize / 2, pOutBuffer);

	pOutBuffer[iOutDataLen] = 0;

	strDesBase64.Format("%s", pOutBuffer);
	std::string result_64 = pOutBuffer;
	//清理资源
	if (NULL != pOutBuffer)
		delete []pOutBuffer;
	pOutBuffer = NULL;

	if (NULL != pTemp)
		delete []pTemp;
	pTemp = NULL;

	
    return result_64;
}


std::string StringConvertByCodePage( const std::string& strConvertData_org, IN const int& iSourceCodepage, IN const int& iTargetCodepage)
{
	CString strConvertData;
	strConvertData.Format(_T("%s"),strConvertData_org.c_str());
	CString strReturn;
	CString strRealConvertData(strConvertData);
	int len = strRealConvertData.GetLength();
	int unicodeLen = MultiByteToWideChar(iSourceCodepage, 0, strRealConvertData, -1, NULL, 0);

	wchar_t *pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	if (NULL != pUnicode)
	{
		memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
		MultiByteToWideChar(iSourceCodepage, 0, strConvertData, -1, (LPWSTR)pUnicode, unicodeLen);

		BYTE *pTargetData = NULL ;
		int targetLen = WideCharToMultiByte(iTargetCodepage, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);

		pTargetData = new BYTE[targetLen + 1];
		if (NULL != pTargetData)
		{
			memset(pTargetData, 0, targetLen + 1);

			WideCharToMultiByte(iTargetCodepage, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, targetLen, NULL, NULL);

			strReturn.Format(_T("%s"), pTargetData);

			delete []pTargetData;
		}
		delete []pUnicode;
	}
	std::string code_str = (LPCSTR)strReturn;
	return code_str ;
}


time_t FileTimeToTime_t(const FILETIME &ft)  
{  
    ULARGE_INTEGER ui;  
    ui.LowPart = ft.dwLowDateTime;  
    ui.HighPart = ft.dwHighDateTime;  
    return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);  
}  
  
__int64 GetDiffMilliSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2)  
{  
    FILETIME fTime1 = { 0, 0 };  
    FILETIME fTime2 = { 0, 0 };  
    SystemTimeToFileTime(&t1, &fTime1);  
    SystemTimeToFileTime(&t2, &fTime2);  
  
    time_t tt1 = FileTimeToTime_t(fTime1);  
    time_t tt2 = FileTimeToTime_t(fTime2); 

    __int64 diff_milli_secs =  static_cast<__int64>((tt2 - tt1)*1000 + (t2.wMilliseconds - t1.wMilliseconds));
  
    return diff_milli_secs;  
}  




int GetFileModifyTime(const std::string& file_path, SYSTEMTIME& sys_time)
{
    HANDLE hFile = CreateFile(file_path.c_str(),
                              GENERIC_READ, //必须有GENERIC_READ属性才能得到时间
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        FILETIME fCreateTime, fAccessTime, fWriteTime;
        if(!GetFileTime(hFile, &fCreateTime, &fAccessTime, &fWriteTime))
		{
			CloseHandle(hFile);
			return -1;	
		}//获取文件时间
		CloseHandle(hFile);
        if(!FileTimeToSystemTime(&fWriteTime, &sys_time))
		{
			return -1;
		}//将文件时间转换为本地系统时间
		
        return 0;
    }
    return -1;
}

std::string GetGuid()
{
    char buffer[100] = { 0 };  
    GUID guid;  
    
    if (S_OK!=CoCreateGuid(&guid))  
    {  
        return "";
    }  
    _snprintf(buffer, sizeof(buffer),  
        "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X",  
        guid.Data1, guid.Data2, guid.Data3,  
        guid.Data4[0], guid.Data4[1], guid.Data4[2],  
        guid.Data4[3], guid.Data4[4], guid.Data4[5],  
        guid.Data4[6], guid.Data4[7]);  
    return std::string(buffer);
}