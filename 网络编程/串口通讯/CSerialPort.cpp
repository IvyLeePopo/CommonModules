#include "CSerialPort.h"


CSerialPort::CSerialPort()
{
	m_hComm = NULL;
}


CSerialPort::~CSerialPort()
{
}

bool CSerialPort::InitPort(int iPort, int iBaud, QString strParity, int iDatabit, int iStopbit, DWORD dwCommEvents)
{
	COMMTIMEOUTS CommTimeouts;
	DCB	CommDcb;

	if (NULL != m_hComm)
	{
		ClosePort();
	}

	QString strPort = "COM" + QString::number(iPort);
	QString strBaud = QString::number(iBaud) + "," + strParity + "," + QString::number(iDatabit) + "," + QString::number(iStopbit);

	LPCWSTR lpsPort = reinterpret_cast<const wchar_t *>(strPort.utf16());
	LPCWSTR lpsBaud = reinterpret_cast<const wchar_t *>(strBaud.utf16());

	m_hComm = ::CreateFile(lpsPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
	if (INVALID_HANDLE_VALUE == m_hComm)//打开串口失败
	{
		m_hComm = NULL;
		return FALSE;
	}

	// set the timeout values
	CommTimeouts.ReadIntervalTimeout = 1000;
	CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	CommTimeouts.ReadTotalTimeoutConstant = 1000;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	CommTimeouts.WriteTotalTimeoutConstant = 1000;
	if (FALSE == SetCommTimeouts(m_hComm, &CommTimeouts))
	{
		m_hComm = NULL;
		return FALSE;
	}

	if (FALSE == SetupComm(m_hComm, 100, 100))
	{
		m_hComm = NULL;
		return FALSE;
	}

	if (FALSE == SetCommMask(m_hComm, dwCommEvents))
	{
		return FALSE;
	}
	if (FALSE == GetCommState(m_hComm, &CommDcb))
	{
		return FALSE;
	}
	CommDcb.EvtChar = 'q';
	CommDcb.fRtsControl = RTS_CONTROL_ENABLE;
	if (FALSE == BuildCommDCB(lpsBaud, &CommDcb))
	{
		return FALSE;
	}
	if (FALSE == SetCommState(m_hComm, &CommDcb))
	{
		return FALSE;
	}
	if (FALSE == PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT))
	{
		return FALSE;
	}
	return TRUE;
}

bool CSerialPort::ClosePort()
{
	if (FALSE == CloseHandle(m_hComm))
	{
		return FALSE;
	}
	m_hComm = NULL;
	return TRUE;
}

bool CSerialPort::WritePort(const char *pSendBuffer, int iLength)
{
	DWORD dwErrorFlags;
	DWORD dwBytesWrite = iLength;
	COMSTAT commStat;
	OVERLAPPED osWrite;

	memset(&osWrite, 0, sizeof(osWrite));
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ClearCommError(m_hComm, &dwErrorFlags, &commStat);
	BOOL bWriteStatus = WriteFile(m_hComm, pSendBuffer, dwBytesWrite, &dwBytesWrite, &osWrite);
	if (!bWriteStatus)
	{
		if (ERROR_IO_PENDING == GetLastError())
		{
			while (!GetOverlappedResult(m_hComm, &osWrite, &dwBytesWrite, TRUE))
			{
				if (ERROR_IO_INCOMPLETE == GetLastError())
				{
					continue;
				}
			}
		}
		else
		{
			dwBytesWrite = 0;
		}
	}
	return dwBytesWrite;
}

bool CSerialPort::ReadPort(int &iReadLen, char ReadBuffer[])
{
	DWORD dwErrorFlags = 0;
	DWORD dwBytesToRead = 0; //需要接收的数据长度
	DWORD dwBytesRead = 0; //实际接收的数据长度
	COMSTAT commStat;
	OVERLAPPED osReader;

	memset(&commStat, 0, sizeof(COMSTAT));
	memset(&osReader, 0, sizeof(osReader));
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (ClearCommError(m_hComm, &dwErrorFlags, &commStat))
	{
		dwBytesToRead = commStat.cbInQue;
	}

	if (!ReadFile(m_hComm, ReadBuffer, dwBytesToRead, &dwBytesRead, &osReader))
	{
		if (ERROR_IO_PENDING == GetLastError())
		{
			while (!GetOverlappedResult(m_hComm, &osReader, &dwBytesRead, FALSE))
			{
				if (ERROR_IO_INCOMPLETE == GetLastError())
				{
					continue;
				}
			}
		}
		else
		{
			dwBytesRead = 0;
		}
	}
	iReadLen = dwBytesRead;
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
	return dwBytesRead;
}
