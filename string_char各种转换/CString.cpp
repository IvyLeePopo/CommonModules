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
