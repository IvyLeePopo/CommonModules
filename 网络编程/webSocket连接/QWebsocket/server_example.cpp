#include "server/readerwebsocketserver.h"

// 创建一个服务器
bool StartReaderServer()
{
    ReaderWebsocketServer::getInstance();
    ReaderWebsocketServer::getInstance()->setCallBack(RecvMsgToSerial);
}

// 服务器发送数据
ReaderWebsocketServer::getInstance()->sendData(byteArray);


// 接收数据：声明一个回调函数，其实可以用信号槽代替
void RecvMsgToSerial(const QByteArray &byteArray)
{
    // todo 
}