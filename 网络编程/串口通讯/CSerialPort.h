
#include <qt_windows.h>
#include <QString>

#pragma once


class CSerialPort
{
public:
	CSerialPort();
	virtual ~CSerialPort();

public:
	bool InitPort(int iPort, int iBaud, QString strParity, int iDatabit, int iStopbit, DWORD dwCommEvents);
	bool ClosePort();

	bool ReadPort(int &iReadLen, char ReadBuffer[]);
	bool WritePort(const char *pSendBuffer, int iLength);
private:
	HANDLE m_hComm;
};

