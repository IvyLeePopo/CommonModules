#include "CSocket_GT.h"

bool CHDLCnet::Init_NetWork(const char *pBufIpAddr, const unsigned short iPort, SOCKET &socket_Out)
{
	SOCKET sSocket_Temp;

	int iError = -1;
	int len = sizeof(int);

	fd_set rfdSet; //描述符集 这个将测试连接是否可用
	FD_ZERO(&rfdSet); //先清空描述符集

	timeval TimeOut;
	TimeOut.tv_sec = 6; //连接超时时间为6秒
	TimeOut.tv_usec = 0;

	u_long ul;

	//初始化套结字动态库
	WSADATA wsd; //WSADATA变量
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return false;
	}

	//创建套接字
	sSocket_Temp = socket(AF_INET, SOCK_STREAM, 0);
	if (sSocket_Temp == INVALID_SOCKET)
	{
		WSACleanup();//释放套接字资源
		return false;
	}

	SOCKADDR_IN sAddr_Temp;
	sAddr_Temp.sin_addr.S_un.S_addr = inet_addr(pBufIpAddr);
	sAddr_Temp.sin_family = AF_INET;
	sAddr_Temp.sin_port = htons(iPort);

	ul = 1;
	ioctlsocket(sSocket_Temp, FIONBIO, &ul); //设置为非阻塞模式
	connect(sSocket_Temp, (SOCKADDR*)&sAddr_Temp, sizeof(SOCKADDR));

	FD_SET(sSocket_Temp, &rfdSet);
	if (select(sSocket_Temp + 1, 0, &rfdSet, 0, &TimeOut) <= 0)
	{
		ClosePort();
		WSACleanup();//释放套接字资源
		return false;
	}
	getsockopt(sSocket_Temp, SOL_SOCKET, SO_ERROR, (char*)&iError, (int *)&len);
	ul = 0;
	ioctlsocket(sSocket_Temp, FIONBIO, &ul); //设置为阻塞模式
	setsockopt(sSocket_Temp, SOL_SOCKET, SO_SNDTIMEO, (char*)&iNetTimeOut, sizeof(int)); //设置发送的超时时间
	setsockopt(sSocket_Temp, SOL_SOCKET, SO_RCVTIMEO, (char*)&iNetTimeOut, sizeof(int)); //设置接收的超时时间

	socket_Out = sSocket_Temp;
	return true;
}

void CHDLCnet::Create_Write_Messgae(byte BufferIn[], byte addr, int ilen, char BufOut[])
{
	byte checksum = 0;
	memset(BufOut, 0, sizeof(BufOut));

	BufOut[0] = '{';
	BufOut[1] = 'G';
	BufOut[2] = 'T';
	BufOut[3] = addr;
	BufOut[4] = 0x00;
	BufOut[5] = 0x00;
	BufOut[6] = 0x01;
	BufOut[7] = ilen;
	for (int i = 8; i < ilen + 8; i++)
	{
		BufOut[i] = BufferIn[i - 8];
	}
	for (int i = 3; i < ilen + 8; i++)
	{
		checksum += (byte)BufOut[i];
	}
	BufOut[8 + ilen] = 0xff - checksum;
	BufOut[9 + ilen] = 'E';
	BufOut[10 + ilen] = 'N';
	BufOut[11 + ilen] = 'D';
	BufOut[12 + ilen] = '}';
}

void CHDLCnet::Create_Read_Message(byte addr, char BufferOut[])
{
	byte checksum = 0;
	BufferOut[0] = '{';
	BufferOut[1] = 'G';
	BufferOut[2] = 'T';
	BufferOut[3] = addr;
	BufferOut[4] = 0x00;
	BufferOut[5] = 0x00;
	BufferOut[6] = 0x00;
	BufferOut[7] = 0x00;
	for (int i = 3; i < 8; i++)
	{
		checksum += BufferOut[i];
	}
	BufferOut[8] = 0xff - checksum;
	BufferOut[9] = 'E';
	BufferOut[10] = 'N';
	BufferOut[11] = 'D';
	BufferOut[12] = '}';
}

bool CHDLCnet::Parse_Read_Data(char RecvData[], int iDataLen, char RecvBuf[])
{
	int iPos_Begin = Get_Head_RecvData(RecvData, iDataLen, true);
	int iPos_End = Get_Tail_RecvData(RecvData, iPos_Begin, iDataLen);

	if ((iPos_Begin < 0) || (iPos_End < 0) || (iPos_End < iPos_Begin + 10))
	{
		return false;
	}

	byte checkSum = 0;
	for (int i = iPos_Begin + 3; i < iPos_End - iPos_Begin - 3; i++)
	{
		checkSum += RecvData[i];
	}
	if (checkSum != 0xff)
	{
		return false;
	}
	for (int i = 0; i < iPos_End - iPos_Begin - 12; i++)
	{
		RecvBuf[i] = RecvData[i + iPos_Begin + 8];
	}
	return true;
}

bool CHDLCnet::Parse_Write_Data(char RecvData[], int iDataLen)
{
	int iPos_Begin = Get_Head_RecvData(RecvData, iDataLen, false);
	int iPos_End = Get_Tail_RecvData(RecvData, iPos_Begin, iDataLen);

	if ((iPos_Begin < 0) || (iPos_End < 0) || (iPos_End < iPos_Begin + 10))
	{
		return false;
	}

	byte checkSum = 0;
	for (int i = iPos_Begin + 3; i < iPos_End - iPos_Begin - 3; i++)
	{
		checkSum += RecvData[i];
	}
	if (checkSum != 0xff)
	{
		return false;
	}
	return true;
}

int CHDLCnet::Get_Head_RecvData(char RecvData[], int iDataLen, bool bIsRead)
{
	int iPos_Output = -1;
	for (int i = 0; i < iDataLen; i++)
	{
		//即找到'{'在数组中的索引
		if (0x7B != RecvData[i]) // "{" = 123(十进制) = 0x7b（十六进制）
		{
			continue;
		}
		if (i + 6 > iDataLen)
		{
			continue;
		}
		if (0x47 != RecvData[i + 1]) // "G" = 71(十进制) = 0x47（十六进制）
		{
			continue;
		}
		if (0x54 != RecvData[i + 2]) // "T" = 84(十进制) = 0x54（十六进制）
		{
			continue;
		}
		if (0x00 != RecvData[i + 4]) // 指令版本号_1
		{
			continue;
		}
		if (0x00 != RecvData[i + 5]) // 指令版本号_2
		{
			continue;
		}
		if (bIsRead)
		{
			if (0x00 != RecvData[i + 6]) // CMD-1
			{
				continue;
			}
		}
		else
		{
			if (0x01 != RecvData[i + 6]) // CMD-1
			{
				continue;
			}
		}
		iPos_Output = i;
		break;
	}

	return iPos_Output;
}

int CHDLCnet::Get_Tail_RecvData(char RecvData[], const int iIndex_Begin, int iDataLen)
{
	int iPos_Output = -1;
	for (int i = iIndex_Begin; i < iDataLen; i++)
	{
		//即找到'}'在数组中的索引
		if (0x7d != RecvData[i])
		{
			continue;
		}
		if (i - 3 < 0)
		{
			continue;
		}
		if (0x45 != RecvData[i - 3]) // "E" = 69(十进制) = 0x45（十六进制）
		{
			continue;
		}
		if (0x4e != RecvData[i - 2]) // "N" = 78(十进制) = 0x4e（十六进制）
		{
			continue;
		}
		if (0x44 != RecvData[i - 1]) // "D" = 68(十进制) = 0x44（十六进制）
		{
			continue;
		}
		if (0x7d != RecvData[i]) // "}" = 125(十进制) = 0x7d（十六进制）
		{
			continue;
		}
		iPos_Output = i;
		break;
	}
	return iPos_Output;
}

QString CHDLCnet::Get_Hex_ErrorInfo(const char ReadData[], const int iDataLen)
{
	QString strOutput;
	char szError[300] = { 0 };
	int iSpaceCount = iDataLen - 1; //空格的数量
	int iPos_End = -1;
	for (int i = 0; i < iDataLen; i++)
	{
		//即找到'}'在数组中的索引
		if (0x7d == ReadData[i])
		{
			iPos_End = i;
		}
	}
	if (iPos_End < 0)
	{
		strOutput = "NULL";
	}
	else
	{
		for (int i = 0; i < iPos_End; i++)
		{
			szError[2 * i] = ReadData[i];
			szError[2 * i - 1] = ' ';
		}
		strOutput = ((QByteArray)szError).toHex();
	}
	return strOutput;
}

std::string CHDLCnet::byteToHexStr(byte *pData, int iLen)
{
	std::string strHex, strTemp_1, strTemp_2;
	strHex.clear();
	for (int i = 0; i < iLen; i++)
	{
		int high = pData[i] / 16;
		int low = pData[i] % 16;
		strTemp_1 = (high < 10) ? ('0' + high) : ('A' + high - 10);
		strTemp_2 = (low < 10) ? ('0' + low) : ('A' + low - 10);
		strHex.append(strTemp_1);
		strHex.append(strTemp_2);
	}
	return strHex;
}
