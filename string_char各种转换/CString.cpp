#include "CString.h"

void printfCString()
{
	CString strLog1;
	int nDstLen = 141;
	char pcucDstData[nDstLen] = { "DF003C000058017B224F70656E526561646572223A7B226E4D6F6465223A2232222C22735061726173223A223139322E3136382E3130312E3235302C39303030227D7D0696FD" };
	for (int nIndex = 0; nIndex < nDstLen; nIndex++)
	{
		CString str;
		//strLog.Format(_T("[发往串口动态库],往串口发送的数据是："));
		str.Format("%02X", pcucDstData[nIndex] & 0xFF);
		strLog1 += str;
	}
}

//读卡器发出来的指令是 char* 
//1、转化为十六进制的字符，打印出来
void printf_char2HexString()
{
	CString strLog;
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CString str;
		str.Format(_T("%02X"), pszData[nIndex] & 0xFF);
		strLog += str;
	}
}

//CString转char*
const char* szStr = strLog.GetString();

//char*转string
std::string cmd_data(szStr);

//数据格式之间的转换：
//1、QByteArray转vector
void QByteArray2vector()
{
	vector<unsigned int> m_VerData;
	QByteArray array;
	m_VerData.insert(m_VerData.end(), array.begin(), array.end());
}

//2、vector转QByteArray
void vector2QByteArray()
{
	vector<unsigned int> m_VerData;
	QByteArray array(reinterpret_cast<const char*>(m_VerData.data()), static_cast<int>(m_VerData.size()));
}

