
/******************************************************************
@file    : Serial.cpp
@author  : 
@version : 1.0.0.0
@brief   : 串口实现类，实现具体的数据收发。
@history :
+Cteate this file。
******************************************************************/
#include "Platform.h"
#include "Serial.h"

#if IS_WINDOWS
//////////////////////////////////////////////////////////////////////////
//数据底层收发相关错误

#define LIB_NO_ERROR      (0)  //无错误//成功

#define LIB_ERR_COM_OPENFAIL      (-1002)   //打开设备失败
#define LIB_ERR_COM_NOTOPEN       (-9000) //(-1003)   //设备未打开
#define LIB_ERR_COM_BAUDRATE      (-1004) 
#define LIB_ERR_COM_PARITY        (-1005) 
#define LIB_ERR_COM_GETATTR       (-1006) 
#define LIB_ERR_COM_DATABITS      (-1007) 
#define LIB_ERR_COM_STOPBITS      (-1008) 
#define LIB_ERR_COM_SETATTR       (-1009) 
#define LIB_ERR_COM_BUILDDCB      (-1010) 
#define LIB_ERR_COM_PARAM         (-1011) 
#define LIB_ERR_COM_THREAD        (-1012) 
#define LIB_ERR_COM_DATA          (-1013) 
#define LIB_ERR_COM_INITNET       (-1014) 
#define LIB_ERR_COM_ADDR          (-1015) 
#define LIB_ERR_COM_WRITE         (-1016) 
#define LIB_ERR_COM_TIMEOUT       (-1017) 
#define LIB_ERR_COM_EXCEPTION     (-1018) 

//
#define SKCOMM_SAFECLOSE(p)  do { \
	if(HANDLE_IS_VALID(p)) \
	{\
		::CloseHandle(p); \
		p = NULL;\
	}\
}while(0)

/*****************************************************************
 函数名称：SerialDevString
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SerialDevString(int iSerialPort, char *pcString, int iMaxLen)
{
	if (iSerialPort < 10)
	{
		return snprintf(pcString, iMaxLen, "COM%d", iSerialPort);
	}
	else
	{
		return snprintf(pcString, iMaxLen, "\\\\.\\COM%d", iSerialPort);
	}
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SerialInterface::SerialInterface(void)
{
	m_hSerialHandle = NULL;
	memset(&m_stDCB, 0, sizeof(m_stDCB));
	m_stDCB.DCBlength = sizeof(m_stDCB);
	memset(&m_ovRead, 0, sizeof(m_ovRead));
	m_ovRead.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	memset(&m_ovWrite, 0, sizeof(m_ovWrite));
	m_ovWrite.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SerialInterface::SerialInterface(const char *pccCom, int iBaudrate, char cParity, int iDataBits, int iStopBits)
{
	m_hSerialHandle = NULL;
	memset(&m_stDCB, 0, sizeof(m_stDCB));
	m_stDCB.DCBlength = sizeof(m_stDCB);
	memset(&m_ovRead, 0, sizeof(m_ovRead));
	m_ovRead.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	memset(&m_ovWrite, 0, sizeof(m_ovWrite));
	m_ovWrite.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	Connect(pccCom, iBaudrate, cParity, iDataBits, iStopBits);
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SerialInterface::SerialInterface(const char *pccArgs)
{
	m_hSerialHandle = NULL;
	memset(&m_stDCB, 0, sizeof(m_stDCB));
	m_stDCB.DCBlength = sizeof(m_stDCB);
	memset(&m_ovRead, 0, sizeof(m_ovRead));
	m_ovRead.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	memset(&m_ovWrite, 0, sizeof(m_ovWrite));
	m_ovWrite.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	Connect(pccArgs);
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
SerialInterface::~SerialInterface(void)
{
	Disconnect(); //关闭端口
	SKCOMM_SAFECLOSE(m_ovRead.hEvent);
	SKCOMM_SAFECLOSE(m_ovWrite.hEvent);
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SerialInterface::SetSettings(int iBaudrate, char cParity, int iDataBits, int iStopBits)
{
	char szSettings[64];
	if (!this->IsConnected())
	{
		return LIB_ERR_COM_NOTOPEN;
	}
	if ((iBaudrate < 4800) || ((iBaudrate % 4800) != 0))
	{
		return LIB_ERR_COM_BAUDRATE;
	}
	if (cParity == 0)
	{
		cParity = 'N';
	}
	if ((cParity != 'N') && (cParity != 'n') && (cParity != 'E') && (cParity != 'e') && (cParity != 'o') && (cParity != 'O'))
	{
		return LIB_ERR_COM_PARITY;
	}
	if (iDataBits < 5 || iDataBits > 8)
	{
		return LIB_ERR_COM_DATABITS;
	}
	if ((iStopBits != 1) && (iStopBits != 2))
	{
		return LIB_ERR_COM_STOPBITS;
	}
	snprintf(szSettings, sizeof(szSettings), "baud=%ld parity=%c data=%d stop=%d", iBaudrate, cParity, iDataBits, iStopBits);
	::SetupComm(m_hSerialHandle, 4 * 1024, 4 * 1024);
	if (!::GetCommState(m_hSerialHandle, &m_stDCB))
	{
		return LIB_ERR_COM_GETATTR;
	}
	if (!::BuildCommDCBA(szSettings, &m_stDCB))
	{
		return  LIB_ERR_COM_BUILDDCB;
	}
	//禁用硬件流控制
	m_stDCB.fRtsControl = RTS_CONTROL_DISABLE;
	m_stDCB.fDtrControl = DTR_CONTROL_DISABLE;
	m_stDCB.fOutxCtsFlow = FALSE;
	m_stDCB.fOutxDsrFlow = FALSE;
	m_stDCB.fDsrSensitivity = FALSE;

	//禁用软件流控制
	m_stDCB.fOutX = FALSE;
	m_stDCB.fInX = FALSE;
	m_stDCB.XoffChar = FALSE;
	m_stDCB.XonChar = FALSE;

	if (!::SetCommState(m_hSerialHandle, &m_stDCB))
	{
		return LIB_ERR_COM_SETATTR;
	}
	return LIB_NO_ERROR;
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SerialInterface::SetTimeOuts(int iTxTimeoutMs, int iRxTimeoutMs)
{
	COMMTIMEOUTS CommTimeouts;
	if (!this->IsConnected())
	{
		return LIB_ERR_COM_NOTOPEN;// -5;    
	}

	if ((iTxTimeoutMs == 0) && (iRxTimeoutMs == 0))
	{
		CommTimeouts.ReadIntervalTimeout = MAXDWORD;
		CommTimeouts.ReadTotalTimeoutConstant = 5000;  //读间隔超时 //读时间常量
		CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;  //读时间系数 //读时间系数
		// 
		CommTimeouts.WriteTotalTimeoutConstant = 5000;   //写数据总超时常量
		CommTimeouts.WriteTotalTimeoutMultiplier = 0;     //时间系数：平均写一个字节的时间上限
	}
	else
	{
		CommTimeouts.ReadIntervalTimeout = MAXDWORD;
		CommTimeouts.ReadTotalTimeoutConstant = iRxTimeoutMs;
		CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
		CommTimeouts.WriteTotalTimeoutConstant = iTxTimeoutMs;
		CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	}
	if (!::SetCommTimeouts(m_hSerialHandle, &CommTimeouts))
	{
		return LIB_ERR_COM_SETATTR;
	}
	return LIB_NO_ERROR;
}

/*****************************************************************
 函数名称：SerialInterface
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SerialInterface::Connect(const char *pccCom, int iBaudrate, char cParity, int iDataBits, int iStopBits)
{
	int iPortIdx = 0;
	if (HANDLE_IS_VALID(m_hSerialHandle))
	{
		Disconnect();
	}
	m_iBaudrate = iBaudrate;
	m_cParity = cParity;
	m_iDataBits = iDataBits;
	m_iStopBits = iStopBits;
	if (sscanf_s(pccCom, "COM%d", &iPortIdx) != 1)
	{
		Log("%s sscanf_s COM Name(%s) Error\n", __FUNCTION__, m_cComName);
		return false;
	}
	SerialDevString(iPortIdx, m_cComName, sizeof(m_cComName));
	Log("%s Create File for %s\n", __FUNCTION__, m_cComName);
	m_hSerialHandle = ::CreateFileA(m_cComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);////重叠I/O
	if (!HANDLE_IS_VALID(m_hSerialHandle))
	{
		m_hSerialHandle = NULL;
		m_iErrorCode = ::GetLastError();
		Log("%s CreateFile For Serial %s Error ErrorCode = %d\n", __FUNCTION__, m_cComName, m_iErrorCode);
		return false;
	}
	Log("%s CreateFile Return Handle %p\n", __FUNCTION__, m_hSerialHandle);
	//参数设置
	m_iErrorCode = SetSettings(iBaudrate, cParity, iDataBits, iStopBits);
	if (LIB_NO_ERROR != m_iErrorCode)
	{
		SKCOMM_SAFECLOSE(m_hSerialHandle);
		Log("%s Set Param Error ErrorCode = %d\n", __FUNCTION__, m_iErrorCode);
		return false;
	}
	//参数设置
	m_iErrorCode = SetTimeOuts(1000, COMM_RECV_SCAN_INTRERVAL_MS);
	if (LIB_NO_ERROR != m_iErrorCode)
	{
		SKCOMM_SAFECLOSE(m_hSerialHandle);
		Log("%s Set Params Error ErrorCode = %d\n", __FUNCTION__, m_iErrorCode);
		return false;
	}
	::PurgeComm(m_hSerialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	::SetCommMask(m_hSerialHandle, EV_RXCHAR);//EV_CTS|EV_RING|
	m_iErrorCode = ::GetLastError();
	if (m_iErrorCode != NO_ERROR)
	{
		Log("%s PurgeComm & SetCommMask Error ErrorCode = %d\n", __FUNCTION__, m_iErrorCode);
	}
	while (StartRxIORequest(NULL) == true)
	{
		Sleep(10);
	}
	return HANDLE_IS_VALID(m_hSerialHandle);
}

/*****************************************************************
 函数名称：Connect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SerialInterface::Connect(const char *pccArgs)
{
	char cCom[16];
	int iBaudrate = GetKeywordInt(pccArgs, "Baudrate");
	char cParity = GetKeywordChar(pccArgs, "Parity");
	int iDataBits = GetKeywordInt(pccArgs, "DataBits");
	int iStopBits = GetKeywordInt(pccArgs, "StopBits");
	if (iBaudrate < 0)
	{
		iBaudrate = 115200;
	}
	if (cParity < 0)
	{
		cParity = 'N';
	}
	if (iDataBits < 0)
	{
		iDataBits = 8;
	}
	if (iStopBits < 0)
	{
		iStopBits = 2;
	}
	if (GetKeywordString(pccArgs, "Port", cCom, sizeof(cCom)) > 0)
	{
		return Connect(cCom, iBaudrate, cParity, iDataBits, iStopBits);
	}
	return false;
}

/*****************************************************************
 函数名称：TestAndReconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SerialInterface::TestAndReconnect(void)
{
	if (!HANDLE_IS_VALID(m_hSerialHandle))
	{
		Log("%s Try To Reconnect Serial %s\n", __FUNCTION__, m_cComName);
		return Connect(m_cComName, m_iBaudrate, m_cParity, m_iDataBits, m_iStopBits);
	}
	return true;
}

/*****************************************************************
 函数名称：SendTo
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SerialInterface::SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iLen, int iTimeoutMs)
{
	DWORD dwResult, dwBytesWriten = 0;
	TestAndReconnect();
	if (!HANDLE_IS_VALID(m_hSerialHandle))
	{
		PlatSleepMs((iTimeoutMs >= 0) ? iTimeoutMs : 100);
		return LIB_ERR_COM_NOTOPEN;
	}
	m_ovWrite.Offset = m_ovWrite.OffsetHigh = 0;
	m_ovWrite.Internal = m_ovWrite.InternalHigh = 0;
	::ResetEvent(m_ovWrite.hEvent);
	//LogData(pcvData, iLen, __FUNCTION__);
	if ((dwResult = ::WriteFile(m_hSerialHandle, pcvData, iLen, &dwBytesWriten, &m_ovWrite)) == FALSE)
	{
		if ((m_iErrorCode = ::GetLastError()) != ERROR_IO_PENDING)
		{
			Disconnect();
			Log("%s WriteFile ErrorCode = %d\n", __FUNCTION__, m_iErrorCode);
			return -m_iErrorCode;
		}
		dwBytesWriten = 0;
		iTimeoutMs = (iTimeoutMs < 0) ? 100 : iTimeoutMs;
		dwResult = ::WaitForSingleObjectEx(m_ovWrite.hEvent, iTimeoutMs, FALSE);
		if (dwResult != WAIT_OBJECT_0)
		{
			m_iErrorCode = dwResult;
		}
		else if (::GetOverlappedResult(m_hSerialHandle, &m_ovWrite, &dwBytesWriten, FALSE) == FALSE)
		{
			//if (::CancelIoEx(m_hSerialHandle, &m_ovWrite) == FALSE) //WINXP不支持
			if (::CancelIo(m_hSerialHandle) == FALSE)
			{
				::GetOverlappedResult(m_hSerialHandle, &m_ovWrite, &dwBytesWriten, FALSE);
			}
			m_iErrorCode = ::GetLastError();
		}
		//Log("1:%s WriteFile ErrorCode = %d, dwResult = %d\n", __FUNCTION__, m_iErrorCode, dwResult);
	}
	else
	{
		m_iErrorCode = ::GetLastError();
		//Log("2:%s WriteFile ErrorCode = %d, dwResult = %d\n", __FUNCTION__, m_iErrorCode, dwResult);
	}
	return dwBytesWriten;
}

/*****************************************************************
 函数名称：Disconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
void SerialInterface::Disconnect(void)
{
	if (HANDLE_IS_VALID(m_hSerialHandle))
	{
		Log("%s %s, Close Handle %p\n", __FUNCTION__, m_cComName, m_hSerialHandle);
		//::CancelIoEx(m_hSerialHandle, &m_ovRead);  //CancelIoEx WINXP不支持
		//::CancelIoEx(m_hSerialHandle, &m_ovWrite); //CancelIoEx WINXP不支持
		::CancelIo(m_hSerialHandle);
		::CloseHandle(m_hSerialHandle);
		m_hSerialHandle = NULL;
	}
}

/*****************************************************************
 函数名称：RecvFrom
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SerialInterface::RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs)
{
	BYTE *pucBytes = (BYTE *)(pvData);
	DWORD dwBytesRead = 0, dwResult = FALSE, dwRetryCnt = 0;
	iTimeoutMs = (iTimeoutMs < 0) ? 100 : iTimeoutMs;
	while ((dwRetryCnt < 32) && (iMaxLen > 0) && (HANDLE_IS_VALID(m_hSerialHandle)))
	{
		dwRetryCnt++;
		if ((dwResult == FALSE) && (dwBytesRead == 0))
		{
			dwResult = ::WaitForSingleObjectEx(m_ovRead.hEvent, iTimeoutMs, FALSE);
			if (dwResult == WAIT_OBJECT_0)
			{
				dwBytesRead = 0;
				dwResult = ::GetOverlappedResult(m_hSerialHandle, &m_ovRead, &dwBytesRead, FALSE);
				m_iErrorCode = ::GetLastError();
			}
			else
			{
				m_iErrorCode = dwResult;
				dwResult = FALSE;
			}
			if ((dwResult == FALSE) && (m_iErrorCode != ERROR_SUCCESS) && (m_iErrorCode != ERROR_IO_PENDING) &&
				(m_iErrorCode != WAIT_TIMEOUT) && (m_iErrorCode != ERROR_TIMEOUT) && (m_iErrorCode != ERROR_IO_INCOMPLETE))
			{
				Log("%s dwResult = %d, m_iErrorCode = %d\n", __FUNCTION__, dwResult, m_iErrorCode);
				dwBytesRead = 0;
				//CancelIoEx(m_hSerialHandle, &m_ovRead); //WINXP不支持
				::CancelIo(m_hSerialHandle);
				dwResult = StartRxIORequestTryReconnect(&dwBytesRead);
			}
		}
		if ((dwResult == TRUE) || (dwBytesRead > 0))
		{
			if (dwBytesRead > 0)
			{
				dwBytesRead = (dwBytesRead > (DWORD)(iMaxLen)) ? (DWORD)iMaxLen : dwBytesRead;
				memcpy(pucBytes, m_cRxBuf, dwBytesRead);
				pucBytes += dwBytesRead;
				iMaxLen -= (int)(dwBytesRead);
			}
			else
			{
				//CancelIoEx(m_hSerialHandle, &m_ovRead); //WINXP不支持
				::CancelIo(m_hSerialHandle);
			}
			dwBytesRead = 0;
			dwResult = StartRxIORequestTryReconnect(&dwBytesRead);
			//Log("%s StartRxIORequest 2 dwResult = %d, dwBytesRead = %d, m_iErrorCode = %d\n", __FUNCTION__, dwResult, dwBytesRead, m_iErrorCode);
		}
		if ((dwResult == FALSE) && (dwBytesRead == 0))
		{
			break;
		}
	}
	return (int)(pucBytes - (BYTE *)(pvData));
}

/*****************************************************************
 函数名称：WakeUpWaitting
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
void SerialInterface::WakeUpWaitting(void)
{
	if (!HANDLE_IS_VALID(m_hSerialHandle))
	{
		return;
	}
	if (HANDLE_IS_VALID(m_ovRead.hEvent))
	{
		::SetEvent(m_ovRead.hEvent);
	}
	if (HANDLE_IS_VALID(m_ovWrite.hEvent))
	{
		::SetEvent(m_ovWrite.hEvent);
	}
}
/*****************************************************************
 函数名称：StartRxIORequest
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SerialInterface::StartRxIORequest(DWORD *pdwBytesRead)
{
	DWORD dwBytesRead = 0;
	if (!HANDLE_IS_VALID(m_hSerialHandle))
	{
		return false;
	}
	m_ovRead.Offset = m_ovRead.OffsetHigh = 0;
	m_ovRead.Internal = m_ovRead.InternalHigh = 0;
	::ResetEvent(m_ovRead.hEvent);
	pdwBytesRead = (pdwBytesRead == NULL) ? &dwBytesRead : pdwBytesRead;
	if (::ReadFile(m_hSerialHandle, m_cRxBuf, sizeof(m_cRxBuf), pdwBytesRead, &m_ovRead) == TRUE)
	{
		return true;
	}
	if ((m_iErrorCode = ::GetLastError()) == ERROR_IO_PENDING) //997
	{
		return false;
	}
	Log("%s ErrorCode = %d\n", __FUNCTION__, m_iErrorCode);
	Disconnect();
	return false;
}
/*****************************************************************
 函数名称：StartRxIORequestTryReconnect
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
bool SerialInterface::StartRxIORequestTryReconnect(DWORD *pdwBytesRead)
{
	DWORD dwBytesRead = 0, dwRetry;
	for (dwRetry = 3; dwRetry > 0; dwRetry--)
	{
		bool bReconnected = false;
		if (!HANDLE_IS_VALID(m_hSerialHandle))
		{
			TestAndReconnect();
			if (!HANDLE_IS_VALID(m_hSerialHandle))
			{
				continue;
			}
			bReconnected = true;
		}
		if (bReconnected == true)
		{
			break;
		}
		m_ovRead.Offset = m_ovRead.OffsetHigh = 0;
		m_ovRead.Internal = m_ovRead.InternalHigh = 0;
		::ResetEvent(m_ovRead.hEvent);
		pdwBytesRead = (pdwBytesRead == NULL) ? &dwBytesRead : pdwBytesRead;
		if (::ReadFile(m_hSerialHandle, m_cRxBuf, sizeof(m_cRxBuf), pdwBytesRead, &m_ovRead) == TRUE)
		{
			return true;
		}
		if ((m_iErrorCode = ::GetLastError()) == ERROR_IO_PENDING) //997
		{
			return false;
		}
		Log("%s ErrorCode = %d\n", __FUNCTION__, m_iErrorCode);
		Disconnect();
		continue;
	}
	return false;
}
#endif

