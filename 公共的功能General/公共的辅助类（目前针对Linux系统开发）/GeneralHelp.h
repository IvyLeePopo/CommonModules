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

    struct tagFileInfo
    {
        long long uLen;         // 文件大小
        int nCreateTime;        // 文件创建时间
        int nModifyTime;        // 文件最后修改时间
        string strFileName;     // 文件绝对路径
    };

public:
    // app run path 获取app运行路径
    string getAppDirPath();

    // 时间转换函数，从 int -> "yyyy-MM-dd hh:mm:ss:zzz" ,支持毫秒级别的操作     bIsPlain = true:int->"yyyyMMddhhmmss"
    string transMsTimeFromInt(int64_t nTime, bool bIsMs = false, bool bIsPlain = false);

    // 时间转换函数，从"yyyyMMddhhmmss"转换成时间戳
    long long transTimeStampfromString(string strTime, string strFormat = "yyyyMMddhhmmss", bool bIsMs = false);

    // 获取当前时间，"yyyy-MM-dd hh:mm:ss"或者"yyyy-MM-dd hh:mm:ss:zzz"  bIsPlain = true:"yyyyMMddhhmmss"或"yyyyMMddhhmmsszzz"
    string getCurTimeString(bool bIsMs = false, bool bIsPlain = false);

    // get current time stamp(second) 获取当前的时间戳，bIsMsSecond = false 取到秒,否则毫秒
    long long getCurTimeStamp(bool bIsMs = false);

    // 通过时间戳获取唯一的32位Hash码
    string get32Guid();

    // 获取MsgId,示例: 04afe44d-09f5-47e4-9826-a0e2e62596c2
    string getMsgId();

    // string的format函数,注意pFmt的长度不要超过2048000，不然会die……
    string format(const char* pFmt, ...);

    // 判断字符是否为合法的json格式
    bool isJsonIllegal(const char *pszJson);

    // 执行shell命令
    bool excuteCmd(string strCmd);

    // 启动一个进程
    void runProcess(const char* pszName, const char* pszCmd, bool bKill = true);

    // 判断一个进程是否存在
    bool isExistProcess(const char *pszProcessName);

    // 将ip替换成域名
    void replaceIp2Domain(string& strUrl,
                          string strDomain,
                          bool bIsReplaceHttp2Https = false);

    // 替换字符串
    string replaceString(const char* pszSource, const char* pszOldKey, const char* pszNewKey);

    // 分割字符
    void splitString(string& strSource, string strReps, vector<string>& vecString);

    // base64 加密
    string string2Base64(const char* pszData, unsigned int nLen);

    // base 64加密
    int encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

    // base64 解密
	int decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

    // base64 to string
    string base64ToString(string strData);

    // write to file
    bool write2File(string strFilePath, string& strContent);

    // read file
    bool readFile(string strFilePath, string& strContent);

    // 判断文件或者目录是否存在,strFileName最好是全路径的文件
    bool isExistFile(string strFileName);

    // 创建目录
    bool createDir(string strDir);

    // 获取指定所有的指定文件
    bool getDirAllFiles(string strSuffix, string strDir, vector<tagFileInfo>& vecFiles);

    // 判断ip是否合法
	bool isVaildIp(const char* pszIp);

    // 日志记录
    void writeLog(int nType, string strLog, int nLine, const char* pszFuncName);

    // 将string以16进制的方式转换
    string string2Hex(const char* pszBuf, int nLen);

    // 将16进制转换成字符串
    string hex2String(const char* pszBuf, int nLen);

    // 去掉日期中的 '-'、':'、' '
    string getPlainDateTime(string strDateTime);

	// crc校验
	uint16_t crc16(uint8_t* puchMsg, uint32_t usDataLen);

    // int值转16进制string
    string int2HexString(int nData);

    // 获取当前时间，"yyyy-MM-dd hh:mm.ss"或者"yyyy-MM-dd hh:mm:ss.zzz"
    string getAntennaCurTimeString(bool bIsMs);

    // gbk->utf8
    string G2U(string strGBK);

    //国标编码 转 utf8编码
    QString GBK_2_UTF8(const char* p);
    //utf8编码 转 国标编码
    QString UTF8_2_GBK(const char* p);

    QString GetCurrentTime();
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
