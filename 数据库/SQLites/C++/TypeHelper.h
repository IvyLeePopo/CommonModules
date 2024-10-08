/*
* Copyright(C) 2017,苹果 保留所有权利。( All rights reserved. )
* 文件名称: TypeHelper.h
* 摘   要:  
* 当前版本:1.0.0.0
* 作   者: Ivy
* 创建时间: 2017-04-12
* 修改记录: 
*/
#ifndef TYPE_HELPER_H
#define TYPE_HELPER_H
#include <string>
#include <vector>
#include <list>
#include <windows.h>

std::string Bin2Hex(const unsigned char* bin_buf,size_t buf_len);

std::string Bin2Hex(const std::vector<unsigned char> src);
std::string Bin2Hex(const std::list<unsigned char> src);
std::string Int2Str(int info);

int Str2Int(const std::string& info);
bool Hex2Bin(const char* hex_buf,char* bin_buf,size_t hex_len);
std::string HexToBase64( const std::string& strSrcHex_org );

time_t Str2Datetime(const std::string& info);
std::string Datetime2Str(time_t dt);
std::string _Datetime2Str(time_t dt);



std::string ChinaTime2Str(time_t t);
std::string _ChinaTime2Str(time_t t);
time_t Str2ChinaTime(const std::string& time_str);
time_t ChinaTime();
std::string ChinaTimeStr();
std::string ChinaTimeStrWithMS();

unsigned int String2IP(const std::string& info);


std::vector< std::vector<unsigned char>> SplitCharArray(const unsigned char* src,int len,unsigned char split_ch);
std::string StringConvertByCodePage( const std::string& strConvertData_org,  const int& iSourceCodepage,  const int& iTargetCodepage);
__int64 GetDiffMilliSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2) ;

int GetFileModifyTime(const std::string& file_path,SYSTEMTIME& sys_time);
std::string GetGuid();

#endif // TYPE_HELPER_H
