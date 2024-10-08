#include "CString.h"

void printfCString()
{
	CString strLog1;
	int nDstLen = 141;
	char pcucDstData[nDstLen] = { "DF003C000058017B224F70656E526561646572223A7B226E4D6F6465223A2232222C22735061726173223A223139322E3136382E3130312E3235302C39303030227D7D0696FD" };
	for (int nIndex = 0; nIndex < nDstLen; nIndex++)
	{
		CString str;
		//strLog.Format(_T("[�������ڶ�̬��],�����ڷ��͵������ǣ�"));
		str.Format("%02X", pcucDstData[nIndex] & 0xFF);
		strLog1 += str;
	}
}

//��������������ָ���� char* 
//1��ת��Ϊʮ�����Ƶ��ַ�����ӡ����
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

//CStringתchar*
const char* szStr = strLog.GetString();

//char*תstring
std::string cmd_data(szStr);

//���ݸ�ʽ֮���ת����
//1��QByteArrayתvector
void QByteArray2vector()
{
	vector<unsigned int> m_VerData;
	QByteArray array;
	m_VerData.insert(m_VerData.end(), array.begin(), array.end());
}

//2��vectorתQByteArray
void vector2QByteArray()
{
	vector<unsigned int> m_VerData;
	QByteArray array(reinterpret_cast<const char*>(m_VerData.data()), static_cast<int>(m_VerData.size()));
}

