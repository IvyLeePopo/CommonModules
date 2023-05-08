#include "CSocket_GT_example.h"


CRosplatNet::CRosplatNet()
{
}


CRosplatNet::~CRosplatNet()
{
}

bool CRosplatNet::OpenPort(const char *pBufIpAddr, const unsigned short iPort)
{
	if (false == Init_NetWork(pBufIpAddr, iPort, m_sSocket))
	{
		return false;
	}
	return true;
}

bool CRosplatNet::ClosePort()
{
	if (SOCKET_ERROR == closesocket(m_sSocket))
	{
		return false;
	}
	return true;
}

bool CRosplatNet::Write(byte Buffer[], byte addr, int iBufferLen)
{
	char szCmd[30] = { 0 };
	char pBuffer[100] = { 0 };
	Create_Ros_Write_Messgae(Buffer, addr, iBufferLen, szCmd);
	Sleep(10);

	if (SOCKET_ERROR == send(m_sSocket, szCmd, 0x0d + iBufferLen, 0))
	{
		return false;
	}

	return true;
}

bool CRosplatNet::Read(byte addr, char RecvBuf[])
{
	char pBuffer[100] = { 0 };

	Sleep(10);


	if (SOCKET_ERROR == recv(m_sSocket, pBuffer, sizeof(pBuffer), 0))
	{
		return false;
	}
	if (false == Parse_Ros_Read_Data(pBuffer, sizeof(pBuffer), RecvBuf))
	{
		return false;
	}
	return true;
}

void CRosplatNet::Create_Ros_Write_Messgae(byte BufferIn[], byte addr, int ilen, char BufOut[])
{
	byte checksum = 0;
	memset(BufOut, 0, sizeof(BufOut));

	BufOut[0] = '{';
	BufOut[1] = 'G';
	BufOut[2] = 'T';
	BufOut[3] = addr;
	BufOut[4] = 0x5A;
	BufOut[5] = 0x58;
	BufOut[6] = 0x00;
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

bool CRosplatNet::Parse_Ros_Read_Data(char RecvData[], int iDataLen, char RecvBuf[])
{
	int iPos_Begin = Get_Ros_Head_RecvData(RecvData, iDataLen, true);
	int iPos_End = Get_Tail_RecvData(RecvData, iPos_Begin, iDataLen);

	if ((iPos_Begin < 0) || (iPos_End < 0) || (iPos_End < iPos_Begin + 10))
	{
		return false;
	}

	byte checkSum = 0;
	for (int i = iPos_Begin + 3; i < iPos_End - 3; i++)
	{
		byte check_Temp = RecvData[i];
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

int CRosplatNet::Get_Ros_Head_RecvData(char RecvData[], int iDataLen, bool bIsRead)
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
		if (0x5A != RecvData[i + 4]) // 指令版本号_1
		{
			continue;
		}
		if (0x58 != RecvData[i + 5]) // 指令版本号_2
		{
			continue;
		}
		if (0x01 != RecvData[i + 6]) // CMD-1
		{
			continue;
		}
		iPos_Output = i;
		break;
	}

	return iPos_Output;
}