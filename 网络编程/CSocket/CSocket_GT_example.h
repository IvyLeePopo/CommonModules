#include "CSocket_GT.h"

// 实例演示：包含该类的头文件

class CRosplatNet :	public CHDLCnet
{
public:
	CRosplatNet();
	virtual ~CRosplatNet();

public:
	bool OpenPort(const char *pBufIpAddr, const unsigned short iPort);
	bool ClosePort();
	bool Write(byte Buffer[], byte addr, int iBufferLen);
	bool Read(byte addr, char RecvBuf[]);

private:
	void Create_Ros_Write_Messgae(byte BufferIn[], byte addr, int ilen, char BufOut[]);
	bool Parse_Ros_Read_Data(char RecvData[], int iDataLen, char RecvBuf[]);

	int Get_Ros_Head_RecvData(char RecvData[], int iDataLen, bool bIsRead);

private:
	SOCKET m_sSocket;
	SOCKADDR_IN m_sAddr;
};