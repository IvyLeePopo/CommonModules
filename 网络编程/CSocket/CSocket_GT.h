/*
 * @Description: 推荐这个Socket，相对简单些
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#include <QString>
#include <vector>
#include <string>
#include "winsock2.h"

#pragma comment(lib, "ws2_32.lib")

#pragma once


const int iNetTimeOut = 1000;


class CHDLCnet
{
public:
	virtual~CHDLCnet(void)
	{};

public:
	virtual bool OpenPort(const char *pBufIpAddr, const unsigned short iPort) = 0;
	virtual bool ClosePort() = 0;
	virtual bool Write(byte Buffer[], byte addr, int iBufferLen) = 0;
	virtual bool Read(byte addr, char RecvBuf[]) = 0;

public:
	inline bool Init_NetWork(const char *pBufIpAddr, const unsigned short iPort, SOCKET &socket_Out);

	inline void Create_Write_Messgae(byte BufferIn[], byte addr, int ilen, char BufOut[]);
	inline void Create_Read_Message(byte addr, char BufferOut[]);
	inline bool Parse_Read_Data(char RecvData[], int iDataLen, char RecvBuf[]);
	inline bool Parse_Write_Data(char RecvData[], int iDataLen);
	inline int Get_Head_RecvData(char RecvData[], int iDataLen, bool bIsRead);
	inline int Get_Tail_RecvData(char RecvData[], const int iIndex_Begin, int iDataLen);
	inline QString Get_Hex_ErrorInfo(const char ReadData[], const int iDataLen);

	inline std::string byteToHexStr(byte *pData, int iLen);

private:

};