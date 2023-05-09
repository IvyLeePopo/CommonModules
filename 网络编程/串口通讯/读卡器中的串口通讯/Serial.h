
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "CommInterface.h"
#include "SysDefines.h"

typedef enum enSerialErrorCode
{
    SERIAL_NO_ERROR = 0,
    SERIAL_ERROR_CODE_TIMEOUT = 0,
    SERIAL_ERROR_CODE_OPEN_FAIL = -100,
    SERIAL_ERROR_CODE_PARAMETER = -101,
    SERIAL_ERROR_CODE_NOT_OPEN = -102,
    SERIAL_ERROR_CODE_SET_SETTINGS = -103,
    SERIAL_ERROR_CODE_WAKEUP_BY_EXT = -104,
    SERIAL_ERROR_CODE_EXCEPTION = -105,
    SERIAL_ERROR_CODE_RW_ERROR = -106, 
}enSerialErrorCode_t;

//
#define COMM_RECV_SCAN_INTRERVAL_MS  (1000)
/*****************************************************************
 函数名称：SerialDevString
 函数描述：
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int SerialDevString(int iSerialPort, char *pcString, int iMaxLen);


//////////////////////////////////////////////////////////////////////////
class SerialInterface : public CommInterface
{
public:
	SerialInterface(void);
	SerialInterface(const char *pccCom, int iBaudrate, char cParity, int iDataBits, int iStopBits);
	SerialInterface(const char *pccArgs);
	~SerialInterface(void);

	bool Connect(const char *pccCom, int iBaudrate, char cParity, int iDataBits, int iStopBits);
	bool Connect(const char *pccArgs);
	bool TestAndReconnect(void);
	void Disconnect(void);
	int RecvFrom(DWORD *pdwSrcIP, WORD *pwSrcPort, void *pvData, int iMaxLen, int iTimeoutMs);
	int SendTo(DWORD dwDstIP, WORD wDstPort, const void *pcvData, int iLen, int iTimeoutMs);
	bool IsConnected(void) { return HANDLE_IS_VALID(m_hSerialHandle); }
	int GetErrorCode(void) { return m_iErrorCode; }
	long GetHandle(void) { return (long)(m_hSerialHandle); }
	void WakeUpWaitting(void);
private:
	//参数设置    
	HANDLE m_hSerialHandle = 0;
	int m_iErrorCode = 0, m_iSendErrCnt = 0;
	int m_iBaudrate = 115200, m_iDataBits = 8, m_iStopBits = 1;
	char m_cParity = 'N', m_cComName[16] = { 0 }, m_cRxBuf[1024];
	int SetSettings(int iBaudrate = 115200, char cParity = 'N', int iDataBits = 8, int iStopBits = 1);
	#if IS_WINDOWS
	bool StartRxIORequest(DWORD *pdwBytesRead);
	bool StartRxIORequestTryReconnect(DWORD *pdwBytesRead);
	int SetTimeOuts(int iTxTimeoutMs = 1000, int iRxTimeoutMs = COMM_RECV_SCAN_INTRERVAL_MS);
	DCB m_stDCB;
	OVERLAPPED m_ovRead, m_ovWrite;
	#else
    bool FindHandle(const HANDLE chHandles[], int iCnt, HANDLE hHandle);
	#endif
};
#endif

