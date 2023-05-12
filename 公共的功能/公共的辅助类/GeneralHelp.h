/*
 * @Description: 帮助类，将公用的帮助方法放到此处
 * @Author: Ivy
 * demo :
 * string strAppPath = Helper::getAppDirPath()
 * @Date: 2022-04-09 09:34:43
 * @LastEditTime: 2023-03-23 13:39:52
 * @LastEditors: XTZJ-2022OQEHLZ
 */
#pragma once
#include <string>
#include <vector>
#include "Singleton.h"

using namespace std;

enum EnumLogType
{
    EnumLogType_Info = 0,
    EnumLogType_Err = 1,
    EnumLogType_Warning = 2, 
};

class GeneralHelp : public Singleton<GeneralHelp>
{
    friend class Singleton<Singleton>;

public:
    GeneralHelp();
    virtual ~GeneralHelp();

public:
	// 1、字符串的拼接
	// string的format函数,注意pFmt的长度不要超过2048000，不然会die……
	std::string format(const char* pFmt, ...);

    // 日志记录
    void writeLog(int nType, string strLog, int nLine, const char* pszFuncName);

};

#define Helper GeneralHelp::getInstance()
#define WriteLog(type, log, line, name) Helper->writeLog(type, log, line, name);
#define WriteLogInfo(log) WriteLog(EnumLogType_Info, log, __LINE__, __FUNCTION__);
#define WriteLogErr(log) WriteLog(EnumLogType_Err, log, __LINE__, __FUNCTION__);
#define WriteLogWarning(log) WriteLog(EnumLogType_Warning, log, __LINE__, __FUNCTION__);

//json合法性检查
#define CHECKJSON(strData)\
    if(strData.empty())\
        return false;\
    Json::Value jsonRoot;\
    Json::Reader jsonReader;\
    if(!jsonReader.parse(strData,jsonRoot))\
    {\
        WriteLog(EnumLogType_Err, strData, __LINE__, __FUNCTION__);\
        return false;\
    }
