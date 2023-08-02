#include "Websocket/WebSocketClient.h"

// 实例演示：包含该类的头文件
// 创建客户端;  
WebSocketClientInterface *m_pcReaderInterface = NULL;//socket的客户端 or serial的接口

// 连接服务器
m_pcReaderInterface->TestAndReconnect()；

// 发送数据给服务器
m_pcReaderInterface->SendTo();

// 现场等待
int iRet = 0, iWaitMs;//看是否需要超时等待
while (m_pcReaderInterface->IsConnected()) //pRspData 应答数据
{
	// 等待服务器返回数据
	iRet = m_pcReaderInterface->RecvFrom(NULL, NULL, ucBuf + nRspLen, sizeof(ucBuf) - nRspLen, iWaitMs);

	// 将接收到的数据进行检测，看是不是符合要求的数据
	// 符合的话可以直接退出循环
}