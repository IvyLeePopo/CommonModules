#ifndef READERWEBSOCKETSERVER_H
#define READERWEBSOCKETSERVER_H

#include <QObject>
#include "Singleton.h"


// 回调函数，将websocket的结果抛给上层
typedef void(*recvMsgToSerial)(const char* pszData, unsigned int  nSize);

class WebsocketServerBase;
class ReaderWebsocketServer : public QObject, public Singleton<ReaderWebsocketServer>
{
    Q_OBJECT
    friend class Singleton<ReaderWebsocketServer>;

public:
    explicit ReaderWebsocketServer(QObject *parent = 0);
    virtual ~ReaderWebsocketServer();

public:
    // 设置回调函数
    void setCallBack(recvMsgToSerial pFunc, void* pUser = NULL);

    //接收串口发来数据，转发给读卡器客户端
    bool sendData(const QByteArray &byteArray);

signals:
	//转发数据给读卡器
    void sigSendToAllClients(const QByteArray &data);

private slots:
    //处理读卡器发来的数据，转发给串口
    void slotProcessServerMessage(const QByteArray &data);

private:
    WebsocketServerBase* m_pWsServer;
    QThread* m_thdWsServer;

    void* m_pUser;							// 返回回调的对象
    recvMsgToSerial m_pRecvMsgToSerial;		// 回调
};

#endif // READERWEBSOCKETSERVER_H
