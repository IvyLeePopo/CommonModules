#include "GeneralHelp.h"

#include <QProcess>
#include <QDateTime>
#include <QString>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>
#include <QTime>
#include <QDir>
#include <QFileInfoList>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QUuid>
#include <QTextCodec>
#include <QDebug>
#include <sys/stat.h> /*chmod*/

typedef unsigned char BYTE;
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

GeneralHelp::GeneralHelp()
{

}

GeneralHelp::~GeneralHelp()
{

}

string GeneralHelp::getAppDirPath()
{
    return QCoreApplication::applicationDirPath().toStdString();
}

string GeneralHelp::transMsTimeFromInt(int64_t nTime, bool bIsMs, bool bIsPlain)
{
    QDateTime _time = bIsMs ? QDateTime::fromMSecsSinceEpoch(nTime) : QDateTime::fromSecsSinceEpoch(nTime);

    if(!bIsPlain)
        return bIsMs ? _time.toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() : _time.toString("yyyy-MM-dd hh:mm:ss").toStdString();
    else
        return bIsMs ? _time.toString("yyyyMMddhhmmsszzz").toStdString() : _time.toString("yyyyMMddhhmmss").toStdString();
}

string GeneralHelp::get32Guid()
{
    QTime curTime = QTime::currentTime();
    QByteArray hash = QCryptographicHash::hash(
                QString(curTime.toString() + QUuid::createUuid().toString()).toLocal8Bit(),
                QCryptographicHash::Md5);
    return string(hash.toHex().data());
}

string GeneralHelp::getMsgId()
{
    // 组装成 04afe44d-09f5-47e4-9826-a0e2e62596c2
    string strId = get32Guid();
    string strMsgId = strId.substr(0, 8) + "-" + strId.substr(9, 4) + "-" +
            strId.substr(13, 4) + "-" + strId.substr(17, 4) +"-" + strId.substr(21, 12);

    return  strMsgId;
}

string GeneralHelp::format(const char *pFmt, ...)
{
#define FORMAT_MSG_BUFFER_SIZE (2048000)
    char szBuffer[FORMAT_MSG_BUFFER_SIZE + 1] = { 0 };
    va_list args;
    va_start(args, pFmt);
    vsnprintf(szBuffer, FORMAT_MSG_BUFFER_SIZE, pFmt, args);
    va_end(args);

    std::string strRet  = szBuffer;
    return strRet;
}

bool GeneralHelp::isJsonIllegal(const char *pszJson)
{
    std::stack<char> jsonstr;
    char startBuf = pszJson[0];
    char endBuf = '\0';
    bool bIsObject = false; //防止 {}{}的判断
    bool bIsArray = false;  //防止[][]的判断

    while (*pszJson != '\0')
    {
        endBuf = *pszJson;
        switch (*pszJson)
        {
        case '{':
            if (!bIsObject)
            {
                bIsObject = true;
            }
            else  if (jsonstr.empty())//对象重复入栈
            {
                return false;
            }
            jsonstr.push('{');
            break;
        case '"':
            if (jsonstr.empty() || jsonstr.top() != '"')
            {
                jsonstr.push(*pszJson);
            }
            else
            {
                jsonstr.pop();
            }
            break;
        case '[':
            if (!bIsArray)
            {
                bIsArray = true;
            }
            else  if (jsonstr.empty())//数组重复入栈
            {
                return false;
            }
            jsonstr.push('[');
            break;
        case ']':
            if (jsonstr.empty() || jsonstr.top() != '[')
            {
                return false;
            }
            else
            {
                jsonstr.pop();
            }
            break;
        case '}':
            if (jsonstr.empty() || jsonstr.top() != '{')
            {
                return false;
            }
            else
            {
                jsonstr.pop();
            }
            break;
        case '\\'://被转义的字符,跳过
            pszJson++;
            break;
        default:
            ;
        }
        pszJson++;
    }

    if (jsonstr.empty())
    {
        //确保是对象或者是数组,之外的都不算有效
        switch (startBuf)//确保首尾符号对应
        {
        case  '{':
        {

            if (endBuf = '}')
            {
                return true;
            }
            return false;
        }
        case '[':
        {
            if (endBuf = ']')
            {
                return true;
            }
            return false;
        }
        default:
            return false;
        }

        return true;
    }

    return false;
}

bool GeneralHelp::excuteCmd(string strCmd)
{
    return QProcess::startDetached(QString(strCmd.c_str()));;
}

void GeneralHelp::runProcess(const char *pszName, const char *pszCmd, bool bKill)
{
    if(nullptr == pszName || nullptr == pszCmd)
        return;

    //  step 01 强制杀掉本地推流进程
    string strCmd = Helper->format("ps -ef | grep %s | grep -v grep | awk '{print $2}' | xargs kill -9", pszName);
    if(bKill)
        system(strCmd.c_str());

    // step 02 启动
     excuteCmd(pszCmd);
}

bool GeneralHelp::isExistProcess(const char *pszProcessName)
{
    if(nullptr == pszProcessName)
        return false;

    bool bRet = false;

    FILE *fstream = nullptr;
    char buff[1024] = {0};

    //用空格，是去掉类似dah_main的噪声
    sprintf(buff, "ps -A | grep \" %s\"", pszProcessName);
    if (nullptr == (fstream=popen(buff, "r")))
        return false;

    QString strCurPid = QString::number(getpid());
    while (nullptr != fgets(buff, sizeof(buff), fstream))
    {
        if (strlen(buff) <= 0)
            break;

        QString strResult = buff;
        if(strResult.contains(strCurPid))
            continue;

        char* psHead = strstr(buff, pszProcessName);
        if (psHead == nullptr)
            continue;

        int pos = strlen(psHead)-1;
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

void GeneralHelp::replaceIp2Domain(string &strUrl, string strDomain, bool bIsReplaceHttp2Https)
{
    if(-1 == strUrl.find_first_of("://"))
        return;

    int nStartPos = strUrl.find_first_of("://") + 3;
    string _strUrl = strUrl.substr(nStartPos, strUrl.size() - nStartPos);
    if(_strUrl.find_first_of(":") < _strUrl.find_first_of("/"))     // 说明会有端口
    {
        strUrl.replace(nStartPos, _strUrl.find_first_of(":"), strDomain.c_str());
    }
    else            // 如果先找到了 “/” ，那就说明没有端口
    {
        int nFindPos = _strUrl.find_first_of("/") + nStartPos;
        strUrl.replace(nStartPos, nFindPos, strDomain.c_str());
    }

    if(bIsReplaceHttp2Https)
    {
        if(-1 == strUrl.find("https") && 0 == strUrl.find("http"))
        {
            strUrl.replace(0, 4, "https");
        }
    }
}

string GeneralHelp::replaceString(const char *pszSource, const char *pszOldKey, const char *pszNewKey)
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

   while(true)
   {
       string::size_type   pos(0);
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

void GeneralHelp::splitString(string& strSource, string strReps, vector<string>& vecString)
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

string GeneralHelp::string2Base64(const char* pszData, unsigned int nLen)
{
    if(nullptr == pszData)
        return "";

    QByteArray qByteArray = pszData;
    qByteArray = qByteArray.toBase64();
    QString qStr = qByteArray.data();
    return qStr.toStdString();
}

string GeneralHelp::base64ToString(string strData)
{
    QByteArray qByteArray = QByteArray(strData.c_str());
    QString strResult = QString::fromUtf8(qByteArray.fromBase64(qByteArray));
    return strResult.toStdString();
}

int GeneralHelp::encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput)
{
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

int GeneralHelp::decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput)
{
	int nCnt = 0;
	int nSrcLen = nLength;

	char * pChar = pszOutput;
	for (int nIndex = 0; nIndex < nSrcLen; nIndex++)
	{
		if (pszInput[nIndex] > 127)
			continue;

		if (pszInput[nIndex] == '=')
			return pChar - pszOutput + 1;

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
	return pChar - pszOutput;
}

bool GeneralHelp::write2File(string strFilePath, string& strContent)
{
    ofstream out(strFilePath.c_str());
    out << strContent.c_str() << endl;
    out.close();
    return true;
}

bool GeneralHelp::readFile(string strFilePath, string& strContent)
{
    FILE *pFile = fopen(strFilePath.c_str(), "r");
    if( nullptr == pFile )
        return false;

    fseek(pFile, 0, SEEK_END);
    int nLen = ftell(pFile);
    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    fseek(pFile, 0, SEEK_SET);
    fread(pBuf, nLen, 1, pFile);
    fclose(pFile);

    strContent = pBuf;
    delete [] pBuf;
    pBuf = nullptr;
    
    return true;;
}

bool GeneralHelp::isExistFile(string strFileName)
{
    return access(strFileName.c_str(), F_OK);
}

bool GeneralHelp::createDir(string strDir)
{
    if(isExistFile(strDir))
        return true;

    // 创建目录，需要循环创建每一层的目录
    vector<string> vecDir;
    splitString(strDir, "/", vecDir);

    string strPath = "/";
    for (size_t nIndex = 0; nIndex < vecDir.size(); nIndex++)
    {
        strPath = strPath + vecDir[nIndex] + "/";
        if(isExistFile(strPath))
            continue;
        
        mkdir(strPath.c_str(), 07777);
    }
    return  true;
}

string GeneralHelp::getCurTimeString(bool bIsMs, bool bIsPlain)
{
    return transMsTimeFromInt(getCurTimeStamp(bIsMs), bIsMs, bIsPlain);
}

string GeneralHelp::getAntennaCurTimeString(bool bIsMs)
{
    long long llDatetime = bIsMs ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
    QDateTime _time = bIsMs ? QDateTime::fromMSecsSinceEpoch(llDatetime) : QDateTime::fromSecsSinceEpoch(llDatetime);


    return bIsMs ? _time.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString() : _time.toString("yyyy-MM-dd hh:mm:ss").toStdString();

}

long long GeneralHelp::getCurTimeStamp(bool bIsMs)
{
    return bIsMs ? QDateTime::currentMSecsSinceEpoch() : QDateTime::currentSecsSinceEpoch();
}

bool GeneralHelp::getDirAllFiles(string strSuffix, string strDir, vector<tagFileInfo> &vecFiles)
{
    QDir qDir(strDir.c_str());
    if(!qDir.exists())
        return false;

    QFileInfoList subFiles = qDir.entryInfoList(QDir::Files | QDir::CaseSensitive);
    int nSize = subFiles.size() - 1 ;

    // 按照时间先后逆向塞入列表，越早的文件越在在后边
    for(int nIndex = nSize; nIndex >= 0; nIndex--)
    {
        QString strFileName = strDir.c_str() + subFiles[nIndex].baseName() + strSuffix.c_str();
        QFileInfo fileInfo(strFileName);

        tagFileInfo tagInfo;
        tagInfo.uLen = fileInfo.size();
        tagInfo.nCreateTime = fileInfo.created().toTime_t();
        tagInfo.nModifyTime = fileInfo.lastModified().toTime_t();
        tagInfo.strFileName = strFileName.toStdString();
        vecFiles.push_back(tagInfo);
    }

    return true;
}

// 判断ip是否合法
bool GeneralHelp::isVaildIp(const char* pszIp)
{
	if (nullptr == pszIp || '.' == *pszIp)		// 排除输入参数为NULL, 或者一个字符为'.'的字符串 
		return false;
    
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

void GeneralHelp::writeLog(int nType, string strLog, int nLine, const char* pszFuncName)
{
   if(nullptr == pszFuncName)
       return;

   strLog = format("[%s-%d]%s", pszFuncName, nLine, strLog.c_str());
   switch(nType)
   {
       case EnumLogType_Info:
           strLog = "[Info]" + strLog;
           break;
       case EnumLogType_Err:
           strLog = "[Error]" + strLog;
           break;
       case EnumLogType_Warning:
           strLog = "[Warning]" + strLog;
           break;
   }
   qDebug() << strLog.c_str();
}

string GeneralHelp::string2Hex(const char* pszBuf, int nLen)
{
    if(nullptr == pszBuf || nLen <= 0)
        return "";

    QByteArray szData(pszBuf, nLen);
    szData = szData.toHex();
    string strData = szData.toStdString();
//    string strData;
//    for (int nIndex = 0; nIndex < nLen; nIndex++)
//    {
//        strData += format("%02x", 0xFF & pszBuf[nIndex]);
//    }
    return strData;
}

uint16_t GeneralHelp::crc16(uint8_t* puchMsg, uint32_t usDataLen)
{
    short crctab16[] =
    {
        0, 4489, 8978, 12955, 17956, 22445, 25910, 29887,
        -29624, -25151, -20646, -16685, -13716, -9243, -5762, -1801, 4225,
         264, 13203, 8730, 22181, 18220, 30135, 25662, -25399, -29376,
         -16421, -20910, -9491, -13468, -1537, -6026, 8450, 12427,
         528, 5017, 26406, 30383, 17460, 21949, -21174, -17213, -29096,
         -24623, -5266, -1305, -14212, -9739, 12675, 8202, 4753, 792,
         30631, 26158, 21685, 17724, -16949, -21438, -24871, -28848,
         -1041, -5530, -9987, -13964, 16900, 21389, 24854, 28831, 1056,
         5545, 10034, 14011, -12724, -8251, -4770, -809, -30616, -26143,
         -21638, -17677, 21125, 17164, 29079, 24606, 5281, 1320, 14259,
         9786, -8499, -12476, -545, -5034, -26391, -30368, -17413,
         -21902, 25350, 29327, 16404, 20893, 9506, 13483, 1584, 6073,
         -4274, -313, -13220, -8747, -22166, -18205, -30088, -25615, 29575,
         25102, 20629, 16668, 13731, 9258, 5809, 1848, -49, -4538, -8995,
         -12972, -17941, -22430, -25863, -29840, -31736, -27263, -22758,
         -18797, -15828, -11355, -7874, -3913, 2112, 6601, 11090, 15067,
         20068, 24557, 28022, 31999, -27511, -31488, -18533, -23022, -11603,
         -15580, -3649, -8138, 6337, 2376, 15315, 10842, 24293, 20332, 32247,
         27774, -23286, -19325, -31208, -26735, -7378, -3417, -16324, -11851,
         10562, 14539, 2640, 7129, 28518, 32495, 19572, 24061, -19061, -23550,
         -26983, -30960, -3153, -7642, -12099, -16076, 14787, 10314, 6865, 2904,
         32743, 28270, 23797, 19836, -14836, -10363, -6882, -2921, -32728, -28255,
         -23750, -19789, 19012, 23501, 26966, 30943, 3168, 7657, 12146, 16123,
         -10611, -14588, -2657, -7146, -28503, -32480, -19525, -24014, 23237,
         19276, 31191, 26718, 7393, 3432, 16371, 11898, -6386, -2425, -15332,
         -10859, -24278, -20317, -32200, -27727, 27462, 31439, 18516, 23005,
         11618, 15595, 3696, 8185, -2161, -6650, -11107, -15084, -20053, -24542,
         -27975, -31952, 31687, 27214, 22741, 18780, 15843, 11370, 7921, 3960
    };

    int16_t fcs = -1;
    int i;
    uint16_t res;

    for (i = 0; i < usDataLen; i++)
    {
        fcs = (short)(((fcs & 0xFFFF) >> 8) ^ crctab16[(fcs ^ puchMsg[i]) & 0xFF]);
    }

    fcs = (int16_t)(~fcs);

    res = fcs;
    return res;
}

string GeneralHelp::getPlainDateTime(string strDateTime)
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

string GeneralHelp::int2HexString(int nData)
{
    QString qstrHex = QString::number(nData, 16);
    return qstrHex.toStdString();
}

string GeneralHelp::hex2String(const char* pszBuf, int nLen)
{
    if (nullptr == pszBuf || nLen <= 0)
        return "";

    bool ok;
    QString strData(pszBuf);
    string strHex;
    for (int i = 0; i < (nLen / 2); i++)
    {
        uint8_t nHex = (uint8_t)strData.mid(i*2, 2).toInt(&ok, 16);
        strHex.push_back(nHex);
    }
    return strHex;
}

long long GeneralHelp::transTimeStampfromString(string strTime, string strFormat, bool bIsMs)
{
    QDateTime dateTime = QDateTime::fromString(QString::fromStdString(strTime), QString::fromStdString(strFormat));
    return bIsMs? dateTime.toMSecsSinceEpoch() : dateTime.toSecsSinceEpoch();
}

string GeneralHelp::G2U(string strGBK)
{
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    //QTextCodec::setCodecForLocale(utf8);

    QByteArray szGBK = QByteArray::fromStdString(strGBK);
    QString strUnicode= gbk->toUnicode(szGBK.data());
    QByteArray utf8data = utf8->fromUnicode(strUnicode);
    return utf8data.toStdString();
}

//国标编码 转 utf8编码
QString GeneralHelp::GBK_2_UTF8(const char* p)
{
    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    QString strUnicode = gbk->toUnicode(p);
    QByteArray utf8_bytes = utf8->fromUnicode(strUnicode);

    QString result = QString::fromLocal8Bit(utf8_bytes);

    return result;
}

//utf8编码 转 国标编码
QString GeneralHelp::UTF8_2_GBK(const char* p)
{
    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    QString strUnicode = utf8->toUnicode(p);
    QByteArray gbk_bytes = gbk->fromUnicode(strUnicode);

    QString result = QString::fromLocal8Bit(gbk_bytes);

    return result;
}

QString GeneralHelp::GetCurrentTime()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString str = dateTime.toString("yyyy-MM-ddThh:mm:ss");
    return str;
}
