/*
 * @Description: 字符串_打印log_时间等的转换
 * @Author: Ivy
 * @Date: 2022-05-12 09:34:43
 * @LastEditTime: 2022-05-12 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

 // 1、字符串的拼接
 // string的format函数,注意pFmt的长度不要超过2048000，不然会die……
std::string format(const char* pFmt, ...)
{
	if (NULL == pFmt)
		return "";

#define FORMAT_MSG_BUFFER_SIZE (90000)//注意看这个大小
	char szBuffer[FORMAT_MSG_BUFFER_SIZE + 1] = { 0 };
	va_list args;
	va_start(args, pFmt);
	vsnprintf(szBuffer, FORMAT_MSG_BUFFER_SIZE, pFmt, args);
	va_end(args);

	std::string strRet = szBuffer;
	return strRet;
}

namespace string
{
	// 1、数字转字符串
	string i2a(int nValue);

	// 2、浮点转字符串 nPoint 指的是小数点后几位
	string f2a(float fValue, int nPoint = 4);

	// 3、将16进制转换成字符串
	string hex2String_1(const char* pszBuf, int nLen);
	string hex2String_2(const char* pszBuf, int nLen);

	// 4、将string以16进制的方式转换
	string string2Hex(const char* pszBuf, int nLen);
	string string2Hex_qt(const char* pszBuf, int nLen);

	// 5、int值转16进制string
	string int2HexString_qt(int nData);

	// 6、替换字符串
	string replaceString(const char* pszSource, const char* pszOldKey, const char* pszNewKey);

	// 7、分割字符
	void splitString(string& strSource, string strReps, vector<string>& vecString);


}

namespace time
{
	// 1、获取时间戳,精确到秒，int类型
	__int64 getCurrentTimeStamp(bool bMSecond = false);

	// 时间转换函数，从"yyyyMMddhhmmss"转换成时间戳
	long long transTimeStampfromString_qt(string strTime, string strFormat = "yyyyMMddhhmmss", bool bIsMs = false);

	// 2、获取字符串的时间，精确到字符串，返回字符串如下：2022-09-03 10:09:13.958 \ 2022-09-03 10:09:13,   
	// 如bUseSeparator为false，则格式为20220903100913958 \ 20220903100913
	string getCurrentTimeString(bool bMSecond = false, bool bUseSeparator = true);

	// 时间转换函数，从 int -> "yyyy-MM-dd hh:mm:ss:zzz" ,支持毫秒级别的操作     bIsPlain = true:int->"yyyyMMddhhmmss"
	string transMsTimeFromInt_qt(int64_t nTime, bool bIsMs = false, bool bIsPlain = false);

	// 3、get current time stamp(second) 获取当前的时间戳，bIsMsSecond = false 取到秒,否则毫秒
	long long getCurTimeStamp_qt(bool bIsMs = false);

	// 4、获取当前时间，"yyyy-MM-dd hh:mm.ss"或者"yyyy-MM-dd hh:mm:ss.zzz"
	string getCurTimeString_qt(bool bIsMs);

	// 5、获取字符串的时间，精确到字符串，返回字符串如下：2022-09-03星期四 10:09:13.958 \ 2022-09-03星期四 10:09:13
	string getCurrentTimeString_DayofWeek(bool bMSecond = true);

	// 6、获取当前的日期，直接返回int类型，返回的格式如下：20221102
	int getCurrentDate();
	QString getCurrentDate_qt();

	// 7、是否为匹配的时间,默认0点0时0分
	bool isEqualTime(int nHour = 0, int nMin = 0, int nSec = 0);

	// 8、测试时间间隔：还有一种方法是，用时间戳来加减
	void calTimeGap();
	void calTimeGap_TimeStamp();

	// 9、通过时间戳获取唯一的32位Hash码
	string get32Guid();

	// 10、将时间戳获取唯一的32位Hash码，组装04afe44d-09f5-47e4-9826-a0e2e62596c2
	string getMsgId();

	// 11、去掉日期中的 '-'、':'、' '
	string getPlainDateTime(string strDateTime);
}


