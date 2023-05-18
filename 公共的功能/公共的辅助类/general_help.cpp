#include "general_help.h"

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
#include <windows.h>

typedef unsigned char BYTE;
GeneralHelp::GeneralHelp()
{

}

GeneralHelp::~GeneralHelp()
{

}



std::string GeneralHelp::format(const char * pFmt, ...)
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

