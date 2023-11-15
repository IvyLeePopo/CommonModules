#include "client/WsReaderClient.h"

//创建客户端
WsReaderClient::getInstance();

//发送数据
WsReaderClient::getInstance()->SendTo(byteArray);

//接收数据：也可以通过信号槽，不过此处是阻塞等待就直接获取数据
QByteArray recvArray;
WsReaderClient::getInstance()->RecvFrom(recvArray);