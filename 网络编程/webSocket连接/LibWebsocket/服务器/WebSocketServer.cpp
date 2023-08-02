#include "StdAfx.h"
#include "WebSocketServer.h"

#include "Helper.h"
#include "WebSocketBase.h"

void recvMsgFunc(void* pArgs, const char* pszData, unsigned int nSize)
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
}

bool WebSocketServer::startServer()
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

bool WebSocketServer::sendData(const char* pszJson, unsigned int nSize)
{
	bool bRet = WebSocketBaseServer->send(pszJson, nSize);
	return bRet;
}

bool WebSocketServer::receiveData(const char* pszData, unsigned int nSize)
{
	/*
	* 1、接收到客户端的数据，通过串口直接发给读卡器
	* 2、读卡器收到指令，将执行指令后的结果返回
	* 3、将返回的结果，通过sendData发给客户端
	*/
	m_pRecvMsgToSerial(pszData, nSize);
	return true;
}

void WebSocketServer::setCallBack(recvMsgToSerial pFunc, void* pUser)
{
	if (nullptr != pFunc)
		m_pRecvMsgToSerial = pFunc;

	if (nullptr != pUser)
		m_pUser = pUser;
}

void WebSocketServer::init()
{
	WebSocketBaseServer->setCallBack(recvMsgFunc, (void*)this);
}