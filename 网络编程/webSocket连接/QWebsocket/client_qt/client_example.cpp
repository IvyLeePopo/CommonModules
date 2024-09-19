#include "WebSocketClient.h"

//创建客户端
WebSocketClient* m_pSocket;
m_pSocket = new WebSocketClient();

//连接服务器
m_pSocket->setIp("172.32.202.27");
m_pSocket->setPort("9000");
m_pSocket->connectSvr();

//发送数据
m_pSocket->send("hello");

//接收数据：也可以通过信号槽，不过此处是阻塞等待就直接获取数据


//销毁
if (nullptr != m_pSocket)
delete m_pSocket;