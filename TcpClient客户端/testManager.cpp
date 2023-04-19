#include "testManager.h"
using namespace std;

#define DELETEOBJECT(_object) \
    if(NULL != _object) \
    {\
        delete _object; \
        _object = NULL; \
    }


testManager::testManager()
{
	init();
}

testManager::~testManager()
{
	DELETEOBJECT(m_pTcpClient);
    DELETEOBJECT(m_pRailingService);
}

void testManager::init()
{
	setType(ManagerType_GroundNode);//业务管理类统一管理
	m_pTcpClient = new TcpClient(this);

	m_pTcpClient->connectSvr(ip, port);
	if (m_pTcpClient->isConnected())
	{
		WriteLogInfo("tcp连接成功 testManager tcp connected");
		m_pTcpClient->login();
	}
	else
	{
		WriteLogErr("tcp连接失败 testManager tcp connect failed");
	}
}

//接收到指令--TCP分发消息过来
void testManager::cycle(void* pObject)
{
	if (nullptr == pObject)
	{
		WriteLog(EnumLogType_Err, "接收到的数据体为null", __LINE__, __FUNCTION__);
		return;
	}

	//todo 根据具体需求来
	//sendEvtMessage();//给服务器发送消息

	return;
}


void testManager::sendEvtMessage(string strData)
{
	tagTcpDataParams tagParam;//见协议的具体数据格式

	tagParam.strMsgId = Helper->getMsgId();
	tagParam.strMsgType = MSG_TYPE_EVENT;
    tagParam.strData = strData;
	m_pTcpClient->send(tagParam);
}

