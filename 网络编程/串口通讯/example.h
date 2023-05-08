#include "CSocket_GT.h"

// 实例演示：包含该类的头文件

class CMotorModbus
{
public:
	CMotorModbus();
	virtual ~CMotorModbus();

public:
	bool OpenPort(int iPort, int iBaud, QString strParity, int iDatabit, int iStopbit, DWORD dwCommEvents);
	bool ClosePort();

	bool ReadData(long szReadBuf[]);
	bool WriteData(int iIndex, QString strCmd);

private:
	QString GetWriteData(int iIndex, QString strWriteData_Input);
	QString GetReadData();

	void Modbus_calc();

	void CRC16_send_calc();
	void CRC16_read_calc();
	byte GetCrcHigh(int iIndex);
	byte GetCrcLow(int iIndex);
private:
	CSerialPort *m_pSeriaPort;

	CRC16_type m_nCrc16_send;

	CRC16_type m_nCrc16_read;
};