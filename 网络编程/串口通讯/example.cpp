#include "example.h"


CMotorModbus::CMotorModbus()
{
	m_pSeriaPort = new CSerialPort();
}


CMotorModbus::~CMotorModbus()
{
	if (NULL != m_pSeriaPort)
	{
		double m_pSeriaPort;
		m_pSeriaPort = NULL;
	}
}

QString CMotorModbus::GetWriteData(int iIndex, QString strWriteData_Input)
{
	QString strWriteData_Output;
	byte send_byte;
	m_nCrc16_send.result_high = 0xFF;
	m_nCrc16_send.result_low = 0xFF;

	send_byte = 1;
	strWriteData_Output = send_byte; //地址位
	m_nCrc16_send.data_in = send_byte;
	CRC16_send_calc();

	long iData_Input = strWriteData_Input.toInt();

	send_byte = 6;
	strWriteData_Output += send_byte;
	m_nCrc16_send.data_in = send_byte;
	CRC16_send_calc();

	send_byte = 0;
	strWriteData_Output += send_byte;
	m_nCrc16_send.data_in = send_byte;
	CRC16_send_calc();

	if (iIndex == 12)
	{
		send_byte = 20;
	}
	else if (iIndex == 16)
	{
		send_byte = 24;
	}
	else if (iIndex == 17)
	{
		send_byte = 25;
	}
	else
	{
		send_byte = iIndex;
	}
	strWriteData_Output += send_byte;
	m_nCrc16_send.data_in = send_byte;
	CRC16_send_calc();

	if (iData_Input < 0)
	{
		iData_Input = 0x7FFF + iData_Input + 1;
		send_byte = iData_Input / 256 + 0x80;
	}
	else
	{
		send_byte = iData_Input / 256;
	}
	strWriteData_Output += send_byte;
	m_nCrc16_send.data_in = send_byte;
	CRC16_send_calc();

	send_byte = iData_Input % 256;
	strWriteData_Output += send_byte;
	m_nCrc16_send.data_in = send_byte;
	CRC16_send_calc();

	send_byte = m_nCrc16_send.result_high;
	strWriteData_Output += send_byte;
	send_byte = m_nCrc16_send.result_low;
	strWriteData_Output += send_byte;

	return strWriteData_Output;
}

QString CMotorModbus::GetReadData()
{
	QString strReadData_Output;
	byte read_byte;
	m_nCrc16_send.result_high = 0xFF;
	m_nCrc16_send.result_low = 0xFF;

	read_byte = 1;
	strReadData_Output = read_byte;
	m_nCrc16_send.data_in = read_byte;
	CRC16_send_calc();

	read_byte = 3;
	strReadData_Output += read_byte;
	m_nCrc16_send.data_in = read_byte;
	CRC16_send_calc();

	read_byte = 0;
	strReadData_Output += read_byte;
	m_nCrc16_send.data_in = read_byte;
	CRC16_send_calc();

	read_byte = 0;
	strReadData_Output += read_byte;
	m_nCrc16_send.data_in = read_byte;
	CRC16_send_calc();

	read_byte = 0;
	strReadData_Output += read_byte;
	m_nCrc16_send.data_in = read_byte;
	CRC16_send_calc();

	read_byte = modbus_read_register_num;
	strReadData_Output += read_byte;
	m_nCrc16_send.data_in = read_byte;
	CRC16_send_calc();

	read_byte = m_nCrc16_send.result_high;
	strReadData_Output += read_byte;
	read_byte = m_nCrc16_send.result_low;
	strReadData_Output += read_byte;

	return strReadData_Output;
}

void CMotorModbus::CRC16_send_calc()
{
	int iIndex = m_nCrc16_send.result_high ^ m_nCrc16_send.data_in;
	m_nCrc16_send.result_high = m_nCrc16_send.result_low ^ GetCrcHigh(iIndex);
	m_nCrc16_send.result_low = GetCrcLow(iIndex);
}

byte CMotorModbus::GetCrcHigh(int iIndex)
{
	return bzCrc_HighParam[iIndex];
}

byte CMotorModbus::GetCrcLow(int iIndex)
{
	return bzCrc_LowParam[iIndex];
}

bool CMotorModbus::OpenPort(int iPort, int iBaud, QString strParity, int iDatabit, int iStopbit, DWORD dwCommEvents)
{
	if (NULL != m_pSeriaPort)
	{
		if (m_pSeriaPort->InitPort(iPort, iBaud, strParity, iDatabit, iStopbit, dwCommEvents))
		{
			return true;
		}
	}
	return false;
}

bool CMotorModbus::ClosePort()
{
	if (NULL != m_pSeriaPort)
	{
		if (m_pSeriaPort->ClosePort())
		{
			return true;
		}
	}
	return false;
}

bool CMotorModbus::WriteData(int iIndex, QString strCmd)
{
	if (NULL != m_pSeriaPort)
	{
		QString strTemp = GetWriteData(iIndex, strCmd);
		if (m_pSeriaPort->WritePort(strTemp.toLatin1(), strTemp.length()))
		{
			return true;
		}
	}
	return false;
}

void CMotorModbus::Modbus_calc()
{
	switch (m_nModbus.iStatus)
	{
	case 1: //设备地址
		m_nModbus.iDevice_addr = m_nModbus.iData_Input;
		if (m_nModbus.iDevice_addr == 1)
		{
			m_nModbus.iStatus = 2;
			m_nCrc16_read.result_high = 0xFF;
			m_nCrc16_read.result_low = 0xFF;
			m_nCrc16_read.data_in = m_nModbus.iData_Input;
			CRC16_read_calc();
		}
		break;
	case 2: //功能码
		m_nModbus.iFunction_code = m_nModbus.iData_Input;
		if (m_nModbus.iFunction_code == 3)
		{
			m_nModbus.iStatus = 3;
			m_nCrc16_read.data_in = m_nModbus.iData_Input;
			CRC16_read_calc();
		}
		else
		{
			m_nModbus.iStatus = 1;
			m_nCrc16_read.result_high = 0xFF;
			m_nCrc16_read.result_low = 0xFF;
		}
		break;
	case 3: //数据长度
		m_nModbus.iData_len = m_nModbus.iData_Input;
		m_nModbus.iStatus = 4;
		m_nModbus.iIndex = 0;
		m_nCrc16_read.data_in = m_nModbus.iData_Input;
		CRC16_read_calc();
		break;
	case 4: //数据位高位
		m_nModbus.bzData[m_nModbus.iIndex] = m_nModbus.iData_Input;
		m_nModbus.bzData[m_nModbus.iIndex] = m_nModbus.iData_Input * 256;
		m_nModbus.iStatus = 5;
		m_nCrc16_read.data_in = m_nModbus.iData_Input;
		CRC16_read_calc();
		break;
	case 5: //数据位低位
		m_nModbus.bzData[m_nModbus.iIndex] = m_nModbus.bzData[m_nModbus.iIndex] + m_nModbus.iData_Input;
		m_nModbus.iStatus = 4;
		m_nCrc16_read.data_in = m_nModbus.iData_Input;
		CRC16_read_calc();
		m_nModbus.iIndex = m_nModbus.iIndex + 1;
		if (m_nModbus.iIndex >= m_nModbus.iData_len / 2)
		{
			m_nModbus.iStatus = 6;
		}
		break;
	case 6: //CRC高位
		if (m_nCrc16_read.result_high == m_nModbus.iData_Input)
		{
			m_nModbus.iStatus = 7;
		}
		break;
	case 7: //CRC低位
		if (m_nCrc16_read.result_low == m_nModbus.iData_Input)
		{
			if (m_nModbus.iFunction_code == 3)
			{

			}
		}
		break;
	default:
		break;
	}
}

void CMotorModbus::CRC16_read_calc()
{
	int iIndex = m_nCrc16_read.result_high ^ m_nCrc16_read.data_in;
	m_nCrc16_read.result_high = m_nCrc16_read.result_low ^ GetCrcHigh(iIndex);
	m_nCrc16_read.result_low = GetCrcLow(iIndex);
}

bool CMotorModbus::ReadData(long szReadBuf[])
{
	if (NULL != m_pSeriaPort)
	{
		QString strReadData = GetReadData();
		if (m_pSeriaPort->WritePort(strReadData.toLatin1(), strReadData.length()))
		{
			int iRead_len = 0;
			char szRecvBuffer[8192] = { 0 };
			//初始化电机驱动信息
			memset(&m_nModbus, 0, sizeof(m_nModbus));
			m_nModbus.iStatus = 1;


			if (m_pSeriaPort->ReadPort(iRead_len, szRecvBuffer))
			{
				if (iRead_len >= 100)
				{
					iRead_len = 100;
				}

				for (int i = 0; i < iRead_len; i++)
				{
					m_nModbus.iData_Input = szRecvBuffer[i];
					Modbus_calc();
				}

				for (int i = 0; i < iRead_len; i++)
				{
					szReadBuf[i] = m_nModbus.bzData[i];
				}
				return true;
			}
		}
	}
	return false;
}
