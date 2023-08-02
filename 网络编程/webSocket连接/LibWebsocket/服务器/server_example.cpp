#include "Websocket/WebSocketServer.h"

// 实例演示：包含该类的头文件
// 创建读卡器动态库的服务器;  
WebSocketServer* m_pWebSocketServerReader;


// 接收 websocket客户端发来的数据，转发到需要的地方去
void RecvMsgToSerial(const char* pszData, unsigned int nSize)
{
	if (NULL == pszData)
		return;

	RecvMsgFuncFromWebsocket(pszData, nSize);
}

// 接收别处地方发来的数据，通过服务器转发给客户端
void RecvMsgFromSerial(const char* pszData, const int nSize)
{
	if (NULL == pszData)
		return;

	RecvMsgFuncToWebsocket(pszData, nSize);
}



//开启服务器
bool StartReaderServer()
{
	if (!m_pWebSocketServerReader)
	{
		m_pWebSocketServerReader = new WebSocketServer();
		m_pWebSocketServerReader->startServer();
		m_pWebSocketServerReader->setCallBack(RecvMsgToSerial);
	}

	return true;
}


bool RecvMsgFuncFromWebsocket(const char* pszData, unsigned int nSize)
{
	// 接收客户端的信息，转发给读卡器
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strLog;
	bool bRet = false;

	try
	{
		if (m_pThreadManage == NULL)
			throw - 1;

		if (m_pThreadManage->m_pThreadDevice == NULL)
			throw - 2;

		CString strLog;

		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			CString str;
			str.Format(_T("%02X"), pszData[nIndex] & 0xFF);
			strLog += str;
		}
		RecordLog(strLog);

		bRet = theApp.m_pThreadManage->m_pThreadDevice->EtcEventWebsocketControl(pszData, nSize);

		if (bRet)
			strLog.Format(_T("[回调函数中RecvMsgFuncFromWebsocket], 接口EtcEventWebsocketControl 返回结果：成功"));
		else
			strLog.Format(_T("[回调函数中RecvMsgFuncFromWebsocket], 接口EtcEventWebsocketControl 返回结果：失败"));

		RecordLog(strLog);

	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("RecvMsgFuncFromWebsocket,业务管理线程资源为空！"));
			break;
		case -2:
			strLog.Format(_T("RecvMsgFuncFromWebsocket,设备控制线程资源为空！"));
			break;
		}

		RecordLog(strLog);
	}

	return bRet;

}

bool RecvMsgFuncToWebsocket(const char* pszData, unsigned int nSize)
{
	bool bRet = m_pWebSocketServerReader->sendData(pszData, nSize);
	return bRet;
}
