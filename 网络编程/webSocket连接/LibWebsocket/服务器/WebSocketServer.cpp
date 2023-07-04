#include "StdAfx.h"
#include "WebSocketServer.h"

#include "Helper.h"
#include "TwProtocol.h"

void recvMsgFunc(void* pArgs, const char* pszData, int nSize)
{
	if (nullptr == pArgs || nullptr == pszData)
		return;

	WebSocketServer* pWebSocket = (WebSocketServer*)pArgs;
	pWebSocket->receiveData(pszData, nSize);
}

WebSocketServer::WebSocketServer(void)
{
	init();
}

WebSocketServer::~WebSocketServer(void)
{
	unInit();
}

int WebSocketServer::circle()
{
	int nTime = Helper->getCurrentTimeStamp();

	while(isCanRun())
	{
		int nCurTime = Helper->getCurrentTimeStamp();
		if (nCurTime - nTime > 10)
		{
			// 将读卡器返回的数据，原封不动发送给客户端（读卡器动态库）
			//WebSocketBaseServer->send(m_pszRecvDataReader, (int)strlen(m_pszRecvDataReader));
			nTime = nCurTime;
		}

		readFromReader();
		Sleep(20);
	}

	return TRUE;
}

bool WebSocketServer::startServer(/*const char* pszIp, const int nPort*/)
{
	WebSocketBaseServer->startSever();
	
	vStart();
	return true;
}

void WebSocketServer::stopServer()
{
	WebSocketBaseServer->stopServer();
	vStop();
}

bool WebSocketServer::sendData(const char* pszJson, int nSize)
{
	bool bRet = WebSocketBaseServer->send(pszJson, nSize);
	return bRet;
}

bool WebSocketServer::receiveData(const char* pszData, int nSize)
{
	/*
	* 1、接收到客户端的数据，通过串口直接发给读卡器
	* 2、读卡器收到指令，将执行指令后的结果返回
	* 3、将返回的结果，通过sendData发给客户端
	*/
	int a = 0;
	string pRspDataJson;
	eSubCmdType_t eSubCmdType_tTemp;
	bool bParseResult = TW_Protocol::ParseProtocolData((const u8 *)pszData, nSize, eSubCmdType_tTemp, pRspDataJson);

	pRspDataJson.clear();
	u32 nReqLen = 10;
	u8* pReqData = new u8[nReqLen + 1];
	switch (eSubCmdType_tTemp)
	{
	case eSubCmdType_t::SUB_CMD_TYPE_OpenReader:		//打开 IC 卡读写器
		TW_Protocol::generateOpenReaderRespJson(pRspDataJson);
		nReqLen += pRspDataJson.size();

		delete[] pReqData;
		pReqData = NULL;
		pReqData = new u8[nReqLen + 1];
		memset(pReqData, 0, nReqLen + 1);
		TW_Protocol::ConstructFrame(eSubCmdType_tTemp, pRspDataJson, pReqData, nReqLen);//pcvData 请求的结构
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_CloseReader:		//关闭 IC 卡读写器
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_SetInitPPS:		//设置 IC 卡读写器通讯速率（可选）
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_SetInitTimeOut:    //设置 IC 卡读写器超时时间（可选）
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_LEDDisplay:        //设置读写器发光二极管
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_AudioControl:      //设置读写器蜂鸣器
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_GetStatus:         //获取设备状态
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_ReaderVersion:     //获取设备版本信息
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_GetVersion:        //获取设备动态库版本
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_GetStatusMsg:      //获取错误码详细描述
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_OpenCard:			//打开卡片
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_CloseCard:         //关闭卡片
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_CPUCommand:        //CPU 卡通用指令函数
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_SamReset:          //SAM 卡复位
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_SamCommand:        //SAM 卡通用指令函数
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_LoadKey:           //M1 卡载入密钥（可选）
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_ReadBlock:         //M1 卡读块函数（可选）
		break;
	case eSubCmdType_t::SUB_CMD_TYPE_WriteBlock:        //M1 卡写块函数（可选）
		break;
	default:
		break;
	}

	sendData((const char*)pReqData, nReqLen);
	return true;
}

// 接收读卡器从串口传送过来的数据
void WebSocketServer::readFromReader()
{
	// todo
	m_pszRecvDataReader = "hello";
}

void WebSocketServer::init()
{
	WebSocketBaseServer->setCallBack(recvMsgFunc, (void*)this);
}

void WebSocketServer::unInit()
{	
}